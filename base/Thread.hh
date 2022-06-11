#ifndef __THREAD_HH_
#define __THREAD_HH_

#include "Log.hh"

#include <cassert>
#include <functional>

#include <unistd.h>
#include <pthread.h>

using std::function;

typedef function<void (void)> CallbackFunc;

namespace base {

class Thread {
public:
    Thread(CallbackFunc func) : _func(func), _tid(0), _started(false)
    {

    }

    ~Thread()
    {

    }

    pthread_t GetThreadId() const;

    // start this thread
    bool Start();
    // detach this thread
    bool Detach();
    // join
    bool Join();
    // run functor
    void RunCallbackFunc();
protected:
    
private:
    bool _started;
    pthread_t _tid;
    CallbackFunc _func;
};

}

#endif