# softadastra/fs

> Filesystem observation layer for local-first systems.

The `fs` module is responsible for **observing and describing changes in the local filesystem**.

It is the **entry point of all local data changes** in Softadastra systems.

## Purpose

The goal of `softadastra/fs` is simple:

> Detect changes in a directory and emit structured, reliable events.

## Core Principle

> Observe everything. Decide nothing.

This module:

* Detects changes
* Describes state
* Emits events

It does **not** decide what to do with them.

## Responsibilities

The `fs` module provides:

* Directory watching
* Initial filesystem scanning
* File state representation
* Snapshot creation
* Snapshot comparison (diff)
* Path utilities

## What this module does NOT do

* No sync logic (sync module)
* No durability (wal module)
* No network communication (transport module)
* No state ownership (metadata module)

👉 It is a pure observation layer.

## Design Principles

### 1. Passive

The module observes changes but never triggers business logic.

### 2. Deterministic

Given the same filesystem state, it must produce the same results.

### 3. Reliable

* Duplicate events are acceptable
* Missing events are not

### 4. Decoupled

No dependency on higher-level modules.

## Module Structure

```id="fs9x21"
modules/fs/
├── include/softadastra/fs/
│   ├── Watcher.hpp
│   ├── Scanner.hpp
│   ├── PathUtils.hpp
│   ├── FileState.hpp
│   └── Snapshot.hpp
└── src/
```

## Core Components

### Watcher

Monitors a directory in real time.

Detects:

* File creation
* File modification
* File deletion

### Scanner

Performs an initial scan of a directory.

Used for:

* Bootstrapping state
* Rebuilding snapshots

### FileState

Represents a file at a given moment.

Includes:

* Path
* Size
* Last modified time
* Optional hash

### Snapshot

Represents the full state of a directory.

Used to:

* Compare states
* Detect differences
* Feed the sync engine

### PathUtils

Utility helpers for:

* Path normalization
* Relative / absolute resolution
* Cross-platform handling

## Event Model

The module emits events such as:

* `FileCreated`
* `FileUpdated`
* `FileDeleted`

Events can come from:

* Real-time watcher
* Snapshot diff

## Example Usage

```cpp id="ex7"
#include <softadastra/fs/watcher/Watcher.hpp>
#include <softadastra/fs/path/Path.hpp>

using namespace softadastra::fs;

int main()
{
  watcher::Watcher watcher;

  auto path = path::Path::from("./data").value();

  watcher.start(path, [](const events::EventBatch &batch)
  {
    // handle filesystem changes
  });

  std::this_thread::sleep_for(std::chrono::minutes(1));
  watcher.stop();
}
```

## Integration

Used by:

* sync (primary consumer)
* metadata (indirectly via sync)

## Reliability Strategy

To guarantee correctness:

* Watcher captures real-time events
* Snapshot diff acts as a fallback
* Event coalescing reduces noise

## Dependencies

### Internal

* softadastra/core

### External

* OS filesystem APIs:

  * Linux: inotify
  * macOS: FSEvents (planned)
  * Windows: ReadDirectoryChangesW (planned)

## MVP Scope

* Single directory watcher
* Basic file events
* Snapshot diff support
* No advanced rename detection

## Roadmap

* Rename detection (true move tracking)
* Ignore rules (.gitignore-style)
* Incremental hashing
* Large directory optimization
* Smarter event batching

## Rules

* Never trigger sync directly
* Never store state permanently
* Never assume event completeness
* Always allow reconciliation via snapshot

## Philosophy

The `fs` module is the **eyes of the system**.

> It observes everything, but decides nothing.

## Summary

* Detects filesystem changes
* Emits structured events
* Feeds the sync engine
* Fully decoupled

## Installation

```bash
vix add @softadastra/fs
```

## License

See root LICENSE file.
