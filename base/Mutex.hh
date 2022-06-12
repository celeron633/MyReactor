#ifndef __MUTEX_HH__
#define __MUTEX_HH__

#include <pthread.h>
#include <unistd.h>

#include <iostream>

namespace base {

class Mutex {
public:
    Mutex();
    ~Mutex();

    bool Lock();
    bool Unlock();
    pthread_mutex_t * Get();

private:
    pthread_mutex_t _mutex;
};

class MutexGuard {
public:
    explicit MutexGuard(Mutex *mutex)
    {
        this->_m = mutex;
        _m->Lock();
    }

    ~MutexGuard()
    {
        _m->Unlock();
    }
private:
    Mutex *_m;
};

}
#endif