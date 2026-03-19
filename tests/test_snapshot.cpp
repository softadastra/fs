/*
 * test_snapshot.cpp
 */

#include <cassert>

#include <softadastra/fs/snapshot/Snapshot.hpp>
#include <softadastra/fs/state/FileState.hpp>
#include <softadastra/fs/state/FileMetadata.hpp>
#include <softadastra/fs/types/FileType.hpp>
#include <softadastra/fs/path/Path.hpp>

using namespace softadastra::fs;

state::FileState make_file(const std::string &p, uint64_t size = 0)
{
  auto path = path::Path::from(p).value();

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
  auto path = path::Path::from(p).value();

  state::FileMetadata meta;
  meta.type = types::FileType::Directory;

  return state::FileState{
      path,
      meta,
      std::nullopt};
}

void test_put_and_get()
{
  snapshot::Snapshot snap;

  auto f = make_file("/a/file.txt", 100);

  snap.put(f);

  auto *res = snap.get(f.path);
  assert(res != nullptr);
  assert(res->metadata.size == 100);
}

void test_overwrite()
{
  snapshot::Snapshot snap;

  auto f1 = make_file("/a/file.txt", 100);
  auto f2 = make_file("/a/file.txt", 200);

  snap.put(f1);
  snap.put(f2);

  auto *res = snap.get(f1.path);
  assert(res != nullptr);
  assert(res->metadata.size == 200);
}

void test_size()
{
  snapshot::Snapshot snap;

  snap.put(make_file("/a"));
  snap.put(make_file("/b"));
  snap.put(make_dir("/c"));

  assert(snap.size() == 3);
}

void test_contains()
{
  snapshot::Snapshot snap;

  auto f = make_file("/a/file");

  snap.put(f);

  assert(snap.contains(f.path));

  auto other = path::Path::from("/not_exists").value();
  assert(!snap.contains(other));
}

void test_all()
{
  snapshot::Snapshot snap;

  snap.put(make_file("/a"));
  snap.put(make_file("/b"));

  const auto &all = snap.all();

  assert(all.size() == 2);
}

void test_remove()
{
  snapshot::Snapshot snap;

  auto f = make_file("/a");

  snap.put(f);
  assert(snap.contains(f.path));

  snap.remove(f.path);
  assert(!snap.contains(f.path));
}

int main()
{
  test_put_and_get();
  test_overwrite();
  test_size();
  test_contains();
  test_all();
  test_remove();

  return 0;
}
