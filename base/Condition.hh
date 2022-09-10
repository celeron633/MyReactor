#ifndef __CONDITION_HH__
#define __CONDITION_HH__

#include "Mutex.hh"

#include <pthread.h>
#include <unistd.h>

#include <iostream>

namespace base
{

class Condition {
public:
    Condition();
    ~Condition();

    bool Wait();
    bool Signal();
    bool Broadcast();

private:
    pthread_cond_t _cond;
    Mutex* _mutex;
};

}

#endif