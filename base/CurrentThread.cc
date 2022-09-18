#include "CurrentThread.hh"

#include <cstdio>

namespace base {

namespace CurrentThread {
__thread pid_t _tid;    // TLS variable

pid_t GetThreadId()
{
    if (_tid == 0) {
        CacheTid();
    }

    return _tid;
}

void CacheTid()
{
    _tid = syscall(SYS_gettid);
}

};

};

/*
int main(int argc, char *argv[])
{
    printf("%d\n", base::CurrentThread::GetThreadId());
    return 0;
}
*/