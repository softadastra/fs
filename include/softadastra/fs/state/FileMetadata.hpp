/*
 * FileMetadata.hpp
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

  struct FileMetadata
  {
    types::FileType type{types::FileType::Unknown};
    std::uint64_t size{0};
    time::Timestamp modified{};
  };

} // namespace softadastra::fs::state

#endif
