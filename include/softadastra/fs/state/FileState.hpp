/*
 * FileState.hpp
 */

#ifndef SOFTADASTRA_FS_FILE_STATE_HPP
#define SOFTADASTRA_FS_FILE_STATE_HPP

#include <optional>

#include <softadastra/fs/path/Path.hpp>
#include <softadastra/fs/state/FileMetadata.hpp>
#include <softadastra/fs/types/FileType.hpp>
#include <softadastra/core/hash/Hash.hpp>

namespace softadastra::fs::state
{
  namespace path = softadastra::fs::path;
  namespace types = softadastra::fs::types;
  namespace hash = softadastra::core::hash;

  struct FileState
  {
    path::Path path;
    FileMetadata metadata;
    std::optional<hash::Hash> content_hash;

    bool is_directory() const noexcept
    {
      return metadata.type == types::FileType::Directory;
    }

    bool is_file() const noexcept
    {
      return metadata.type == types::FileType::File;
    }
  };

} // namespace softadastra::fs::state

#endif
