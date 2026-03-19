/*
 * Path.hpp
 */

#ifndef SOFTADASTRA_FS_PATH_HPP
#define SOFTADASTRA_FS_PATH_HPP

#include <string>
#include <vector>

#include <softadastra/core/types/StrongType.hpp>
#include <softadastra/core/types/Result.hpp>
#include <softadastra/core/errors/Error.hpp>
#include <softadastra/core/errors/ErrorCode.hpp>
#include <softadastra/core/errors/Severity.hpp>

#include <softadastra/fs/path/PathUtils.hpp>
#include <softadastra/fs/path/PathNormalizer.hpp>

namespace softadastra::fs::path
{
  namespace types = softadastra::core::types;
  namespace errors = softadastra::core::errors;

  struct PathTag
  {
  };

  class Path : public types::StrongType<std::string, PathTag>
  {
  public:
    using Base = types::StrongType<std::string, PathTag>;
    using Result = types::Result<Path, errors::Error>;

    using Base::Base;

    // Factory
    static Result from(std::string raw)
    {
      if (raw.empty())
      {
        return Result::err(errors::Error(
            errors::ErrorCode::InvalidArgument,
            errors::Severity::Error,
            "Path cannot be empty"));
      }

      auto normalized = PathNormalizer::normalize(std::move(raw));
      return Result::ok(Path(std::move(normalized)));
    }

    // Access
    const std::string &str() const noexcept
    {
      return this->get();
    }

    bool empty() const noexcept
    {
      return this->get().empty();
    }

    // Properties
    bool is_absolute() const noexcept
    {
      return PathUtils::is_absolute(this->get());
    }

    bool is_relative() const noexcept
    {
      return PathUtils::is_relative(this->get());
    }

    // Components
    std::vector<std::string> components() const
    {
      return PathUtils::split(this->get());
    }

    std::string filename() const
    {
      return PathUtils::filename(this->get());
    }

    // Join
    Path join(const Path &other) const
    {
      if (other.is_absolute())
      {
        return other;
      }

      auto joined = PathUtils::join(this->get(), other.get());
      return Path(PathNormalizer::normalize(std::move(joined)));
    }

    // Parent
    Path parent() const
    {
      auto p = PathUtils::parent(this->get());
      return Path(PathNormalizer::normalize(std::move(p)));
    }

    // Comparison helpers
    bool starts_with(const Path &other) const
    {
      return PathUtils::starts_with(this->get(), other.get());
    }
  };

} // namespace softadastra::fs::path

#endif
