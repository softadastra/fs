/*
 * test_state.cpp
 */

#include <cassert>
#include <optional>

#include <softadastra/fs/state/FileState.hpp>
#include <softadastra/fs/state/FileMetadata.hpp>
#include <softadastra/fs/types/FileType.hpp>
#include <softadastra/fs/path/Path.hpp>

using namespace softadastra::fs;

void test_file_state_file()
{
  auto p = path::Path::from("/tmp/file.txt").value();

  state::FileMetadata meta;
  meta.type = types::FileType::File;
  meta.size = 123;

  state::FileState fs{
      p,
      meta,
      std::nullopt};

  assert(fs.is_file());
  assert(!fs.is_directory());
}

void test_file_state_directory()
{
  auto p = path::Path::from("/tmp/folder").value();

  state::FileMetadata meta;
  meta.type = types::FileType::Directory;

  state::FileState fs{
      p,
      meta,
      std::nullopt};

  assert(fs.is_directory());
  assert(!fs.is_file());
}

void test_metadata_values()
{
  auto p = path::Path::from("/a/b").value();

  state::FileMetadata meta;
  meta.type = types::FileType::File;
  meta.size = 42;

  state::FileState fs{
      p,
      meta,
      std::nullopt};

  assert(fs.metadata.size == 42);
  assert(fs.metadata.type == types::FileType::File);
}

void test_state_equality_semantics()
{
  auto p1 = path::Path::from("/a/file").value();
  auto p2 = path::Path::from("/a/file").value();

  state::FileMetadata m1;
  m1.type = types::FileType::File;
  m1.size = 10;

  state::FileMetadata m2;
  m2.type = types::FileType::File;
  m2.size = 10;

  state::FileState f1{p1, m1, std::nullopt};
  state::FileState f2{p2, m2, std::nullopt};

  // même contenu logique
  assert(f1.path.str() == f2.path.str());
  assert(f1.metadata.size == f2.metadata.size);
}

void test_optional_hash()
{
  auto p = path::Path::from("/file").value();

  state::FileMetadata meta;
  meta.type = types::FileType::File;

  state::FileState fs{
      p,
      meta,
      std::nullopt};

  assert(!fs.content_hash.has_value());
}

int main()
{
  test_file_state_file();
  test_file_state_directory();
  test_metadata_values();
  test_state_equality_semantics();
  test_optional_hash();

  return 0;
}
