/*
 * InotifyWatcher.cpp
 */

#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>

#include <thread>
#include <atomic>
#include <vector>
#include <cstring>
#include <optional>

#include <softadastra/fs/watcher/IWatcherBackend.hpp>
#include <softadastra/fs/events/EventBatch.hpp>
#include <softadastra/fs/events/FileEvent.hpp>
#include <softadastra/fs/types/FileEventType.hpp>

namespace softadastra::fs::watcher
{
  namespace events = softadastra::fs::events;
  namespace types = softadastra::fs::types;
  namespace result = softadastra::core::types;
  namespace errors = softadastra::core::errors;

  class InotifyWatcher : public IWatcherBackend
  {
  public:
    using Callback = IWatcherBackend::Callback;
    using Result = IWatcherBackend::Result;

    InotifyWatcher() = default;

    ~InotifyWatcher()
    {
      stop();
    }

    Result start(const path::Path &root, Callback callback) override
    {
      if (running_)
      {
        return Result::err(errors::Error(
            errors::ErrorCode::InvalidState,
            errors::Severity::Error,
            "Watcher already running"));
      }

      root_ = root;
      callback_ = std::move(callback);

      fd_ = inotify_init1(IN_NONBLOCK);
      if (fd_ < 0)
      {
        return Result::err(errors::Error(
            errors::ErrorCode::InternalError,
            errors::Severity::Error,
            "Failed to init inotify"));
      }

      wd_ = inotify_add_watch(fd_, root_.str().c_str(),
                              IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);

      if (wd_ < 0)
      {
        close(fd_);
        return Result::err(errors::Error(
            errors::ErrorCode::InternalError,
            errors::Severity::Error,
            "Failed to add watch"));
      }

      running_ = true;
      worker_ = std::thread([this]()
                            { loop(); });

      return Result::ok();
    }

    void stop() override
    {
      if (!running_)
        return;

      running_ = false;

      if (worker_.joinable())
      {
        worker_.join();
      }

      if (wd_ >= 0)
      {
        inotify_rm_watch(fd_, wd_);
      }

      if (fd_ >= 0)
      {
        close(fd_);
      }
    }

    bool is_running() const noexcept override
    {
      return running_;
    }

  private:
    void loop()
    {
      std::vector<char> buffer(4096);

      while (running_)
      {
        int length = read(fd_, buffer.data(), buffer.size());

        if (length <= 0)
        {
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
          continue;
        }

        events::EventBatch batch;

        int i = 0;
        while (i < length)
        {
          auto *event = reinterpret_cast<struct inotify_event *>(&buffer[i]);

          if (event->len > 0)
          {
            std::string full_path = root_.str() + "/" + event->name;

            auto path_res = path::Path::from(full_path);
            if (path_res.is_err())
            {
              i += sizeof(struct inotify_event) + event->len;
              continue;
            }

            state::FileState current{path_res.value()};

            types::FileEventType type;
            bool valid = true;

            if ((event->mask & IN_CREATE) || (event->mask & IN_MOVED_TO))
            {
              type = types::FileEventType::Created;
            }
            else if (event->mask & IN_MODIFY)
            {
              type = types::FileEventType::Updated;
            }
            else if ((event->mask & IN_DELETE) || (event->mask & IN_MOVED_FROM))
            {
              type = types::FileEventType::Deleted;
            }
            else
            {
              valid = false;
            }

            if (valid)
            {
              events::FileEvent ev{
                  type,
                  current,
                  std::nullopt};

              batch.add(std::move(ev));
            }
          }

          i += sizeof(struct inotify_event) + event->len;
        }

        if (!batch.empty() && callback_)
        {
          callback_(batch);
        }
      }
    }

  private:
    path::Path root_;
    Callback callback_;

    int fd_{-1};
    int wd_{-1};

    std::thread worker_;
    std::atomic<bool> running_{false};
  };

} // namespace softadastra::fs::watcher
