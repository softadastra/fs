/*
 * FileEvent.hpp
 */

#ifndef SOFTADASTRA_FS_FILE_EVENT_HPP
#define SOFTADASTRA_FS_FILE_EVENT_HPP

#include <optional>

#include <softadastra/fs/types/FileEventType.hpp>
#include <softadastra/fs/state/FileState.hpp>

namespace softadastra::fs::events
{
  namespace types = softadastra::fs::types;
  namespace state = softadastra::fs::state;

  struct FileEvent
  {
    types::FileEventType type;
    state::FileState current;
    std::optional<state::FileState> previous;
  };

} // namespace softadastra::fs::events

#endif
