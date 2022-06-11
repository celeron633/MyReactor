#include "Thread.hh"

#include <cstdint>
#include <cstdio>

using namespace base;

// wrapper for pthread to run std::function
static void * PthreadFuncWrapper(void *args)
{
    assert(args != NULL);

    Thread *thread = (Thread *)args;
    thread->RunCallbackFunc();
    return NULL;
}

pthread_t Thread::GetThreadId() const
{
    return _tid;
}

bool Thread::Detach()
{
    assert(_tid != 0);

    if (pthread_detach(_tid) != 0) {
        return false;
    }

    return true;
}

bool Thread::Start()
{
    if (pthread_create(&_tid, NULL, PthreadFuncWrapper, (void *)this) != 0) {
        LOG_ERROR("pthread_create failed");
        return false;
    } else {
        LOG_DEBUG("pthread_create successful, ThreadID: [0x%lx]", _tid);
    }

    _started = true;

    return true;
}

bool Thread::Join()
{
    LOG_DEBUG("ThreadID: [0x%lx]", _tid);

    if (pthread_join(_tid, NULL) != 0) {
        LOG_ERROR("pthread_join failed");
        return false;
    }

    _started = false;

    return true;
}

void Thread::RunCallbackFunc()
{
    _func();
}

void testFunc(void)
{
    uint64_t i = 0;
    for (;;) {
        printf("%lu\n", i++);
        usleep(300 * 1000);

        if (i == 10) {
            return;
        }
    }
}

int main(int argc, char *argv[])
{
    Thread th(testFunc);
    Thread th2(testFunc);
    th.Start();
    th2.Start();
    th.Join();
    th2.Join();
    return 0;
}
