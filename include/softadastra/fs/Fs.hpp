/**
 *
 *  @file Fs.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Softadastra.
 *  All rights reserved.
 *  https://github.com/softadastra/softadastra
 *
 *  Licensed under the Apache License, Version 2.0.
 *
 *  Softadastra FS
 *
 */

#ifndef SOFTADASTRA_FS_FS_HPP
#define SOFTADASTRA_FS_FS_HPP

// Events
#include <softadastra/fs/events/EventBatch.hpp>
#include <softadastra/fs/events/FileEvent.hpp>

// Path
#include <softadastra/fs/path/Path.hpp>
#include <softadastra/fs/path/PathNormalizer.hpp>
#include <softadastra/fs/path/PathUtils.hpp>

// Scanner
#include <softadastra/fs/scanner/Scanner.hpp>

// Snapshot
#include <softadastra/fs/snapshot/Snapshot.hpp>
#include <softadastra/fs/snapshot/SnapshotBuilder.hpp>
#include <softadastra/fs/snapshot/SnapshotDiff.hpp>

// State
#include <softadastra/fs/state/FileMetadata.hpp>
#include <softadastra/fs/state/FileState.hpp>

// Types
#include <softadastra/fs/types/FileEventType.hpp>
#include <softadastra/fs/types/FileType.hpp>
#include <softadastra/fs/types/WatchOptions.hpp>

// Utils
#include <softadastra/fs/utils/IgnoreMatcher.hpp>

// Watcher
#include <softadastra/fs/watcher/IWatcherBackend.hpp>
#include <softadastra/fs/watcher/PollingWatcher.hpp>
#include <softadastra/fs/watcher/Watcher.hpp>

#endif // SOFTADASTRA_FS_FS_HPP
