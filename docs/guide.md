# Filesystem Guide

The `softadastra/fs` module lets you **observe, scan, and track changes in the filesystem**.

It works the same across Linux, macOS, and Windows.

## Installation

```bash
vix add @softadastra/fs
```

### Import

```cpp
#include <softadastra/fs/Fs.hpp>
```

## 1. Create a Path

All filesystem operations start with a `Path`:

```cpp
using namespace softadastra::fs;

auto result = path::Path::from("./data");

if (result.is_err()) {
    // handle error
}

auto root = result.value();
```

Paths are:

- normalized
- safe
- strongly typed

## 2. Scan a directory

Read the full filesystem state:

```cpp
auto result = scanner::Scanner::scan(root);

if (result.is_err()) {
    // handle error
}

const auto& snapshot = result.value();

for (const auto& [_, file] : snapshot.all()) {
    std::cout << file.path.str() << "\n";
}
```

## 3. Build a Snapshot

A snapshot represents the filesystem at a moment in time:

```cpp
auto snap = snapshot::SnapshotBuilder::build(root).value();
```

Each file contains:

- `path`
- `type`
- `size`
- `timestamp`

## 4. Detect changes (Diff)

Compare two snapshots:

```cpp
auto diff = snapshot::SnapshotDiff::compute(
    before.all(),
    after.all()
);

for (const auto& e : diff) {
    std::cout << e.current.path.str() << "\n";
}
```

Event types:

- `Created`
- `Updated`
- `Deleted`

## 5. Watch files in real time

Monitor a directory continuously:

```cpp
watcher::Watcher watcher;

auto result = watcher.start(root, [](const events::EventBatch& batch) {
    for (const auto& e : batch.all()) {
        std::cout << e.current.path.str() << "\n";
    }
});

if (result.is_err()) {
    // handle error
}
```

Keep your program alive:

```cpp
while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
```

## How it works

Internally, the watcher:

1. Builds a snapshot
2. Compares it with the previous one
3. Emits events

This guarantees consistent behavior, cross-platform support, and deterministic results.

## Platform support

| OS | Implementation |
|----|---------------|
| Linux | inotify |
| macOS | FSEvents |
| Windows | WinAPI |
| Other | polling |

## Event model

Each change is a `FileEvent`:

```cpp
FileEvent {
    type:     Created | Updated | Deleted
    current:  FileState
    previous: optional<FileState>
}
```

## Common patterns

### Detect file creation

```cpp
if (e.type == types::FileEventType::Created) {
    // new file
}
```

### Filter files

```cpp
if (e.current.path.str().ends_with(".txt")) {
    // handle only text files
}
```

## When to use `fs`

Use this module when you need:

- sync engine
- file monitoring
- backup systems
- live applications
- indexing systems

## What `fs` does NOT do

- no file writing
- no sync logic
- no network
- no storage

> It only observes the filesystem.

## Summary

- Scan directories
- Build snapshots
- Detect changes
- Watch in real time
- Works on all platforms
