/*
 * IgnoreMatcher.hpp
 */

#ifndef SOFTADASTRA_FS_UTILS_IGNORE_MATCHER_HPP
#define SOFTADASTRA_FS_UTILS_IGNORE_MATCHER_HPP

#include <string>
#include <vector>

namespace softadastra::fs::utils
{
  class IgnoreMatcher
  {
  public:
    IgnoreMatcher() = default;

    explicit IgnoreMatcher(std::vector<std::string> patterns)
        : patterns_(std::move(patterns))
    {
    }

    void add(std::string pattern)
    {
      patterns_.emplace_back(std::move(pattern));
    }

    bool matches(const std::string &path) const
    {
      for (const auto &pattern : patterns_)
      {
        if (match_pattern(pattern, path))
        {
          return true;
        }
      }
      return false;
    }

  private:
    static bool match_pattern(const std::string &pattern,
                              const std::string &path)
    {
      return match_wildcard(pattern, path, 0, 0);
    }

    // simple wildcard matcher: '*' and '?'
    static bool match_wildcard(const std::string &pattern,
                               const std::string &str,
                               size_t p,
                               size_t s)
    {
      while (p < pattern.size() || s < str.size())
      {
        if (p < pattern.size() && pattern[p] == '*')
        {
          // collapse multiple '*'
          while (p < pattern.size() && pattern[p] == '*')
            ++p;

          if (p == pattern.size())
            return true;

          while (s < str.size())
          {
            if (match_wildcard(pattern, str, p, s))
              return true;
            ++s;
          }

          return false;
        }
        else if (p < pattern.size() &&
                 s < str.size() &&
                 (pattern[p] == '?' || pattern[p] == str[s]))
        {
          ++p;
          ++s;
        }
        else
        {
          return false;
        }
      }

      return true;
    }

  private:
    std::vector<std::string> patterns_;
  };

} // namespace softadastra::fs::utils

#endif
