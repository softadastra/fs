/**
 *
 *  @file FileType.hpp
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

#ifndef SOFTADASTRA_FS_FILE_TYPE_HPP
#define SOFTADASTRA_FS_FILE_TYPE_HPP

#include <cstdint>
#include <string_view>

namespace softadastra::fs::types
{
  /**
   * @brief Filesystem entity types.
   *
   * Represents the type of a filesystem entry.
   *
   * Used by:
   * - Scanner
   * - Snapshot
   * - Watcher
   * - Metadata system
   *
   * Rules:
   * - Must remain stable (do not reorder)
   * - Must be serializable (WAL, sync)
   */
  enum class FileType : std::uint8_t
  {
    Unknown = 0, ///< Unknown or unsupported type
    File,        ///< Regular file
    Directory,   ///< Directory
    Symlink      ///< Symbolic link
  };

  /**
   * @brief Returns a string representation of FileType.
   *
   * Used for logging and debugging.
   */
  [[nodiscard]] constexpr std::string_view
  to_string(FileType type) noexcept
  {
    switch (type)
    {
    case FileType::Unknown:
      return "unknown";

    case FileType::File:
      return "file";

    case FileType::Directory:
      return "directory";

    case FileType::Symlink:
      return "symlink";

    default:
      return "invalid";
    }
  }

} // namespace softadastra::fs::types

#endif // SOFTADASTRA_FS_FILE_TYPE_HPP
