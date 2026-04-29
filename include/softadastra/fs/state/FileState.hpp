/**
 *
 *  @file FileState.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_FILE_STATE_HPP
#define SOFTADASTRA_FS_FILE_STATE_HPP

#include <optional>

#include <softadastra/core/hash/Hash.hpp>
#include <softadastra/fs/path/Path.hpp>
#include <softadastra/fs/state/FileMetadata.hpp>
#include <softadastra/fs/types/FileType.hpp>

namespace softadastra::fs::state
{
  namespace path = softadastra::fs::path;
  namespace types = softadastra::fs::types;
  namespace hash = softadastra::core::hash;

  /**
   * @brief Complete state of a filesystem entry.
   *
   * FileState combines:
   * - normalized path
   * - filesystem metadata
   * - optional content hash
   *
   * Used by Scanner, Snapshot, and SnapshotDiff.
   */
  struct FileState
  {
    /**
     * @brief Normalized filesystem path.
     */
    path::Path path;

    /**
     * @brief Metadata associated with the path.
     */
    FileMetadata metadata{};

    /**
     * @brief Optional content hash.
     *
     * Usually present for regular files only.
     */
    std::optional<hash::Hash> content_hash{std::nullopt};

    /**
     * @brief Returns true if entry is a directory.
     */
    [[nodiscard]] bool is_directory() const noexcept
    {
      return metadata.type == types::FileType::Directory;
    }

    /**
     * @brief Returns true if entry is a regular file.
     */
    [[nodiscard]] bool is_file() const noexcept
    {
      return metadata.type == types::FileType::File;
    }

    /**
     * @brief Returns true if entry has a valid path and metadata.
     */
    [[nodiscard]] bool is_valid() const noexcept
    {
      return !path.empty() && metadata.is_valid();
    }

    /**
     * @brief Returns true if a content hash is available.
     */
    [[nodiscard]] bool has_hash() const noexcept
    {
      return content_hash.has_value();
    }
  };

} // namespace softadastra::fs::state

#endif // SOFTADASTRA_FS_FILE_STATE_HPP
