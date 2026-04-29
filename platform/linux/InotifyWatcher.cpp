/*
 * InotifyWatcher.cpp
 */

#include <sys/inotify.h>
#include <unistd.h>

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

namespace softadastra::fs::watcher
{
  namespace events = softadastra::fs::events;
  namespace path = softadastra::fs::path;
  namespace state = softadastra::fs::state;
  namespace types = softadastra::fs::types;
  namespace core_errors = softadastra::core::errors;

  class InotifyWatcher : public IWatcherBackend
  {
  public:
    using Callback = IWatcherBackend::Callback;
    using Result = IWatcherBackend::Result;

    InotifyWatcher() = default;

    ~InotifyWatcher() override
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

      fd_ = inotify_init1(IN_NONBLOCK);
      if (fd_ < 0)
      {
        return Result::err(
            core_errors::Error::make(
                core_errors::ErrorCode::InternalError,
                "failed to initialize inotify"));
      }

      wd_ = inotify_add_watch(
          fd_,
          root_.str().c_str(),
          IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);

      if (wd_ < 0)
      {
        close_fd();

        return Result::err(
            core_errors::Error::make(
                core_errors::ErrorCode::InternalError,
                "failed to add inotify watch",
                core_errors::ErrorContext(root_.str())));
      }

      running_ = true;

      worker_ = std::thread([this]
                            { loop(); });

      return Result::ok();
    }

    void stop() override
    {
      if (!running_)
      {
        return;
      }

      running_ = false;

      if (worker_.joinable())
      {
        worker_.join();
      }

      close_watch();
      close_fd();
    }

    [[nodiscard]] bool is_running() const noexcept override
    {
      return running_;
    }

  private:
    void loop()
    {
      std::vector<char> buffer(4096);

      while (running_)
      {
        const auto length = ::read(fd_, buffer.data(), buffer.size());

        if (length <= 0)
        {
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
          continue;
        }

        events::EventBatch batch;

        std::size_t offset = 0;

        while (offset < static_cast<std::size_t>(length))
        {
          const auto *event =
              reinterpret_cast<const inotify_event *>(buffer.data() + offset);

          handle_event(*event, batch);

          offset += sizeof(inotify_event) + event->len;
        }

        if (!batch.empty() && callback_)
        {
          callback_(batch);
        }
      }
    }

    void handle_event(const inotify_event &event, events::EventBatch &batch)
    {
      if (event.len == 0)
      {
        return;
      }

      auto type = event_type(event.mask);

      if (!type.has_value())
      {
        return;
      }

      const std::string full_path = root_.str() + "/" + event.name;

      auto path_result = path::Path::from(full_path);

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
    event_type(std::uint32_t mask) noexcept
    {
      if ((mask & IN_CREATE) || (mask & IN_MOVED_TO))
      {
        return types::FileEventType::Created;
      }

      if (mask & IN_MODIFY)
      {
        return types::FileEventType::Updated;
      }

      if ((mask & IN_DELETE) || (mask & IN_MOVED_FROM))
      {
        return types::FileEventType::Deleted;
      }

      return std::nullopt;
    }

    void close_watch() noexcept
    {
      if (wd_ >= 0 && fd_ >= 0)
      {
        inotify_rm_watch(fd_, wd_);
        wd_ = -1;
      }
    }

    void close_fd() noexcept
    {
      if (fd_ >= 0)
      {
        close(fd_);
        fd_ = -1;
      }
    }

  private:
    path::Path root_{};
    Callback callback_{};

    int fd_{-1};
    int wd_{-1};

    std::thread worker_{};
    std::atomic<bool> running_{false};
  };

} // namespace softadastra::fs::watcher
