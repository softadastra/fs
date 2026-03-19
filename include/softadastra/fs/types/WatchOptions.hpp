/*
 * WatchOptions.hpp
 * Defines watcher configuration options.
 */

#ifndef SOFTADASTRA_FS_WATCH_OPTIONS_HPP
#define SOFTADASTRA_FS_WATCH_OPTIONS_HPP

#include <cstdint>

namespace softadastra::fs::types
{
  struct WatchOptions
  {
    bool recursive{true};
    bool include_hidden{false};
    bool track_symlinks{false};

    std::uint64_t polling_interval_ms{1000};
  };

} // namespace softadastra::fs::types

#endif
