/**
 *
 *  @file SnapshotDiff.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_SNAPSHOT_DIFF_HPP
#define SOFTADASTRA_FS_SNAPSHOT_DIFF_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include <softadastra/fs/events/FileEvent.hpp>
#include <softadastra/fs/state/FileState.hpp>
#include <softadastra/fs/types/FileEventType.hpp>

namespace softadastra::fs::snapshot
{
  namespace events = softadastra::fs::events;
  namespace state = softadastra::fs::state;
  namespace types = softadastra::fs::types;

  /**
   * @brief Computes filesystem changes between two snapshots.
   *
   * SnapshotDiff compares an old filesystem state with a new one and
   * produces a list of FileEvent objects.
   *
   * It detects:
   * - created files
   * - updated files
   * - deleted files
   */
  class SnapshotDiff
  {
  public:
    /**
     * @brief Snapshot map type.
     *
     * Key = normalized path string.
     */
    using Map = std::unordered_map<std::string, state::FileState>;

    /**
     * @brief Computes the list of changes between two snapshots.
     *
     * @param old_snapshot Previous snapshot.
     * @param new_snapshot Current snapshot.
     * @return Detected file events.
     */
    [[nodiscard]] static std::vector<events::FileEvent>
    compute(const Map &old_snapshot, const Map &new_snapshot)
    {
      std::vector<events::FileEvent> changes;

      for (const auto &[path, current] : new_snapshot)
      {
        const auto it = old_snapshot.find(path);

        if (it == old_snapshot.end())
        {
          changes.push_back(events::FileEvent{
              types::FileEventType::Created,
              current,
              std::nullopt});
        }
        else
        {
          const auto &previous = it->second;

          if (has_changed(previous, current))
          {
            changes.push_back(events::FileEvent{
                types::FileEventType::Updated,
                current,
                previous});
          }
        }
      }

      for (const auto &[path, previous] : old_snapshot)
      {
        if (new_snapshot.find(path) == new_snapshot.end())
        {
          changes.push_back(events::FileEvent{
              types::FileEventType::Deleted,
              previous,
              previous});
        }
      }

      return changes;
    }

  private:
    /**
     * @brief Returns true if two file states differ.
     */
    [[nodiscard]] static bool has_changed(
        const state::FileState &a,
        const state::FileState &b)
    {
      if (a.metadata.type != b.metadata.type)
      {
        return true;
      }

      if (a.metadata.size != b.metadata.size)
      {
        return true;
      }

      if (a.metadata.modified != b.metadata.modified)
      {
        return true;
      }

      if (a.content_hash && b.content_hash)
      {
        return *a.content_hash != *b.content_hash;
      }

      if (a.content_hash.has_value() != b.content_hash.has_value())
      {
        return true;
      }

      return false;
    }
  };

} // namespace softadastra::fs::snapshot

#endif // SOFTADASTRA_FS_SNAPSHOT_DIFF_HPP
