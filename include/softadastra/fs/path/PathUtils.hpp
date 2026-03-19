/*
 * PathUtils.hpp
 * Stateless utilities for path manipulation and validation.
 */

#ifndef SOFTADASTRA_FS_PATH_UTILS_HPP
#define SOFTADASTRA_FS_PATH_UTILS_HPP

#include <string>
#include <vector>
#include <algorithm>

namespace softadastra::fs::path
{
  class PathUtils
  {
  public:
    static std::string normalize(std::string path)
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

      // Remove trailing slash (except root)
      if (result.size() > 1 && result.back() == '/')
      {
        result.pop_back();
      }

      return result;
    }

    // Split path
    static std::vector<std::string> split(const std::string &path)
    {
      std::vector<std::string> parts;
      std::string current;

      for (char c : path)
      {
        if (c == '/')
        {
          if (!current.empty())
          {
            parts.push_back(current);
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
        parts.push_back(current);
      }

      return parts;
    }

    static std::string join(const std::string &a, const std::string &b)
    {
      if (a.empty())
        return normalize(b);

      if (b.empty())
        return normalize(a);

      if (is_absolute(b))
        return normalize(b);

      std::string result = a;

      if (result.back() != '/')
      {
        result += '/';
      }

      result += b;

      return normalize(std::move(result));
    }

    static std::string filename(const std::string &path)
    {
      auto parts = split(path);
      if (parts.empty())
        return {};
      return parts.back();
    }

    static std::string parent(const std::string &path)
    {
      if (path.empty() || path == "/")
        return path;

      auto pos = path.find_last_of('/');

      if (pos == std::string::npos)
        return {};

      if (pos == 0)
        return "/";

      return path.substr(0, pos);
    }

    static bool is_absolute(const std::string &path)
    {
#ifdef _WIN32
      return path.size() > 2 && path[1] == ':';
#else
      return !path.empty() && path[0] == '/';
#endif
    }

    static bool is_relative(const std::string &path)
    {
      return !is_absolute(path);
    }

    static bool starts_with(const std::string &path, const std::string &prefix)
    {
      if (prefix.size() > path.size())
        return false;

      return std::equal(prefix.begin(), prefix.end(), path.begin());
    }

    static bool is_hidden(const std::string &path)
    {
      auto name = filename(path);
      return !name.empty() && name[0] == '.';
    }
  };

} // namespace softadastra::fs::path

#endif
