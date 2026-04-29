/**
 *
 *  @file IWatcherBackend.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_IWATCHER_BACKEND_HPP
#define SOFTADASTRA_FS_IWATCHER_BACKEND_HPP

#include <functional>

#include <softadastra/core/Core.hpp>

#include <softadastra/fs/events/EventBatch.hpp>
#include <softadastra/fs/path/Path.hpp>

namespace softadastra::fs::watcher
{
  namespace path = softadastra::fs::path;
  namespace events = softadastra::fs::events;
  namespace core_types = softadastra::core::types;
  namespace core_errors = softadastra::core::errors;

  /**
   * @brief Interface for filesystem watcher backends.
   *
   * Implementations:
   * - PollingWatcher
   * - inotify (Linux)
   * - FSEvents (macOS)
   * - ReadDirectoryChangesW (Windows)
   */
  class IWatcherBackend : public core_types::NonCopyable
  {
  public:
    /**
     * @brief Callback invoked when events are detected.
     */
    using Callback = std::function<void(const events::EventBatch &)>;

    /**
     * @brief Result type for operations.
     */
    using Result = core_types::Result<void, core_errors::Error>;

    virtual ~IWatcherBackend() = default;

    /**
     * @brief Starts watching a root path.
     *
     * @param root Root path to monitor.
     * @param callback Event callback.
     */
    [[nodiscard]] virtual Result
    start(const path::Path &root, Callback callback) = 0;

    /**
     * @brief Stops the watcher.
     */
    virtual void stop() = 0;

    /**
     * @brief Returns true if watcher is running.
     */
    [[nodiscard]] virtual bool is_running() const noexcept = 0;
  };

} // namespace softadastra::fs::watcher

#endif // SOFTADASTRA_FS_IWATCHER_BACKEND_HPP
