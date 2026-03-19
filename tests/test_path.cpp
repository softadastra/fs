/*
 * test_path.cpp
 */

#include <cassert>

#include <softadastra/fs/path/Path.hpp>
#include <softadastra/fs/path/PathUtils.hpp>
#include <softadastra/fs/path/PathNormalizer.hpp>

using namespace softadastra::fs::path;

void test_path_creation()
{
  auto r = Path::from("/a/b/c");
  assert(r.is_ok());
  assert(r.value().str() == "/a/b/c");
}

void test_empty_path()
{
  auto r = Path::from("");
  assert(r.is_err());
}

void test_normalization()
{
  auto r = Path::from("/a//b///c/");
  assert(r.is_ok());
  assert(r.value().str() == "/a/b/c");
}

void test_dot_resolution()
{
  auto r = Path::from("/a/./b/./c");
  assert(r.is_ok());
  assert(r.value().str() == "/a/b/c");
}

void test_dotdot_resolution()
{
  auto r = Path::from("/a/b/../c");
  assert(r.is_ok());
  assert(r.value().str() == "/a/c");
}

void test_relative_dotdot()
{
  auto r = Path::from("a/b/../c");
  assert(r.is_ok());
  assert(r.value().str() == "a/c");
}

void test_join()
{
  auto a = Path::from("/a/b").value();
  auto b = Path::from("c/d").value();

  auto result = a.join(b);
  assert(result.str() == "/a/b/c/d");
}

void test_parent()
{
  auto p = Path::from("/a/b/c").value();
  assert(p.parent().str() == "/a/b");

  auto root = Path::from("/").value();
  assert(root.parent().str() == "/");
}

void test_filename()
{
  auto p = Path::from("/a/b/file.txt").value();
  assert(p.filename() == "file.txt");
}

void test_components()
{
  auto p = Path::from("/a/b/c").value();
  auto parts = p.components();

  assert(parts.size() == 3);
  assert(parts[0] == "a");
  assert(parts[1] == "b");
  assert(parts[2] == "c");
}

void test_starts_with()
{
  auto a = Path::from("/a/b/c").value();
  auto b = Path::from("/a/b").value();

  assert(a.starts_with(b));
}

int main()
{
  test_path_creation();
  test_empty_path();
  test_normalization();
  test_dot_resolution();
  test_dotdot_resolution();
  test_relative_dotdot();
  test_join();
  test_parent();
  test_filename();
  test_components();
  test_starts_with();

  return 0;
}
