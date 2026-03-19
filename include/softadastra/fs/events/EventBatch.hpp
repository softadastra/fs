/*
 * EventBatch.hpp
 */

#ifndef SOFTADASTRA_FS_EVENT_BATCH_HPP
#define SOFTADASTRA_FS_EVENT_BATCH_HPP

#include <vector>

#include <softadastra/fs/events/FileEvent.hpp>

namespace softadastra::fs::events
{
  class EventBatch
  {
  public:
    using Container = std::vector<FileEvent>;

    EventBatch() = default;

    void add(const FileEvent &event)
    {
      events_.push_back(event);
    }

    void add(FileEvent &&event)
    {
      events_.push_back(std::move(event));
    }

    const Container &all() const noexcept
    {
      return events_;
    }

    std::size_t size() const noexcept
    {
      return events_.size();
    }

    bool empty() const noexcept
    {
      return events_.empty();
    }

    void clear() noexcept
    {
      events_.clear();
    }

  private:
    Container events_;
  };

} // namespace softadastra::fs::events

#endif
