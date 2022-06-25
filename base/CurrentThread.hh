#ifndef __CURRENT_THREAD_HH__
#define __CURRENT_THREAD_HH__

#include <unistd.h>
#include <sys/syscall.h>

#include <cstdio>

namespace base {

namespace CurrentThread {
    extern __thread pid_t _tid;
    void CacheTid();
    pid_t GetThreadId();
}

}

#endif