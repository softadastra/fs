/*
 * test_watcher.cpp
 */

#include <atomic>
#include <cassert>
#include <chrono>
#include <thread>
#include <fstream>
#include <filesystem>

#include <softadastra/fs/watcher/Watcher.hpp>
#include <softadastra/fs/events/EventBatch.hpp>
#include <softadastra/fs/types/FileEventType.hpp>

using namespace softadastra::fs;

namespace fs = std::filesystem;

void sleep_short()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

void sleep_for_mtime_tick()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(1200));
}

bool wait_until_true(const std::atomic<bool> &flag,
                     std::chrono::milliseconds timeout = std::chrono::milliseconds(4000))
{
  const auto start = std::chrono::steady_clock::now();

  while (std::chrono::steady_clock::now() - start < timeout)
  {
    if (flag.load(std::memory_order_acquire))
    {
      return true;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }

  return false;
}

void test_file_created()
{
  fs::path dir = "./tmp_watch_test";
  fs::remove_all(dir);
  fs::create_directories(dir);

  watcher::Watcher watcher;
  std::atomic<bool> event_received{false};

  auto path_result = path::Path::from(dir.string());
  assert(path_result.is_ok());

  watcher.start(path_result.value(),
                [&](const events::EventBatch &batch)
                {
                  if (!batch.all().empty())
                  {
                    event_received.store(true, std::memory_order_release);
                  }
                });

  sleep_short();

  {
    std::ofstream out(dir / "file.txt", std::ios::binary);
    out << "hello";
    out.flush();
  }

  const bool ok = wait_until_true(event_received);

  watcher.stop();
  fs::remove_all(dir);

  assert(ok);
}

void test_file_updated()
{
  fs::path dir = "./tmp_watch_test2";
  fs::remove_all(dir);
  fs::create_directories(dir);

  fs::path file = dir / "file.txt";
  {
    std::ofstream out(file, std::ios::binary);
    out << "a";
    out.flush();
  }

  watcher::Watcher watcher;
  std::atomic<bool> updated{false};

  auto path_result = path::Path::from(dir.string());
  assert(path_result.is_ok());

  watcher.start(path_result.value(),
                [&](const events::EventBatch &batch)
                {
                  for (const auto &e : batch.all())
                  {
                    if (e.type == types::FileEventType::Updated)
                    {
                      updated.store(true, std::memory_order_release);
                    }
                  }
                });

  sleep_short();

  // Important: leave enough time so a polling/snapshot backend
  // can observe a distinct modification tick.
  sleep_for_mtime_tick();

  {
    std::ofstream out(file, std::ios::binary | std::ios::trunc);
    out << "b";
    out.flush();
  }

  const bool ok = wait_until_true(updated);

  watcher.stop();
  fs::remove_all(dir);

  assert(ok);
}

void test_file_deleted()
{
  fs::path dir = "./tmp_watch_test3";
  fs::remove_all(dir);
  fs::create_directories(dir);

  fs::path file = dir / "file.txt";
  {
    std::ofstream out(file, std::ios::binary);
    out << "data";
    out.flush();
  }

  watcher::Watcher watcher;
  std::atomic<bool> deleted{false};

  auto path_result = path::Path::from(dir.string());
  assert(path_result.is_ok());

  watcher.start(path_result.value(),
                [&](const events::EventBatch &batch)
                {
                  for (const auto &e : batch.all())
                  {
                    if (e.type == types::FileEventType::Deleted)
                    {
                      deleted.store(true, std::memory_order_release);
                    }
                  }
                });

  sleep_short();

  fs::remove(file);

  const bool ok = wait_until_true(deleted);

  watcher.stop();
  fs::remove_all(dir);

  assert(ok);
}

int main()
{
  test_file_created();
  test_file_updated();
  test_file_deleted();

  return 0;
}
