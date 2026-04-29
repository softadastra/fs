/*
 * WinWatcher.cpp
 */

#include <windows.h>

#include <atomic>
#include <chrono>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include <softadastra/core/Core.hpp>
#include <softadastra/fs/events/EventBatch.hpp>
#include <softadastra/fs/events/FileEvent.hpp>
#include <softadastra/fs/path/Path.hpp>
#include <softadastra/fs/state/FileState.hpp>
#include <softadastra/fs/types/FileEventType.hpp>
#include <softadastra/fs/watcher/IWatcherBackend.hpp>

namespace softadastra::fs::watcher
{

  namespace events = softadastra::fs::events;
  namespace path = softadastra::fs::path;
  namespace state = softadastra::fs::state;
  namespace types = softadastra::fs::types;
  namespace core_errors = softadastra::core::errors;

  class WinWatcher : public IWatcherBackend
  {
  public:
    using Callback = IWatcherBackend::Callback;
    using Result = IWatcherBackend::Result;

    WinWatcher() = default;

    ~WinWatcher() override
    {
      stop();
    }

    [[nodiscard]] Result start(const path::Path &root, Callback callback) override
    {
      if (running_)
      {
        return Result::err(
            core_errors::Error::make(
                core_errors::ErrorCode::InvalidState,
                "watcher already running"));
      }

      root_ = root;
      callback_ = std::move(callback);

      directory_ = CreateFileA(
          root_.str().c_str(),
          FILE_LIST_DIRECTORY,
          FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
          nullptr,
          OPEN_EXISTING,
          FILE_FLAG_BACKUP_SEMANTICS,
          nullptr);

      if (directory_ == INVALID_HANDLE_VALUE)
      {
        return Result::err(
            core_errors::Error::make(
                core_errors::ErrorCode::InternalError,
                "failed to open directory for watching",
                core_errors::ErrorContext(root_.str())));
      }

      running_ = true;

      worker_ = std::thread([this]
                            { loop(); });

      return Result::ok();
    }

    void stop() override
    {
      if (!running_)
      {
        return;
      }

      running_ = false;

      if (directory_ != INVALID_HANDLE_VALUE)
      {
        CancelIoEx(directory_, nullptr);
      }

      if (worker_.joinable())
      {
        worker_.join();
      }

      close_directory();
    }

    [[nodiscard]] bool is_running() const noexcept override
    {
      return running_;
    }

  private:
    void loop()
    {
      std::vector<char> buffer(8192);

      while (running_)
      {
        DWORD bytes_returned = 0;

        const BOOL ok = ReadDirectoryChangesW(
            directory_,
            buffer.data(),
            static_cast<DWORD>(buffer.size()),
            TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME |
                FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_LAST_WRITE |
                FILE_NOTIFY_CHANGE_SIZE,
            &bytes_returned,
            nullptr,
            nullptr);

        if (!ok || bytes_returned == 0)
        {
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
          continue;
        }

        events::EventBatch batch;
        std::size_t offset = 0;

        while (offset < bytes_returned)
        {
          const auto *info =
              reinterpret_cast<const FILE_NOTIFY_INFORMATION *>(
                  buffer.data() + offset);

          handle_event(*info, batch);

          if (info->NextEntryOffset == 0)
          {
            break;
          }

          offset += info->NextEntryOffset;
        }

        if (!batch.empty() && callback_)
        {
          callback_(batch);
        }
      }
    }

    void handle_event(
        const FILE_NOTIFY_INFORMATION &info,
        events::EventBatch &batch)
    {
      auto type = event_type(info.Action);

      if (!type.has_value())
      {
        return;
      }

      const std::wstring name(
          info.FileName,
          info.FileNameLength / sizeof(wchar_t));

      const std::string utf8_name = narrow(name);

      if (utf8_name.empty())
      {
        return;
      }

      const std::string full_path = root_.str() + "/" + utf8_name;

      auto path_result = path::Path::from(full_path);

      if (path_result.is_err())
      {
        return;
      }

      state::FileState current{};
      current.path = path_result.value();

      batch.add(events::FileEvent{
          *type,
          current,
          std::nullopt});
    }

    [[nodiscard]] static std::optional<types::FileEventType>
    event_type(DWORD action) noexcept
    {
      switch (action)
      {
      case FILE_ACTION_ADDED:
      case FILE_ACTION_RENAMED_NEW_NAME:
        return types::FileEventType::Created;

      case FILE_ACTION_MODIFIED:
        return types::FileEventType::Updated;

      case FILE_ACTION_REMOVED:
      case FILE_ACTION_RENAMED_OLD_NAME:
        return types::FileEventType::Deleted;

      default:
        return std::nullopt;
      }
    }

    [[nodiscard]] static std::string narrow(const std::wstring &value)
    {
      if (value.empty())
      {
        return {};
      }

      const int size = WideCharToMultiByte(
          CP_UTF8,
          0,
          value.data(),
          static_cast<int>(value.size()),
          nullptr,
          0,
          nullptr,
          nullptr);

      if (size <= 0)
      {
        return {};
      }

      std::string result(static_cast<std::size_t>(size), '\0');

      WideCharToMultiByte(
          CP_UTF8,
          0,
          value.data(),
          static_cast<int>(value.size()),
          result.data(),
          size,
          nullptr,
          nullptr);

      return result;
    }

    void close_directory() noexcept
    {
      if (directory_ != INVALID_HANDLE_VALUE)
      {
        CloseHandle(directory_);
        directory_ = INVALID_HANDLE_VALUE;
      }
    }

  private:
    path::Path root_{};
    Callback callback_{};

    HANDLE directory_{INVALID_HANDLE_VALUE};

    std::thread worker_{};
    std::atomic<bool> running_{false};
  };

} // namespace softadastra::fs::watcher
