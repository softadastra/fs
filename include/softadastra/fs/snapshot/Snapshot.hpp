/**
 *
 *  @file Snapshot.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_SNAPSHOT_HPP
#define SOFTADASTRA_FS_SNAPSHOT_HPP

#include <cstddef>
#include <unordered_map>

#include <softadastra/fs/path/Path.hpp>
#include <softadastra/fs/state/FileState.hpp>

namespace softadastra::fs::snapshot
{
  namespace state = softadastra::fs::state;
  namespace path = softadastra::fs::path;

  /**
   * @brief Snapshot of filesystem state.
   *
   * Snapshot stores a deterministic mapping of paths to FileState.
   *
   * Used by:
   * - Scanner
   * - SnapshotDiff
   * - Sync engine
   */
  class Snapshot
  {
  public:
    /**
     * @brief Internal storage type.
     *
     * Key = normalized path string
     */
    using Map = std::unordered_map<std::string, state::FileState>;

    Snapshot() = default;

    /**
     * @brief Inserts or updates a file state.
     */
    void put(const state::FileState &file)
    {
      files_[file.path.str()] = file;
    }

    void put(state::FileState &&file)
    {
      files_[file.path.str()] = std::move(file);
    }

    /**
     * @brief Removes a file from snapshot.
     */
    void remove(const path::Path &p)
    {
      files_.erase(p.str());
    }

    /**
     * @brief Returns file state if present.
     */
    [[nodiscard]] const state::FileState *
    get(const path::Path &p) const noexcept
    {
      const auto it = files_.find(p.str());
      if (it == files_.end())
      {
        return nullptr;
      }
      return &it->second;
    }

    /**
     * @brief Returns true if path exists.
     */
    [[nodiscard]] bool contains(const path::Path &p) const noexcept
    {
      return files_.find(p.str()) != files_.end();
    }

    /**
     * @brief Returns all entries.
     */
    [[nodiscard]] const Map &all() const noexcept
    {
      return files_;
    }

    /**
     * @brief Number of entries.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
      return files_.size();
    }

    /**
     * @brief Returns true if snapshot is empty.
     */
    [[nodiscard]] bool empty() const noexcept
    {
      return files_.empty();
    }

    /**
     * @brief Clears snapshot.
     */
    void clear() noexcept
    {
      files_.clear();
    }

  private:
    Map files_{};
  };

} // namespace softadastra::fs::snapshot

#endif // SOFTADASTRA_FS_SNAPSHOT_HPP
