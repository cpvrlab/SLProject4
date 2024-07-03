#ifndef WINDOWSDIRWATCHER_H
#define WINDOWSDIRWATCHER_H

#include <chrono>
#include <filesystem>

#include <windows.h>

//-----------------------------------------------------------------------------
class WindowsDirWatcher
{
public:
    WindowsDirWatcher(std::filesystem::path path);
    bool hasChanged();

private:
    typedef std::chrono::file_clock        Clock;
    typedef std::chrono::time_point<Clock> TimePoint;
    typedef std::chrono::duration<Clock>   Duration;

    std::filesystem::path _path;
    HANDLE                _dirHandle;
    OVERLAPPED            _overlapped;
    char                  _changeBuffer[2048];
    TimePoint             _lastChangeTime;
};
//-----------------------------------------------------------------------------

#endif