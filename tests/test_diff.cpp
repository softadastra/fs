/*
 * test_diff.cpp
 */

#include <cassert>
#include <iostream>

#include <softadastra/fs/snapshot/Snapshot.hpp>
#include <softadastra/fs/snapshot/SnapshotDiff.hpp>
#include <softadastra/fs/state/FileState.hpp>
#include <softadastra/fs/state/FileMetadata.hpp>
#include <softadastra/fs/types/FileType.hpp>
#include <softadastra/fs/types/FileEventType.hpp>
#include <softadastra/fs/path/Path.hpp>

using namespace softadastra::fs;

state::FileState make_file(const std::string &p, uint64_t size = 0)
{
  auto path_result = path::Path::from(p);
  assert(path_result.is_ok());

  auto path = path_result.value();

  state::FileMetadata meta;
  meta.type = types::FileType::File;
  meta.size = size;

  return state::FileState{
      path,
      meta,
      std::nullopt};
}

state::FileState make_dir(const std::string &p)
{
  auto path_result = path::Path::from(p);
  assert(path_result.is_ok());

  auto path = path_result.value();

  state::FileMetadata meta;
  meta.type = types::FileType::Directory;

  return state::FileState{
      path,
      meta,
      std::nullopt};
}

void test_file_created()
{
  snapshot::Snapshot before;
  snapshot::Snapshot after;

  after.put(make_file("/a"));

  auto diff = snapshot::SnapshotDiff::compute(before.all(), after.all());

  assert(diff.size() == 1);
  assert(diff[0].type == types::FileEventType::Created);
}

void test_file_deleted()
{
  snapshot::Snapshot before;
  snapshot::Snapshot after;

  before.put(make_file("/a"));

  auto diff = snapshot::SnapshotDiff::compute(before.all(), after.all());

  assert(diff.size() == 1);
  assert(diff[0].type == types::FileEventType::Deleted);
}

void test_file_updated()
{
  snapshot::Snapshot before;
  snapshot::Snapshot after;

  before.put(make_file("/a", 100));
  after.put(make_file("/a", 200));

  auto diff = snapshot::SnapshotDiff::compute(before.all(), after.all());

  assert(diff.size() == 1);
  assert(diff[0].type == types::FileEventType::Updated);
}

void test_no_change()
{
  snapshot::Snapshot before;
  snapshot::Snapshot after;

  before.put(make_file("/a", 100));
  after.put(make_file("/a", 100));

  auto diff = snapshot::SnapshotDiff::compute(before.all(), after.all());

  assert(diff.empty());
}

void test_multiple_changes()
{
  snapshot::Snapshot before;
  snapshot::Snapshot after;

  before.put(make_file("/a", 100));
  before.put(make_file("/b", 100));

  after.put(make_file("/a", 200)); // updated
  after.put(make_file("/c", 50));  // created

  auto diff = snapshot::SnapshotDiff::compute(before.all(), after.all());

  assert(diff.size() == 3);

  bool created = false;
  bool updated = false;
  bool deleted = false;

  for (const auto &c : diff)
  {
    if (c.type == types::FileEventType::Created)
      created = true;

    if (c.type == types::FileEventType::Updated)
      updated = true;

    if (c.type == types::FileEventType::Deleted)
      deleted = true;
  }

  assert(created);
  assert(updated);
  assert(deleted);
}

void test_directory_handling()
{
  snapshot::Snapshot before;
  snapshot::Snapshot after;

  before.put(make_dir("/dir"));
  after.put(make_dir("/dir"));

  auto diff = snapshot::SnapshotDiff::compute(before.all(), after.all());

  assert(diff.empty());
}

int main()
{
  test_file_created();
  test_file_deleted();
  test_file_updated();
  test_no_change();
  test_multiple_changes();
  test_directory_handling();

  std::cout << "All tests passed ✅\n";
  return 0;
}
