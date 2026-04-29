/**
 *
 *  @file Watcher.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_WATCHER_HPP
#define SOFTADASTRA_FS_WATCHER_HPP

#include <memory>
#include <utility>

#include <softadastra/core/Core.hpp>

#include <softadastra/fs/watcher/IWatcherBackend.hpp>
#include <softadastra/fs/watcher/PollingWatcher.hpp>

namespace softadastra::fs::watcher
{

  namespace path = softadastra::fs::path;
  namespace core_errors = softadastra::core::errors;

  /**
   * @brief Public filesystem watcher facade.
   *
   * Watcher provides a stable user-facing API and delegates actual watching
   * to an internal backend.
   *
   * The default backend is PollingWatcher.
   */
  class Watcher
  {
  public:
    using Callback = IWatcherBackend::Callback;
    using Result = IWatcherBackend::Result;

    /**
     * @brief Creates a watcher using the default backend.
     */
    Watcher()
        : backend_(std::make_unique<PollingWatcher>())
    {
    }

    /**
     * @brief Creates a watcher with a custom backend.
     */
    explicit Watcher(std::unique_ptr<IWatcherBackend> backend) noexcept
        : backend_(std::move(backend))
    {
    }

    /**
     * @brief Stops the watcher on destruction.
     */
    ~Watcher()
    {
      stop();
    }

    Watcher(const Watcher &) = delete;
    Watcher &operator=(const Watcher &) = delete;

    Watcher(Watcher &&) noexcept = default;
    Watcher &operator=(Watcher &&) noexcept = default;

    /**
     * @brief Starts watching a root path.
     */
    [[nodiscard]] Result start(const path::Path &root, Callback callback)
    {
      if (!backend_)
      {
        return Result::err(
            core_errors::Error::make(
                core_errors::ErrorCode::InternalError,
                "no watcher backend available"));
      }

      return backend_->start(root, std::move(callback));
    }

    /**
     * @brief Stops watching.
     */
    void stop()
    {
      if (backend_)
      {
        backend_->stop();
      }
    }

    /**
     * @brief Returns true if watcher is running.
     */
    [[nodiscard]] bool is_running() const noexcept
    {
      return backend_ && backend_->is_running();
    }

  private:
    std::unique_ptr<IWatcherBackend> backend_{};
  };

} // namespace softadastra::fs::watcher

#endif // SOFTADASTRA_FS_WATCHER_HPP
