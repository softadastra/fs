/**
 *
 *  @file EventBatch.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_EVENT_BATCH_HPP
#define SOFTADASTRA_FS_EVENT_BATCH_HPP

#include <cstddef>
#include <utility>
#include <vector>

#include <softadastra/fs/events/FileEvent.hpp>

namespace softadastra::fs::events
{
  /**
   * @brief Collection of filesystem events.
   *
   * EventBatch groups multiple FileEvent objects produced by scanners,
   * watchers, or snapshot diff operations.
   */
  class EventBatch
  {
  public:
    /**
     * @brief Internal event container type.
     */
    using Container = std::vector<FileEvent>;

    /**
     * @brief Creates an empty event batch.
     */
    EventBatch() = default;

    /**
     * @brief Adds an event by copy.
     */
    void add(const FileEvent &event)
    {
      events_.push_back(event);
    }

    /**
     * @brief Adds an event by move.
     */
    void add(FileEvent &&event)
    {
      events_.push_back(std::move(event));
    }

    /**
     * @brief Returns all events.
     */
    [[nodiscard]] const Container &all() const noexcept
    {
      return events_;
    }

    /**
     * @brief Returns the number of events.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
      return events_.size();
    }

    /**
     * @brief Returns true if the batch is empty.
     */
    [[nodiscard]] bool empty() const noexcept
    {
      return events_.empty();
    }

    /**
     * @brief Removes all events.
     */
    void clear() noexcept
    {
      events_.clear();
    }

  private:
    Container events_{};
  };

} // namespace softadastra::fs::events

#endif // SOFTADASTRA_FS_EVENT_BATCH_HPP
