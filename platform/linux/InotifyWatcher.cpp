/*
 * InotifyWatcher.cpp
 */

#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>

#include <thread>
#include <atomic>
#include <vector>

#include <softadastra/fs/watcher/IWatcherBackend.hpp>
#include <softadastra/fs/snapshot/SnapshotBuilder.hpp>
#include <softadastra/fs/snapshot/SnapshotDiff.hpp>
#include <softadastra/fs/events/EventBatch.hpp>

namespace softadastra::fs::watcher
{
  namespace fs = std::filesystem;
  namespace snapshot = softadastra::fs::snapshot;
  namespace events = softadastra::fs::events;
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

      auto initial = snapshot::SnapshotBuilder::build(root_);
      if (initial.is_err())
      {
        return Result::err(initial.error());
      }

      current_snapshot_ = std::move(initial.value());

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
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          continue;
        }

        // On ne traite pas chaque event individuellement
        // → on rescan + diff (plus fiable)

        auto next = snapshot::SnapshotBuilder::build(root_);
        if (next.is_err())
        {
          continue;
        }

        auto changes = snapshot::SnapshotDiff::compute(
            current_snapshot_.all(),
            next.value().all());

        if (!changes.empty() && callback_)
        {
          events::EventBatch batch;

          for (auto &c : changes)
          {
            events::FileEvent event{
                c.type,
                c.current,
                c.previous};

            batch.add(std::move(event));
          }

          callback_(batch);
        }

        current_snapshot_ = std::move(next.value());
      }
    }

  private:
    path::Path root_;
    Callback callback_;

    snapshot::Snapshot current_snapshot_;

    int fd_{-1};
    int wd_{-1};

    std::thread worker_;
    std::atomic<bool> running_{false};
  };

} // namespace softadastra::fs::watcher
