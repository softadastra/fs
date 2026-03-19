/*
 * IWatcherBackend.hpp
 */

#ifndef SOFTADASTRA_FS_IWATCHER_BACKEND_HPP
#define SOFTADASTRA_FS_IWATCHER_BACKEND_HPP

#include <functional>

#include <softadastra/fs/path/Path.hpp>
#include <softadastra/fs/events/EventBatch.hpp>

#include <softadastra/core/types/NonCopyable.hpp>
#include <softadastra/core/types/Result.hpp>
#include <softadastra/core/errors/Error.hpp>

namespace softadastra::fs::watcher
{
  namespace path = softadastra::fs::path;
  namespace events = softadastra::fs::events;
  namespace result = softadastra::core::types;
  namespace errors = softadastra::core::errors;

  class IWatcherBackend : public result::NonCopyable
  {
  public:
    using Callback = std::function<void(const events::EventBatch &)>;
    using Result = result::Result<void, errors::Error>;

    virtual ~IWatcherBackend() = default;

    virtual Result start(const path::Path &root, Callback callback) = 0;

    virtual void stop() = 0;

    virtual bool is_running() const noexcept = 0;
  };

} // namespace softadastra::fs::watcher

#endif
