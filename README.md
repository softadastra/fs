# softadastra/fs

> Filesystem observation and synchronization primitives.

The `fs` module provides a **portable, deterministic filesystem layer** for Softadastra.

It enables:

- Scanning directories
- Building snapshots
- Detecting changes (diff)
- Watching filesystem events in real time

## Purpose

The goal of `softadastra/fs` is simple:

> Observe the filesystem and produce deterministic change events.

It is the foundation for:

- sync engine
- WAL operations
- metadata updates
- real-time applications

## Core Principle

> *The filesystem is a stream of events.*

Instead of reacting directly to OS events, Softadastra:

1. Builds a snapshot
2. Computes a diff
3. Produces structured events

## Responsibilities

The `fs` module provides:

- Path abstraction (`Path`)
- Snapshot system (`Snapshot`)
- Change detection (`SnapshotDiff`)
- Scanner (`Scanner`)
- Watchers (Polling, inotify, FSEvents, Windows)
- Event system (`FileEvent`, `EventBatch`)
- File state modeling (`FileState`, `FileMetadata`)

## What this module does NOT do

- No sync logic
- No storage logic
- No network logic
- No conflict resolution

> 👉 It observes. It does not decide.

## Design Principles

### 1. Determinism

Same input → same output, across machines, OS, and time.

### 2. Normalized paths

All paths are normalized, canonical, and comparable.

### 3. Strong typing

No raw strings — only `Path`, `FileState`, `FileEvent`.

### 4. Portable abstraction

| Platform | Backend |
|----------|---------|
| Linux | inotify |
| macOS | FSEvents |
| Windows | ReadDirectoryChangesW |
| fallback | polling |

## Module Structure

```
modules/fs/
├── include/softadastra/fs/
│   ├── events/
│   ├── path/
│   ├── scanner/
│   ├── snapshot/
│   ├── state/
│   ├── types/
│   ├── utils/
│   └── watcher/
├── platform/
│   ├── linux/
│   ├── mac/
│   └── windows/
```

## Core Components

### Path

Strong, normalized filesystem paths:

```cpp
auto p = Path::from("a/./b/../c").value();
// -> "a/c"
```

### Snapshot

Represents filesystem state:

```cpp
auto snap = SnapshotBuilder::build(root).value();
```

### Diff

Detects changes between snapshots:

```cpp
auto events = SnapshotDiff::compute(a.all(), b.all());
```

### Scanner

Simple API to build snapshots:

```cpp
auto snap = Scanner::scan(root);
```

### Watcher

Real-time filesystem monitoring:

```cpp
Watcher watcher;

watcher.start(root, [](const EventBatch& batch) {
    for (const auto& e : batch.all()) {
        // handle event
    }
});
```

### Event Model

```cpp
FileEvent {
    type:     Created | Updated | Deleted
    current:  FileState
    previous: optional<FileState>
}
```

## Example

```cpp
#include <softadastra/fs/Fs.hpp>

using namespace softadastra::fs;

int main()
{
    auto root = path::Path::from("./data").value();

    auto snap = scanner::Scanner::scan(root).value();

    for (const auto& [_, file] : snap.all())
    {
        std::cout << file.path.str() << "\n";
    }
}
```

## Dependencies

**Internal:** `softadastra/core`

**External:**
- C++20 standard library
- OS APIs (inotify, FSEvents, WinAPI)

## Integration

Used by:

- `wal`
- `sync`
- `store`
- `metadata`

## Rules

- No business logic
- No sync decisions
- No side effects
- Deterministic output

## When to modify this module

Only if:

- It improves filesystem observation
- It remains deterministic
- It does not introduce coupling

## Roadmap

- [ ] Metadata enrichment (hash, permissions)
- [ ] Ignore file support (`.softadastraignore`)
- [ ] Incremental scanning
- [ ] Watcher backpressure handling
- [ ] Async event pipeline

## Philosophy

> The filesystem is not state.
> It is a sequence of changes.

## Summary

- Cross-platform filesystem layer
- Snapshot + diff model
- Real-time watchers
- Deterministic and portable

## Installation

```bash
vix add @softadastra/fs
```

## License

Apache License 2.0
