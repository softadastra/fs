/**
 *
 *  @file PathUtils.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_PATH_UTILS_HPP
#define SOFTADASTRA_FS_PATH_UTILS_HPP

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

namespace softadastra::fs::path
{
  /**
   * @brief Stateless utilities for path manipulation.
   *
   * PathUtils provides lightweight, deterministic path helpers used by
   * scanners, watchers, snapshots, and filesystem state tracking.
   */
  class PathUtils
  {
  public:
    /**
     * @brief Normalizes path separators and removes duplicate slashes.
     *
     * Converts '\\' to '/' and removes trailing slash except for root.
     */
    [[nodiscard]] static std::string normalize(std::string path)
    {
      std::replace(path.begin(), path.end(), '\\', '/');

      std::string result;
      result.reserve(path.size());

      bool prev_slash = false;

      for (char c : path)
      {
        if (c == '/')
        {
          if (!prev_slash)
          {
            result.push_back(c);
            prev_slash = true;
          }
        }
        else
        {
          result.push_back(c);
          prev_slash = false;
        }
      }

      if (result.size() > 1 && result.back() == '/')
      {
        result.pop_back();
      }

      return result;
    }

    /**
     * @brief Splits a path into components.
     */
    [[nodiscard]] static std::vector<std::string> split(std::string_view path)
    {
      std::vector<std::string> parts;
      std::string current;

      for (char c : path)
      {
        if (c == '/')
        {
          if (!current.empty())
          {
            parts.push_back(std::move(current));
            current.clear();
          }
        }
        else
        {
          current.push_back(c);
        }
      }

      if (!current.empty())
      {
        parts.push_back(std::move(current));
      }

      return parts;
    }

    /**
     * @brief Joins two path segments and normalizes the result.
     */
    [[nodiscard]] static std::string join(
        std::string_view a,
        std::string_view b)
    {
      if (a.empty())
      {
        return normalize(std::string(b));
      }

      if (b.empty())
      {
        return normalize(std::string(a));
      }

      if (is_absolute(b))
      {
        return normalize(std::string(b));
      }

      std::string result(a);

      if (result.back() != '/')
      {
        result += '/';
      }

      result += b;

      return normalize(std::move(result));
    }

    /**
     * @brief Returns the filename component of a path.
     */
    [[nodiscard]] static std::string filename(std::string_view path)
    {
      auto normalized = normalize(std::string(path));
      auto parts = split(normalized);

      if (parts.empty())
      {
        return {};
      }

      return parts.back();
    }

    /**
     * @brief Returns the parent directory of a path.
     */
    [[nodiscard]] static std::string parent(std::string_view path)
    {
      auto normalized = normalize(std::string(path));

      if (normalized.empty() || normalized == "/")
      {
        return normalized;
      }

      const auto pos = normalized.find_last_of('/');

      if (pos == std::string::npos)
      {
        return {};
      }

      if (pos == 0)
      {
        return "/";
      }

      return normalized.substr(0, pos);
    }

    /**
     * @brief Returns true if path is absolute.
     */
    [[nodiscard]] static bool is_absolute(std::string_view path) noexcept
    {
#ifdef _WIN32
      return path.size() > 2 && path[1] == ':';
#else
      return !path.empty() && path[0] == '/';
#endif
    }

    /**
     * @brief Returns true if path is relative.
     */
    [[nodiscard]] static bool is_relative(std::string_view path) noexcept
    {
      return !is_absolute(path);
    }

    /**
     * @brief Returns true if path starts with prefix.
     */
    [[nodiscard]] static bool starts_with(
        std::string_view path,
        std::string_view prefix) noexcept
    {
      if (prefix.size() > path.size())
      {
        return false;
      }

      return std::equal(prefix.begin(), prefix.end(), path.begin());
    }

    /**
     * @brief Returns true if filename starts with '.'.
     */
    [[nodiscard]] static bool is_hidden(std::string_view path)
    {
      const auto name = filename(path);
      return !name.empty() && name[0] == '.';
    }
  };

} // namespace softadastra::fs::path

#endif // SOFTADASTRA_FS_PATH_UTILS_HPP
