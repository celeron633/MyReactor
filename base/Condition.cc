#include "Condition.hh"
#include "Log.hh"

using namespace base;

Condition::Condition()
{
    _mutex = new Mutex();
    if (pthread_cond_init(&_cond, NULL) != 0) {
        LOG_ERROR("pthread_cond_init failed");
    }
}

Condition::~Condition()
{
    if (pthread_cond_destroy(&_cond) != 0) {
        LOG_ERROR("pthread_cond_destroy failed");
        delete _mutex;
    }
}

bool Condition::Wait()
{
    if (pthread_cond_wait(&_cond, _mutex->Get()) != 0) {
        LOG_ERROR("pthread_cond_wait failed");
        return false;
    }

    return true;
}

bool Condition::Signal()
{
    if (pthread_cond_signal(&_cond) != 0) {
        LOG_ERROR("pthread_cond_signal failed");
        return false;
    }

    return true;
}

bool Condition::Broadcast()
{
    if (pthread_cond_broadcast(&_cond) != 0) {
        LOG_ERROR("pthread_cond_broadcast failed");
        return false;
    }

    return true;
}


#include "Thread.hh"
Condition g_cond;

void TestFunc(void) {
    g_cond.Wait();

    for (int i = 0; i < 10; i++) {
        printf("i = %d\n", i);
        usleep(300 * 1000);
    }
}

/*
int main(int argc, char *argv[])
{
    Thread th(TestFunc);
    th.Start();

    for (auto j = 0; j < 10; ++j) {
        printf("j = %d\n", j);
        usleep(300 * 1000);
    }

    g_cond.Broadcast();

    th.Join();

    return 0;
} */