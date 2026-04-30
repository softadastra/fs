/**
 *
 *  @file SnapshotBuilder.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_SNAPSHOT_BUILDER_HPP
#define SOFTADASTRA_FS_SNAPSHOT_BUILDER_HPP

#include <chrono>
#include <exception>
#include <filesystem>
#include <optional>

#include <softadastra/core/Core.hpp>

#include <softadastra/fs/path/Path.hpp>
#include <softadastra/fs/snapshot/Snapshot.hpp>
#include <softadastra/fs/state/FileMetadata.hpp>
#include <softadastra/fs/state/FileState.hpp>
#include <softadastra/fs/types/FileType.hpp>

namespace softadastra::fs::snapshot
{
  namespace fs = std::filesystem;
  namespace state = softadastra::fs::state;
  namespace path = softadastra::fs::path;
  namespace core_types = softadastra::core::types;
  namespace core_errors = softadastra::core::errors;
  namespace core_time = softadastra::core::time;
  namespace fstypes = softadastra::fs::types;

  /**
   * @brief Builds filesystem snapshots from disk.
   *
   * SnapshotBuilder scans a root directory and produces a Snapshot containing
   * FileState entries for each discovered filesystem item.
   */
  class SnapshotBuilder
  {
  public:
    using Result = core_types::Result<Snapshot, core_errors::Error>;

    /**
     * @brief Builds a snapshot from a root path.
     *
     * @param root Root directory to scan.
     * @return Snapshot on success, Error on failure.
     */
    [[nodiscard]] static Result build(const path::Path &root)
    {
      Snapshot snapshot;

      try
      {
        const fs::path root_path(root.str());

        if (!fs::exists(root_path))
        {
          return Result::err(
              core_errors::Error::make(
                  core_errors::ErrorCode::NotFound,
                  "root path does not exist",
                  core_errors::ErrorContext(root.str())));
        }

        for (const auto &entry : fs::recursive_directory_iterator(root_path))
        {
          auto file_result = build_file(entry);

          if (file_result.is_err())
          {
            return Result::err(file_result.error());
          }

          snapshot.put(std::move(file_result.value()));
        }

        return Result::ok(std::move(snapshot));
      }
      catch (const std::exception &e)
      {
        return Result::err(
            core_errors::Error::make(
                core_errors::ErrorCode::FileReadError,
                "failed to build snapshot",
                core_errors::ErrorContext(e.what())));
      }
    }

  private:
    using FileStateResult =
        core_types::Result<state::FileState, core_errors::Error>;

    /**
     * @brief Builds a FileState from a filesystem entry.
     */
    [[nodiscard]] static FileStateResult
    build_file(const fs::directory_entry &entry)
    {
      try
      {
        auto path_result = path::Path::from(entry.path().string());

        if (path_result.is_err())
        {
          return FileStateResult::err(path_result.error());
        }

        state::FileMetadata metadata{};

        if (entry.is_regular_file())
        {
          metadata.type = fstypes::FileType::File;
          metadata.size = entry.file_size();
        }
        else if (entry.is_directory())
        {
          metadata.type = fstypes::FileType::Directory;
        }
        else if (entry.is_symlink())
        {
          metadata.type = fstypes::FileType::Symlink;
        }
        else
        {
          metadata.type = fstypes::FileType::Unknown;
        }

        metadata.modified = to_timestamp(entry.last_write_time());

        state::FileState file{
            path_result.value(),
            metadata,
            std::nullopt};

        return FileStateResult::ok(std::move(file));
      }
      catch (const std::exception &e)
      {
        return FileStateResult::err(
            core_errors::Error::make(
                core_errors::ErrorCode::FileReadError,
                "failed to read filesystem entry",
                core_errors::ErrorContext(e.what())));
      }
    }

    /**
     * @brief Converts std::filesystem time to Softadastra timestamp.
     */
    [[nodiscard]] static core_time::Timestamp
    to_timestamp(const fs::file_time_type &file_time)
    {
      const auto now_file =
          fs::file_time_type::clock::now();

      const auto now_system =
          std::chrono::system_clock::now();

      const auto system_time =
          std::chrono::time_point_cast<std::chrono::system_clock::duration>(
              file_time - now_file + now_system);

      const auto millis =
          std::chrono::duration_cast<std::chrono::milliseconds>(
              system_time.time_since_epoch());

      return core_time::Timestamp::from_millis(millis.count());
    }
  };

} // namespace softadastra::fs::snapshot

#endif // SOFTADASTRA_FS_SNAPSHOT_BUILDER_HPP
