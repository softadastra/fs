/**
 *
 *  @file IgnoreMatcher.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_UTILS_IGNORE_MATCHER_HPP
#define SOFTADASTRA_FS_UTILS_IGNORE_MATCHER_HPP

#include <string>
#include <string_view>
#include <vector>

namespace softadastra::fs::utils
{

  /**
   * @brief Matches file paths against ignore patterns.
   *
   * Supports simple wildcard patterns:
   * - '*' matches any sequence
   * - '?' matches a single character
   *
   * Example:
   *   "*.log"
   *   "build/*"
   *   "temp?.txt"
   */
  class IgnoreMatcher
  {
  public:
    IgnoreMatcher() = default;

    explicit IgnoreMatcher(std::vector<std::string> patterns)
        : patterns_(std::move(patterns))
    {
    }

    /**
     * @brief Adds a new ignore pattern.
     */
    void add(std::string pattern)
    {
      patterns_.emplace_back(std::move(pattern));
    }

    /**
     * @brief Returns true if path matches any ignore pattern.
     */
    [[nodiscard]] bool matches(std::string_view path) const
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
    static bool match_pattern(std::string_view pattern,
                              std::string_view path)
    {
      return match_wildcard(pattern, path, 0, 0);
    }

    /**
     * @brief Simple wildcard matcher.
     *
     * '*' → any sequence
     * '?' → single character
     */
    static bool match_wildcard(std::string_view pattern,
                               std::string_view str,
                               std::size_t p,
                               std::size_t s)
    {
      while (p < pattern.size() || s < str.size())
      {
        if (p < pattern.size() && pattern[p] == '*')
        {
          // collapse consecutive '*'
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
