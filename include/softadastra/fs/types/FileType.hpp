/*
 * FileType.hpp
 * Defines filesystem entity types.
 */

#ifndef SOFTADASTRA_FS_FILE_TYPE_HPP
#define SOFTADASTRA_FS_FILE_TYPE_HPP

namespace softadastra::fs::types
{
  enum class FileType
  {
    Unknown = 0,
    File,
    Directory,
    Symlink
  };

} // namespace softadastra::fs::types

#endif
