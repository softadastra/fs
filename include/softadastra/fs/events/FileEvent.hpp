/**
 *
 *  @file FileEvent.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_FILE_EVENT_HPP
#define SOFTADASTRA_FS_FILE_EVENT_HPP

#include <optional>

#include <softadastra/fs/state/FileState.hpp>
#include <softadastra/fs/types/FileEventType.hpp>

namespace softadastra::fs::events
{
  namespace types = softadastra::fs::types;
  namespace state = softadastra::fs::state;

  /**
   * @brief Represents a filesystem change event.
   *
   * FileEvent describes a transition between two states:
   * - current: the new state
   * - previous: the old state (optional)
   *
   * Used by:
   * - Watcher (real-time events)
   * - Scanner (initial events)
   * - SnapshotDiff (reconstructed changes)
   */
  struct FileEvent
  {
    /**
     * @brief Event type.
     */
    types::FileEventType type{types::FileEventType::Updated};

    /**
     * @brief Current state after the event.
     */
    state::FileState current{};

    /**
     * @brief Previous state before the event (if available).
     */
    std::optional<state::FileState> previous{std::nullopt};

    /**
     * @brief Returns true if this is a creation event.
     */
    [[nodiscard]] bool is_created() const noexcept
    {
      return type == types::FileEventType::Created;
    }

    /**
     * @brief Returns true if this is an update event.
     */
    [[nodiscard]] bool is_updated() const noexcept
    {
      return type == types::FileEventType::Updated;
    }

    /**
     * @brief Returns true if this is a deletion event.
     */
    [[nodiscard]] bool is_deleted() const noexcept
    {
      return type == types::FileEventType::Deleted;
    }

    /**
     * @brief Returns true if a previous state is available.
     */
    [[nodiscard]] bool has_previous() const noexcept
    {
      return previous.has_value();
    }

    /**
     * @brief Returns true if event is valid.
     */
    [[nodiscard]] bool is_valid() const noexcept
    {
      return current.is_valid();
    }
  };

} // namespace softadastra::fs::events

#endif // SOFTADASTRA_FS_FILE_EVENT_HPP
