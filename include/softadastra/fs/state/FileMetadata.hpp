/**
 *
 *  @file FileMetadata.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_FILE_METADATA_HPP
#define SOFTADASTRA_FS_FILE_METADATA_HPP

#include <cstdint>

#include <softadastra/fs/types/FileType.hpp>
#include <softadastra/core/time/Timestamp.hpp>

namespace softadastra::fs::state
{
  namespace types = softadastra::fs::types;
  namespace time = softadastra::core::time;

  /**
   * @brief Metadata describing a filesystem entry.
   *
   * Used by:
   * - Scanner
   * - Snapshot
   * - SnapshotDiff
   *
   * Represents a lightweight, deterministic view of a file.
   */
  struct FileMetadata
  {
    /**
     * @brief File type (file, directory, symlink).
     */
    types::FileType type{types::FileType::Unknown};

    /**
     * @brief File size in bytes.
     */
    std::uint64_t size{0};

    /**
     * @brief Last modification timestamp.
     */
    time::Timestamp modified{};

    /**
     * @brief Returns true if metadata is valid.
     */
    [[nodiscard]] bool is_valid() const noexcept
    {
      return type != types::FileType::Unknown;
    }
  };

} // namespace softadastra::fs::state

#endif // SOFTADASTRA_FS_FILE_METADATA_HPP
