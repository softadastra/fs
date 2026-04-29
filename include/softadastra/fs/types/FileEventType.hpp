/**
 *
 *  @file FileEventType.hpp
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

#ifndef SOFTADASTRA_FS_FILE_EVENT_TYPE_HPP
#define SOFTADASTRA_FS_FILE_EVENT_TYPE_HPP

#include <cstdint>
#include <string_view>

namespace softadastra::fs::types
{
  /**
   * @brief Filesystem event types.
   *
   * Represents changes detected on the filesystem.
   *
   * Used by:
   * - watchers
   * - scanners
   * - snapshot diff
   *
   * Rules:
   * - Must remain stable (do not reorder)
   * - Used for serialization (WAL, sync)
   */
  enum class FileEventType : std::uint8_t
  {
    Created = 0, ///< File or directory created
    Updated,     ///< File modified (content or metadata)
    Deleted      ///< File or directory removed
  };

  /**
   * @brief Returns a string representation of FileEventType.
   *
   * Used for logging and debugging.
   */
  [[nodiscard]] constexpr std::string_view
  to_string(FileEventType type) noexcept
  {
    switch (type)
    {
    case FileEventType::Created:
      return "created";

    case FileEventType::Updated:
      return "updated";

    case FileEventType::Deleted:
      return "deleted";

    default:
      return "unknown";
    }
  }

} // namespace softadastra::fs::types

#endif // SOFTADASTRA_FS_FILE_EVENT_TYPE_HPP
