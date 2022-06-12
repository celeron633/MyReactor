#include "Mutex.hh"
#include "Log.hh"

using namespace base;

Mutex::Mutex()
{
    if (pthread_mutex_init(&_mutex, NULL) != 0) {
        LOG_ERROR("pthread_mutex_init failed");
        perror("pthread_mutex_init");
    }
}

Mutex::~Mutex()
{
    if (pthread_mutex_destroy(&_mutex) != 0) {
        LOG_ERROR("pthread_mutex_destroy failed");
    }
}

bool Mutex::Lock()
{
    if (pthread_mutex_lock(&_mutex) != 0) {
        LOG_ERROR("pthread_mutex_lock failed");
        return false;
    }

    return true;
}

bool Mutex::Unlock()
{
    if (pthread_mutex_unlock(&_mutex) != 0) {
        LOG_ERROR("pthread_mutex_unlock failed");
    }

     return true;
}

pthread_mutex_t * Mutex::Get()
{
    return &_mutex;
}

/*
int main(int argc, char *argv[])
{
    Mutex m;
    m.Lock();
    m.Lock();
    m.Unlock();
    return 0;
} */