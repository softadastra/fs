/**
 *
 *  @file Scanner.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_SCANNER_HPP
#define SOFTADASTRA_FS_SCANNER_HPP

#include <softadastra/core/Core.hpp>

#include <softadastra/fs/path/Path.hpp>
#include <softadastra/fs/snapshot/Snapshot.hpp>
#include <softadastra/fs/snapshot/SnapshotBuilder.hpp>

namespace softadastra::fs::scanner
{
  namespace path = softadastra::fs::path;
  namespace snapshot = softadastra::fs::snapshot;
  namespace core_types = softadastra::core::types;
  namespace core_errors = softadastra::core::errors;

  /**
   * @brief High-level filesystem scanner.
   *
   * Scanner provides a simple public API for building a filesystem snapshot
   * from a root path.
   *
   * Internally, it delegates the actual filesystem traversal to SnapshotBuilder.
   */
  class Scanner
  {
  public:
    /**
     * @brief Result type returned by scan operations.
     */
    using Result = core_types::Result<snapshot::Snapshot, core_errors::Error>;

    /**
     * @brief Scans a root path and returns a filesystem snapshot.
     *
     * @param root Normalized root path.
     * @return Snapshot on success, Error on failure.
     */
    [[nodiscard]] static Result scan(const path::Path &root)
    {
      return snapshot::SnapshotBuilder::build(root);
    }
  };

} // namespace softadastra::fs::scanner

#endif // SOFTADASTRA_FS_SCANNER_HPP
