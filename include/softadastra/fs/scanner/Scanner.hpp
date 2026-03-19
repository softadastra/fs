/*
 * Scanner.hpp
 */

#ifndef SOFTADASTRA_FS_SCANNER_HPP
#define SOFTADASTRA_FS_SCANNER_HPP

#include <softadastra/fs/path/Path.hpp>
#include <softadastra/fs/snapshot/Snapshot.hpp>
#include <softadastra/fs/snapshot/SnapshotBuilder.hpp>

#include <softadastra/core/types/Result.hpp>
#include <softadastra/core/errors/Error.hpp>

namespace softadastra::fs::scanner
{
  namespace path = softadastra::fs::path;
  namespace snapshot = softadastra::fs::snapshot;
  namespace result = softadastra::core::types;
  namespace errors = softadastra::core::errors;

  class Scanner
  {
  public:
    using Result = result::Result<snapshot::Snapshot, errors::Error>;

    static Result scan(const path::Path &root)
    {
      return snapshot::SnapshotBuilder::build(root);
    }
  };

} // namespace softadastra::fs::scanner

#endif
