/*
 * Snapshot.hpp
 */

#ifndef SOFTADASTRA_FS_SNAPSHOT_HPP
#define SOFTADASTRA_FS_SNAPSHOT_HPP

#include <unordered_map>
#include <string>

#include <softadastra/fs/state/FileState.hpp>
#include <softadastra/fs/path/Path.hpp>

namespace softadastra::fs::snapshot
{
  namespace state = softadastra::fs::state;
  namespace path = softadastra::fs::path;

  class Snapshot
  {
  public:
    using Map = std::unordered_map<std::string, state::FileState>;

    Snapshot() = default;

    // Insert / Update
    void put(const state::FileState &file)
    {
      files_[file.path.str()] = file;
    }

    void put(state::FileState &&file)
    {
      files_[file.path.str()] = std::move(file);
    }

    // Remove
    void remove(const path::Path &p)
    {
      files_.erase(p.str());
    }

    // Access
    const state::FileState *get(const path::Path &p) const noexcept
    {
      auto it = files_.find(p.str());
      if (it == files_.end())
        return nullptr;
      return &it->second;
    }

    bool contains(const path::Path &p) const noexcept
    {
      return files_.find(p.str()) != files_.end();
    }

    const Map &all() const noexcept
    {
      return files_;
    }

    std::size_t size() const noexcept
    {
      return files_.size();
    }

    bool empty() const noexcept
    {
      return files_.empty();
    }

  private:
    Map files_;
  };

} // namespace softadastra::fs::snapshot

#endif
