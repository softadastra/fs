/**
 *
 *  @file Path.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_PATH_HPP
#define SOFTADASTRA_FS_PATH_HPP

#include <string>
#include <vector>

#include <softadastra/core/Core.hpp>

#include <softadastra/fs/path/PathNormalizer.hpp>
#include <softadastra/fs/path/PathUtils.hpp>

namespace softadastra::fs::path
{
  namespace core_types = softadastra::core::types;
  namespace core_errors = softadastra::core::errors;

  /**
   * @brief Tag type for Path strong type.
   */
  struct PathTag
  {
  };

  /**
   * @brief Strong, normalized filesystem path.
   *
   * Path wraps a normalized string path and prevents accidental use of raw
   * strings in the FS module.
   */
  class Path : public core_types::StrongType<std::string, PathTag>
  {
  public:
    using Base = core_types::StrongType<std::string, PathTag>;
    using Result = core_types::Result<Path, core_errors::Error>;

    using Base::Base;

    /**
     * @brief Creates a validated and normalized path.
     */
    [[nodiscard]] static Result from(std::string raw)
    {
      if (raw.empty())
      {
        return Result::err(
            core_errors::Error::make(
                core_errors::ErrorCode::InvalidArgument,
                "path cannot be empty"));
      }

      auto normalized = PathNormalizer::normalize(std::move(raw));
      return Result::ok(Path(std::move(normalized)));
    }

    /**
     * @brief Returns the normalized path string.
     */
    [[nodiscard]] const std::string &str() const noexcept
    {
      return this->get();
    }

    /**
     * @brief Returns true if path is empty.
     */
    [[nodiscard]] bool empty() const noexcept
    {
      return this->get().empty();
    }

    /**
     * @brief Returns true if path is absolute.
     */
    [[nodiscard]] bool is_absolute() const noexcept
    {
      return PathUtils::is_absolute(this->get());
    }

    /**
     * @brief Returns true if path is relative.
     */
    [[nodiscard]] bool is_relative() const noexcept
    {
      return PathUtils::is_relative(this->get());
    }

    /**
     * @brief Returns path components.
     */
    [[nodiscard]] std::vector<std::string> components() const
    {
      return PathUtils::split(this->get());
    }

    /**
     * @brief Returns filename component.
     */
    [[nodiscard]] std::string filename() const
    {
      return PathUtils::filename(this->get());
    }

    /**
     * @brief Joins this path with another path.
     *
     * If other is absolute, other is returned.
     */
    [[nodiscard]] Path join(const Path &other) const
    {
      if (other.is_absolute())
      {
        return other;
      }

      auto joined = PathUtils::join(this->get(), other.get());
      return Path(PathNormalizer::normalize(std::move(joined)));
    }

    /**
     * @brief Returns parent path.
     */
    [[nodiscard]] Path parent() const
    {
      auto p = PathUtils::parent(this->get());
      return Path(PathNormalizer::normalize(std::move(p)));
    }

    /**
     * @brief Returns true if this path starts with another path.
     */
    [[nodiscard]] bool starts_with(const Path &other) const
    {
      return PathUtils::starts_with(this->get(), other.get());
    }
  };

} // namespace softadastra::fs::path

#endif // SOFTADASTRA_FS_PATH_HPP
