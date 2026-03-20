/*
 * PollingWatcher.hpp
 */

#ifndef SOFTADASTRA_FS_POLLING_WATCHER_HPP
#define SOFTADASTRA_FS_POLLING_WATCHER_HPP

#include <thread>
#include <atomic>
#include <chrono>

#include <softadastra/fs/watcher/IWatcherBackend.hpp>
#include <softadastra/fs/snapshot/Snapshot.hpp>
#include <softadastra/fs/snapshot/SnapshotBuilder.hpp>
#include <softadastra/fs/snapshot/SnapshotDiff.hpp>
#include <softadastra/fs/events/EventBatch.hpp>

namespace softadastra::fs::watcher
{
  namespace path = softadastra::fs::path;
  namespace snapshot = softadastra::fs::snapshot;
  namespace events = softadastra::fs::events;
  namespace result = softadastra::core::types;
  namespace errors = softadastra::core::errors;

  class PollingWatcher : public IWatcherBackend
  {
  public:
    using Callback = IWatcherBackend::Callback;
    using Result = IWatcherBackend::Result;

    PollingWatcher() = default;

    ~PollingWatcher()
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
    }

    bool is_running() const noexcept override
    {
      return running_;
    }

  private:
    void loop()
    {
      using namespace std::chrono_literals;

      while (running_)
      {
        std::this_thread::sleep_for(interval_);

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

    std::thread worker_;
    std::atomic<bool> running_{false};

    std::chrono::milliseconds interval_{100};
  };

} // namespace softadastra::fs::watcher

#endif
