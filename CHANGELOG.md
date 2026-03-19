# Changelog - softadastra/fs

All notable changes to the **fs module** are documented in this file.

The `fs` module is responsible for **observing and describing the local filesystem state**, providing reliable events and snapshots to the rest of the system.

---

## [0.1.0] - Initial Filesystem Layer

### Added

* Directory watcher:

  * File creation detection
  * File modification detection
  * File deletion detection
* Initial directory scanner:

  * Full scan of a directory tree
  * Bootstrapping of filesystem state
* File state representation (`FileState`):

  * Path
  * Size
  * Last modified timestamp
  * Optional hash (basic support)
* Snapshot system:

  * Full directory snapshot
  * In-memory representation
* Path utilities:

  * Normalization
  * Relative/absolute resolution
  * Basic cross-platform handling

### Design

* Pure observation layer
* No dependency on sync or network modules
* Emits events and state only

---

## [0.1.1] - Stability Improvements

### Improved

* Reduced duplicate events during rapid file changes
* Better handling of temporary files (editor artifacts)
* More consistent path normalization

### Fixed

* Missed file modification events in edge cases
* Incorrect behavior when files are rapidly created and deleted
* Timestamp inconsistencies across platforms

---

## [0.2.0] - Snapshot & Diff Improvements

### Added

* Snapshot diffing:

  * Compare two snapshots
  * Detect created, updated, and deleted files
* File comparison helpers
* Optional hash-based change detection

### Improved

* Accuracy of change detection
* Performance of initial scan on medium-sized directories
* Memory usage for snapshot structures

---

## [0.3.0] - Watcher Reliability

### Added

* Fallback polling mode when native watchers are unavailable
* Event coalescing (batching rapid changes)
* Basic debounce mechanism

### Improved

* Stability under high-frequency updates
* Reduced noise in emitted events

### Fixed

* Missed events under heavy filesystem activity
* Race conditions between initial scan and watcher start

---

## [0.4.0] - Cross-Platform Abstraction

### Added

* Platform abstraction layer for watchers:

  * Linux (inotify)
  * macOS (planned)
  * Windows (planned)
* Unified event interface across platforms

### Improved

* Consistent behavior across operating systems
* Robust path handling

---

## [0.5.0] - Integration Readiness

### Added

* Structured event model compatible with sync module
* Stable event types:

  * FileCreated
  * FileUpdated
  * FileDeleted
* Hooks for integration with higher-level systems (via events)

### Improved

* Clear separation between observation and decision-making
* Reduced false positives in change detection

---

## [0.6.0] - Performance Improvements

### Added

* Optimized scanning strategies
* Lightweight internal caching for file states

### Improved

* Faster startup scan
* Reduced CPU usage during idle monitoring

---

## [0.7.0] - Extraction Ready

### Added

* Namespace stabilization (`softadastra::fs`)
* Public API cleanup
* Documentation for exposed interfaces

### Improved

* Decoupling from application-specific assumptions
* Prepared for reuse in:

  * Softadastra Sync OS
  * SDK
  * Independent filesystem tools

---

## Roadmap

### Planned

* Native rename detection (move tracking)
* Ignore rules (.gitignore-style)
* Incremental hashing for large files
* Large directory optimization
* Advanced event batching strategies

---

## Philosophy

The `fs` module is the **eyes of the system**.

> It observes everything, but decides nothing.

---

## Rules

* Never include sync logic
* Never interact with network
* Never store authoritative state
* Always allow reconciliation via snapshots

---

## Summary

The `fs` module ensures:

* Reliable filesystem observation
* Accurate change detection
* Structured event emission

It is the **entry point of all local changes** in Softadastra.
