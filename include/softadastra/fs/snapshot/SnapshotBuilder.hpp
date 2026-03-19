/*
 * SnapshotBuilder.hpp
 */

#ifndef SOFTADASTRA_FS_SNAPSHOT_BUILDER_HPP
#define SOFTADASTRA_FS_SNAPSHOT_BUILDER_HPP

#include <filesystem>

#include <softadastra/fs/snapshot/Snapshot.hpp>
#include <softadastra/fs/state/FileState.hpp>
#include <softadastra/fs/state/FileMetadata.hpp>
#include <softadastra/fs/path/Path.hpp>
#include <softadastra/fs/types/FileType.hpp>

#include <softadastra/core/types/Result.hpp>
#include <softadastra/core/errors/Error.hpp>
#include <softadastra/core/errors/ErrorCode.hpp>
#include <softadastra/core/errors/Severity.hpp>
#include <softadastra/core/time/Timestamp.hpp>

namespace softadastra::fs::snapshot
{
  namespace fs = std::filesystem;
  namespace state = softadastra::fs::state;
  namespace path = softadastra::fs::path;
  namespace result = softadastra::core::types;
  namespace errors = softadastra::core::errors;
  namespace fstypes = softadastra::fs::types;

  class SnapshotBuilder
  {
  public:
    using Result = result::Result<Snapshot, errors::Error>;

    static Result build(const path::Path &root)
    {
      Snapshot snapshot;

      try
      {
        fs::path root_path(root.str());

        if (!fs::exists(root_path))
        {
          return Result::err(errors::Error(
              errors::ErrorCode::NotFound,
              errors::Severity::Error,
              "Root path does not exist"));
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
        return Result::err(errors::Error(
            errors::ErrorCode::FileReadError,
            errors::Severity::Error,
            e.what()));
      }
    }

  private:
    static result::Result<state::FileState, errors::Error>
    build_file(const fs::directory_entry &entry)
    {
      try
      {
        auto path_str = entry.path().string();

        auto path_result = path::Path::from(path_str);
        if (path_result.is_err())
        {
          return result::Result<state::FileState, errors::Error>::err(
              path_result.error());
        }

        state::FileMetadata metadata;

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

        return result::Result<state::FileState, errors::Error>::ok(std::move(file));
      }
      catch (const std::exception &e)
      {
        return result::Result<state::FileState, errors::Error>::err(
            errors::Error(
                errors::ErrorCode::FileReadError,
                errors::Severity::Error,
                e.what()));
      }
    }

    static softadastra::core::time::Timestamp
    to_timestamp(const fs::file_time_type &time)
    {
      using namespace std::chrono;

      auto s = time_point_cast<seconds>(time).time_since_epoch().count();
      return softadastra::core::time::Timestamp(s);
    }
  };

} // namespace softadastra::fs::snapshot

#endif
