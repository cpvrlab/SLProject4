/**
 * \file      AsyncWorker.h
 * \brief     Declaration of an async worker thread class
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef ASYNC_WORKER_H
#define ASYNC_WORKER_H

#include <thread>
#include <atomic>

//-----------------------------------------------------------------------------
//! AsyncWorker implements a async worker thread
class AsyncWorker
{
public:
    virtual ~AsyncWorker();

    void start();
    void stop();
    bool isReady();

protected:
    bool stopRequested();
    void setReady();
    virtual void run();

private:
    std::thread _thread;
    std::atomic_bool _stop{false};
    std::atomic_bool _ready{false};
};
//-----------------------------------------------------------------------------

#endif
