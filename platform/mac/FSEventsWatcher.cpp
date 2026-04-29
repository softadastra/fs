/*
 * FSEventsWatcher.cpp
 */

#include <CoreServices/CoreServices.h>

#include <atomic>
#include <chrono>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include <softadastra/core/Core.hpp>
#include <softadastra/fs/events/EventBatch.hpp>
#include <softadastra/fs/events/FileEvent.hpp>
#include <softadastra/fs/path/Path.hpp>
#include <softadastra/fs/state/FileState.hpp>
#include <softadastra/fs/types/FileEventType.hpp>
#include <softadastra/fs/watcher/IWatcherBackend.hpp>

#include <softadastra/fs/watcher/PollingWatcher.hpp>

#ifdef SOFTADASTRA_FS_LINUX
#include <softadastra/fs/watcher/InotifyWatcher.hpp>
#endif

#ifdef SOFTADASTRA_FS_MAC
#include <softadastra/fs/watcher/FSEventsWatcher.hpp>
#endif

#ifdef SOFTADASTRA_FS_WINDOWS
#include <softadastra/fs/watcher/WinWatcher.hpp>
#endif

namespace softadastra::fs::watcher
{

  namespace events = softadastra::fs::events;
  namespace path = softadastra::fs::path;
  namespace state = softadastra::fs::state;
  namespace types = softadastra::fs::types;
  namespace core_errors = softadastra::core::errors;

  class FSEventsWatcher : public IWatcherBackend
  {
  public:
    using Callback = IWatcherBackend::Callback;
    using Result = IWatcherBackend::Result;

    FSEventsWatcher() = default;

    ~FSEventsWatcher() override
    {
      stop();
    }

    [[nodiscard]] Result start(const path::Path &root, Callback callback) override
    {
      if (running_)
      {
        return Result::err(
            core_errors::Error::make(
                core_errors::ErrorCode::InvalidState,
                "watcher already running"));
      }

      root_ = root;
      callback_ = std::move(callback);

      CFStringRef path_ref =
          CFStringCreateWithCString(
              nullptr,
              root_.str().c_str(),
              kCFStringEncodingUTF8);

      if (!path_ref)
      {
        return Result::err(
            core_errors::Error::make(
                core_errors::ErrorCode::InternalError,
                "failed to create FSEvents path reference"));
      }

      CFArrayRef paths =
          CFArrayCreate(
              nullptr,
              reinterpret_cast<const void **>(&path_ref),
              1,
              &kCFTypeArrayCallBacks);

      CFRelease(path_ref);

      if (!paths)
      {
        return Result::err(
            core_errors::Error::make(
                core_errors::ErrorCode::InternalError,
                "failed to create FSEvents path array"));
      }

      FSEventStreamContext context{};
      context.info = this;

      stream_ = FSEventStreamCreate(
          nullptr,
          &FSEventsWatcher::callback,
          &context,
          paths,
          kFSEventStreamEventIdSinceNow,
          0.2,
          kFSEventStreamCreateFlagFileEvents |
              kFSEventStreamCreateFlagNoDefer);

      CFRelease(paths);

      if (!stream_)
      {
        return Result::err(
            core_errors::Error::make(
                core_errors::ErrorCode::InternalError,
                "failed to create FSEvents stream",
                core_errors::ErrorContext(root_.str())));
      }

      running_ = true;

      worker_ = std::thread([this]
                            { run_loop(); });

      return Result::ok();
    }

    void stop() override
    {
      if (!running_)
      {
        return;
      }

      running_ = false;

      if (stream_)
      {
        FSEventStreamStop(stream_);
      }

      if (worker_.joinable())
      {
        worker_.join();
      }

      destroy_stream();
    }

    [[nodiscard]] bool is_running() const noexcept override
    {
      return running_;
    }

  private:
    void run_loop()
    {
      run_loop_ = CFRunLoopGetCurrent();

      FSEventStreamScheduleWithRunLoop(
          stream_,
          run_loop_,
          kCFRunLoopDefaultMode);

      if (!FSEventStreamStart(stream_))
      {
        running_ = false;
        return;
      }

      while (running_)
      {
        CFRunLoopRunInMode(
            kCFRunLoopDefaultMode,
            0.1,
            true);
      }

      FSEventStreamUnscheduleFromRunLoop(
          stream_,
          run_loop_,
          kCFRunLoopDefaultMode);

      run_loop_ = nullptr;
    }

    static void callback(
        ConstFSEventStreamRef,
        void *client_info,
        std::size_t num_events,
        void *event_paths,
        const FSEventStreamEventFlags event_flags[],
        const FSEventStreamEventId[])
    {
      auto *self = static_cast<FSEventsWatcher *>(client_info);

      if (!self)
      {
        return;
      }

      auto **paths = static_cast<char **>(event_paths);

      events::EventBatch batch;

      for (std::size_t i = 0; i < num_events; ++i)
      {
        self->handle_event(paths[i], event_flags[i], batch);
      }

      if (!batch.empty() && self->callback_)
      {
        self->callback_(batch);
      }
    }

    void handle_event(
        const char *raw_path,
        FSEventStreamEventFlags flags,
        events::EventBatch &batch)
    {
      if (!raw_path)
      {
        return;
      }

      auto type = event_type(flags);

      if (!type.has_value())
      {
        return;
      }

      auto path_result = path::Path::from(std::string(raw_path));

      if (path_result.is_err())
      {
        return;
      }

      state::FileState current{};
      current.path = path_result.value();

      batch.add(events::FileEvent{
          *type,
          current,
          std::nullopt});
    }

    [[nodiscard]] static std::optional<types::FileEventType>
    event_type(FSEventStreamEventFlags flags) noexcept
    {
      if (flags & kFSEventStreamEventFlagItemRemoved)
      {
        return types::FileEventType::Deleted;
      }

      if (flags & kFSEventStreamEventFlagItemCreated)
      {
        return types::FileEventType::Created;
      }

      if ((flags & kFSEventStreamEventFlagItemModified) ||
          (flags & kFSEventStreamEventFlagItemRenamed) ||
          (flags & kFSEventStreamEventFlagItemInodeMetaMod) ||
          (flags & kFSEventStreamEventFlagItemChangeOwner) ||
          (flags & kFSEventStreamEventFlagItemXattrMod))
      {
        return types::FileEventType::Updated;
      }

      return std::nullopt;
    }

    void destroy_stream() noexcept
    {
      if (stream_)
      {
        FSEventStreamInvalidate(stream_);
        FSEventStreamRelease(stream_);
        stream_ = nullptr;
      }
    }

  private:
    path::Path root_{};
    Callback callback_{};

    FSEventStreamRef stream_{nullptr};
    CFRunLoopRef run_loop_{nullptr};

    std::thread worker_{};
    std::atomic<bool> running_{false};
  };

} // namespace softadastra::fs::watcher
