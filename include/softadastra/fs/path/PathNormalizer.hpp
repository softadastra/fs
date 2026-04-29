/**
 *
 *  @file PathNormalizer.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_PATH_NORMALIZER_HPP
#define SOFTADASTRA_FS_PATH_NORMALIZER_HPP

#include <string>
#include <vector>

#include <softadastra/fs/path/PathUtils.hpp>

namespace softadastra::fs::path
{
  /**
   * @brief Canonical path normalization.
   *
   * PathNormalizer resolves:
   * - "." (current directory)
   * - ".." (parent directory)
   * - duplicate separators
   *
   * It produces a deterministic, canonical path representation.
   *
   * Examples:
   *   "a/./b"        -> "a/b"
   *   "a/b/../c"     -> "a/c"
   *   "/a/../b"      -> "/b"
   *   "../a/b"       -> "../a/b"
   */
  class PathNormalizer
  {
  public:
    /**
     * @brief Normalizes a filesystem path.
     */
    [[nodiscard]] static std::string normalize(std::string path)
    {
      // Step 1: basic normalization (slashes, duplicates)
      path = PathUtils::normalize(std::move(path));

      const bool absolute = PathUtils::is_absolute(path);

      const auto parts = PathUtils::split(path);

      std::vector<std::string> stack;
      stack.reserve(parts.size());

      for (const auto &part : parts)
      {
        if (part.empty() || part == ".")
        {
          continue;
        }

        if (part == "..")
        {
          if (!stack.empty() && stack.back() != "..")
          {
            stack.pop_back();
          }
          else if (!absolute)
          {
            stack.push_back("..");
          }
        }
        else
        {
          stack.push_back(part);
        }
      }

      std::string result;

      if (absolute)
      {
        result = "/";
      }

      for (std::size_t i = 0; i < stack.size(); ++i)
      {
        result += stack[i];

        if (i + 1 < stack.size())
        {
          result += '/';
        }
      }

      if (result.empty())
      {
        return absolute ? "/" : ".";
      }

      return result;
    }
  };

} // namespace softadastra::fs::path

#endif // SOFTADASTRA_FS_PATH_NORMALIZER_HPP
