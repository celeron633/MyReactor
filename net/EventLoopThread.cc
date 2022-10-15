#include "EventLoopThread.hh"
#include "Log.hh"

// therad support
#include <pthread.h>

using namespace net;
using namespace base;
using namespace std;

void* EventLoopThread::loopThreadRoutine(void* args)
{
    // FIXME: unsafe cast
    EventLoopThread* obj = (EventLoopThread*)args;
    EventLoop* loop = new EventLoop();
    obj->_loop = loop;

    // infinite loop here 
    loop->loop();

    return NULL;
}

EventLoopThread::EventLoopThread() : _loop(NULL)
{
    // reset class member
    bzero(&_thread, sizeof(_thread));
}

EventLoopThread::~EventLoopThread()
{
    stop();
}

void EventLoopThread::start()
{
    if (pthread_create(&this->_thread, NULL, &EventLoopThread::loopThreadRoutine, (void*)this) != 0) { // pass this pointer to static function
        LOG_ERROR("pthread_create failed!");
        perror("pthread_create");
    }

#ifdef __linux__ // linux use unsigned long int for pthread_t, other plat. may not
    LOG_INFO("EventLoopThread object start OK! pthread_t: [0x%lx]", _thread);
#else
    LOG_INFO("EventLoopThread object start OK!");
#endif
}

void EventLoopThread::stop()
{
    if (this->_loop == NULL) {
        return;
    }

    // set stop flag
    _loop->stop();

    // TODO: wait thread to die
}

void EventLoopThread::join()
{
    pthread_join(this->_thread, NULL);
}