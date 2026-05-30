# softadastra/fs

> Filesystem observation primitives for reliable Softadastra products.

`softadastra/fs` provides the filesystem layer of the Softadastra C++ stack.

Softadastra builds reliability-first products for local-first, offline-first, and distributed applications. This module turns filesystem state into deterministic snapshots and structured change events.

## Purpose

`softadastra/fs` exists to observe files safely and consistently.

It is used by higher-level modules such as WAL, Store, Sync, Metadata, SDKs, and product infrastructure.

It is designed to be:

- Portable
- Deterministic
- Event-oriented
- Snapshot-based
- Product-ready

## What it provides

This module provides filesystem primitives such as:

- Path normalization
- Directory scanning
- Filesystem snapshots
- Snapshot diffing
- File state modeling
- Structured file events
- Filesystem watchers
- Event batches

## What it does not do

`softadastra/fs` does not contain:

- Sync decisions
- Conflict resolution
- Storage engines
- Network transport
- Product-specific logic

It observes filesystem changes, but it does not decide how those changes should be synchronized or stored.

## Core model

```txt
Filesystem
     |
Scanner / Watcher
     |
Snapshot
     |
Diff
     |
File events
```

Softadastra treats the filesystem as a source of structured events.

Instead of trusting raw OS events directly, the module can scan, compare snapshots, and produce deterministic changes.

## Where it fits

```txt
Softadastra products
        |
SDKs and product APIs
        |
Sync, WAL, Store, Metadata
        |
softadastra/fs
        |
softadastra/core
```

`softadastra/fs` depends on `softadastra/core` and provides filesystem observation for higher-level modules.

## Installation

```bash
vix add @softadastra/fs
```

## Usage

```cpp
#include <softadastra/fs/Fs.hpp>
```

## Example

```cpp
#include <softadastra/fs/Fs.hpp>
#include <iostream>

int main()
{
    auto root = softadastra::fs::path::Path::from("./data");

    if (!root)
    {
        std::cout << "invalid path\n";
        return 1;
    }

    auto snapshot = softadastra::fs::scanner::Scanner::scan(root.value());

    if (!snapshot)
    {
        std::cout << "scan failed\n";
        return 1;
    }

    for (const auto& [_, file] : snapshot.value().all())
    {
        std::cout << file.path.str() << "\n";
    }

    return 0;
}
```

## Requirements

- C++20
- `softadastra/core`
- Platform filesystem APIs when watcher backends are enabled

## Documentation

For the full documentation, visit [docs.softadastra.com](https://docs.softadastra.com).

## License

Licensed under the Apache License, Version 2.0.
