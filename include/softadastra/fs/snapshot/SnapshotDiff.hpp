/*
 * SnapshotDiff.hpp
 */

#ifndef SOFTADASTRA_FS_SNAPSHOT_DIFF_HPP
#define SOFTADASTRA_FS_SNAPSHOT_DIFF_HPP

#include <vector>
#include <unordered_map>
#include <optional>

#include <softadastra/fs/state/FileState.hpp>
#include <softadastra/fs/types/FileEventType.hpp>

namespace softadastra::fs::snapshot
{
  namespace state = softadastra::fs::state;
  namespace types = softadastra::fs::types;

  struct FileChange
  {
    types::FileEventType type;
    state::FileState current;
    std::optional<state::FileState> previous;
  };

  class SnapshotDiff
  {
  public:
    using Map = std::unordered_map<std::string, state::FileState>;

    static std::vector<FileChange> compute(const Map &old_snapshot,
                                           const Map &new_snapshot)
    {
      std::vector<FileChange> changes;

      // ========================
      // Created + Updated
      // ========================
      for (const auto &[path, current] : new_snapshot)
      {
        auto it = old_snapshot.find(path);

        if (it == old_snapshot.end())
        {
          changes.push_back({types::FileEventType::Created,
                             current,
                             std::nullopt});
        }
        else
        {
          const auto &previous = it->second;

          if (has_changed(previous, current))
          {
            changes.push_back({types::FileEventType::Updated,
                               current,
                               previous});
          }
        }
      }

      // ========================
      // Deleted
      // ========================
      for (const auto &[path, previous] : old_snapshot)
      {
        if (new_snapshot.find(path) == new_snapshot.end())
        {
          changes.push_back({types::FileEventType::Deleted,
                             previous,
                             previous});
        }
      }

      return changes;
    }

  private:
    static bool has_changed(const state::FileState &a,
                            const state::FileState &b)
    {
      // Type change
      if (a.metadata.type != b.metadata.type)
        return true;

      // Size change
      if (a.metadata.size != b.metadata.size)
        return true;

      // Timestamp change
      if (a.metadata.modified != b.metadata.modified)
        return true;

      // Hash change (if both exist)
      if (a.content_hash && b.content_hash)
      {
        if (*a.content_hash != *b.content_hash)
          return true;
      }

      return false;
    }
  };

} // namespace softadastra::fs::snapshot

#endif
