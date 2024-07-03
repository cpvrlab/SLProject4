#include "WindowsDirWatcher.h"

#include <cassert>

constexpr unsigned MIN_TIME_BETEWEEN_CHANGES_MS = 1000;

WindowsDirWatcher::WindowsDirWatcher(std::filesystem::path path)
  : _path(path)
{
    _dirHandle = CreateFile(path.string().c_str(),
                            FILE_LIST_DIRECTORY,
                            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                            NULL,
                            OPEN_EXISTING,
                            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                            NULL);

    ZeroMemory(&_overlapped, sizeof(OVERLAPPED));
    _overlapped.hEvent = CreateEvent(NULL, FALSE, 0, NULL);

    BOOL result = ReadDirectoryChangesW(_dirHandle,
                                        _changeBuffer,
                                        sizeof(_changeBuffer) / sizeof(char),
                                        TRUE,
                                        FILE_NOTIFY_CHANGE_LAST_WRITE,
                                        NULL,
                                        &_overlapped,
                                        NULL);
    assert(result);
}

bool WindowsDirWatcher::hasChanged()
{
    DWORD status = WaitForSingleObject(_overlapped.hEvent, 0);
    if (status != WAIT_OBJECT_0)
        return false;

    DWORD bytesTransferred;
    GetOverlappedResult(_dirHandle, &_overlapped, &bytesTransferred, FALSE);
    FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)_changeBuffer;

    bool hasChanged = false;

    while (true)
    {
        TimePoint now     = Clock::now();
        auto      delta   = now - _lastChangeTime;
        auto      deltaMs = std::chrono::duration_cast<std::chrono::milliseconds>(delta);

        if (deltaMs.count() >= MIN_TIME_BETEWEEN_CHANGES_MS)
        {
            _lastChangeTime = now;
            hasChanged      = true;
        }

        if (event->NextEntryOffset)
            event = (FILE_NOTIFY_INFORMATION*)((char*)event + event->NextEntryOffset);
        else
            break;
    }

    BOOL result = ReadDirectoryChangesW(_dirHandle,
                                        _changeBuffer,
                                        sizeof(_changeBuffer) / sizeof(char),
                                        FALSE,
                                        FILE_NOTIFY_CHANGE_LAST_WRITE,
                                        NULL,
                                        &_overlapped,
                                        NULL);
    assert(result);

    return hasChanged;
}