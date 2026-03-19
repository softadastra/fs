/*
 * FileEventType.hpp
 * Defines filesystem event types.
 */

#ifndef SOFTADASTRA_FS_FILE_EVENT_TYPE_HPP
#define SOFTADASTRA_FS_FILE_EVENT_TYPE_HPP

namespace softadastra::fs::types
{
  enum class FileEventType
  {
    Created = 0,
    Updated,
    Deleted
  };

} // namespace softadastra::fs::types

#endif
