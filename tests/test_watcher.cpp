/*
 * test_watcher.cpp
 */

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

void test_file_created()
{
  fs::path dir = "./tmp_watch_test";
  fs::create_directories(dir);

  watcher::Watcher watcher;

  bool event_received = false;

  watcher.start(path::Path::from(dir.string()).value(),
                [&](const events::EventBatch &batch)
                {
                  if (!batch.all().empty())
                  {
                    event_received = true;
                  }
                });

  sleep_short();

  std::ofstream(dir / "file.txt") << "hello";

  sleep_short();

  watcher.stop();

  fs::remove_all(dir);

  assert(event_received);
}

void test_file_updated()
{
  fs::path dir = "./tmp_watch_test2";
  fs::create_directories(dir);

  fs::path file = dir / "file.txt";
  std::ofstream(file) << "a";

  watcher::Watcher watcher;

  bool updated = false;

  watcher.start(path::Path::from(dir.string()).value(),
                [&](const events::EventBatch &batch)
                {
                  for (const auto &e : batch.all())
                  {
                    if (e.type == types::FileEventType::Updated)
                    {
                      updated = true;
                    }
                  }
                });

  sleep_short();

  std::ofstream(file) << "b";

  sleep_short();

  watcher.stop();
  fs::remove_all(dir);

  assert(updated);
}

void test_file_deleted()
{
  fs::path dir = "./tmp_watch_test3";
  fs::create_directories(dir);

  fs::path file = dir / "file.txt";
  std::ofstream(file) << "data";

  watcher::Watcher watcher;

  bool deleted = false;

  watcher.start(path::Path::from(dir.string()).value(),
                [&](const events::EventBatch &batch)
                {
                  for (const auto &e : batch.all())
                  {
                    if (e.type == types::FileEventType::Deleted)
                    {
                      deleted = true;
                    }
                  }
                });

  sleep_short();

  fs::remove(file);

  sleep_short();

  watcher.stop();
  fs::remove_all(dir);

  assert(deleted);
}

int main()
{
  test_file_created();
  test_file_updated();
  test_file_deleted();

  return 0;
}
