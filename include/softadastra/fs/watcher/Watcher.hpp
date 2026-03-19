/*
 * Watcher.hpp
 */

#ifndef SOFTADASTRA_FS_WATCHER_HPP
#define SOFTADASTRA_FS_WATCHER_HPP

#include <memory>

#include <softadastra/fs/watcher/IWatcherBackend.hpp>
#include <softadastra/fs/watcher/PollingWatcher.hpp>

namespace softadastra::fs::watcher
{
  namespace path = softadastra::fs::path;
  namespace result = softadastra::core::types;
  namespace errors = softadastra::core::errors;

  class Watcher
  {
  public:
    using Callback = IWatcherBackend::Callback;
    using Result = IWatcherBackend::Result;

    Watcher()
        : backend_(std::make_unique<PollingWatcher>())
    {
    }

    Result start(const path::Path &root, Callback callback)
    {
      if (!backend_)
      {
        return Result::err(errors::Error(
            errors::ErrorCode::InternalError,
            errors::Severity::Error,
            "No backend available"));
      }

      return backend_->start(root, std::move(callback));
    }

    void stop()
    {
      if (backend_)
      {
        backend_->stop();
      }
    }

    bool is_running() const noexcept
    {
      return backend_ && backend_->is_running();
    }

  private:
    std::unique_ptr<IWatcherBackend> backend_;
  };

} // namespace softadastra::fs::watcher

#endif
