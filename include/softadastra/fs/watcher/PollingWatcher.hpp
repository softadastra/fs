/**
 *
 *  @file PollingWatcher.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_POLLING_WATCHER_HPP
#define SOFTADASTRA_FS_POLLING_WATCHER_HPP

#include <atomic>
#include <chrono>
#include <thread>
#include <utility>

#include <softadastra/core/Core.hpp>

#include <softadastra/fs/events/EventBatch.hpp>
#include <softadastra/fs/snapshot/Snapshot.hpp>
#include <softadastra/fs/snapshot/SnapshotBuilder.hpp>
#include <softadastra/fs/snapshot/SnapshotDiff.hpp>
#include <softadastra/fs/watcher/IWatcherBackend.hpp>

namespace softadastra::fs::watcher
{
  namespace path = softadastra::fs::path;
  namespace snapshot = softadastra::fs::snapshot;
  namespace events = softadastra::fs::events;
  namespace core_errors = softadastra::core::errors;

  /**
   * @brief Polling-based filesystem watcher.
   *
   * PollingWatcher periodically rebuilds a snapshot and computes the diff
   * against the previous snapshot.
   *
   * It is portable and deterministic, but less efficient than native OS
   * backends such as inotify, FSEvents, or ReadDirectoryChangesW.
   */
  class PollingWatcher : public IWatcherBackend
  {
  public:
    using Callback = IWatcherBackend::Callback;
    using Result = IWatcherBackend::Result;

    PollingWatcher() = default;

    /**
     * @brief Stops the worker thread on destruction.
     */
    ~PollingWatcher() override
    {
      stop();
    }

    /**
     * @brief Starts polling a root path.
     */
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

      auto initial = snapshot::SnapshotBuilder::build(root_);

      if (initial.is_err())
      {
        return Result::err(initial.error());
      }

      current_snapshot_ = std::move(initial.value());
      running_ = true;

      worker_ = std::thread([this]
                            { loop(); });

      return Result::ok();
    }

    /**
     * @brief Stops polling.
     */
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
    }

    /**
     * @brief Returns true if the watcher is running.
     */
    [[nodiscard]] bool is_running() const noexcept override
    {
      return running_;
    }

  private:
    /**
     * @brief Main polling loop.
     */
    void loop()
    {
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

          for (auto &event : changes)
          {
            batch.add(std::move(event));
          }

          callback_(batch);
        }

        current_snapshot_ = std::move(next.value());
      }
    }

  private:
    path::Path root_{};
    Callback callback_{};

    snapshot::Snapshot current_snapshot_{};

    std::thread worker_{};
    std::atomic<bool> running_{false};

    std::chrono::milliseconds interval_{100};
  };

} // namespace softadastra::fs::watcher

#endif // SOFTADASTRA_FS_POLLING_WATCHER_HPP
