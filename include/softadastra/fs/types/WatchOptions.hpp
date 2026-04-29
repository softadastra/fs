/**
 *
 *  @file WatchOptions.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *  All rights reserved.
 *  https://github.com/softadastra/softadastra
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_WATCH_OPTIONS_HPP
#define SOFTADASTRA_FS_WATCH_OPTIONS_HPP

#include <cstdint>

namespace softadastra::fs::types
{

  /**
   * @brief Configuration options for filesystem watching.
   *
   * WatchOptions controls how watchers observe filesystem changes.
   *
   * It is used by:
   * - Watcher
   * - PollingWatcher
   * - platform-specific backends
   */
  struct WatchOptions
  {
    /**
     * @brief Watch directories recursively.
     */
    bool recursive{true};

    /**
     * @brief Include hidden files and directories.
     */
    bool include_hidden{false};

    /**
     * @brief Follow and track symbolic links.
     */
    bool track_symlinks{false};

    /**
     * @brief Polling interval in milliseconds.
     *
     * Used only by polling-based backends.
     */
    std::uint64_t polling_interval_ms{1000};
  };

} // namespace softadastra::fs::types

#endif // SOFTADASTRA_FS_WATCH_OPTIONS_HPP
