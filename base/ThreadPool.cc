#include "ThreadPool.hh"
#include <functional>

using namespace base;
using namespace std;

ThreadPool::ThreadPool(int threadsCount)
{
    this->_threadsCount = threadsCount;
    LOG_DEBUG("threads count: [%d]", _threadsCount);
}

ThreadPool::~ThreadPool()
{
    
}

bool ThreadPool::AddTask(Task t)
{
    LOG_DEBUG("AddTask");
    // protect the '_tasks' which is accessed by more than one thread
    MutexGuard guard(&_mutex);
    this->_tasks.push_back(t);

    // tell one pending thread to fetch a new task
    _cond.Signal();

    return true;
}

int ThreadPool::GetThreadCount()
{
    return _threadsCount;
}

bool ThreadPool::Started()
{
    return _started;
}

void ThreadPool::RunTask()
{
    LOG_DEBUG("RunTask begin");
    // endless loop
    while (true) {
        // wait for comming task
        this->_cond.Wait();

        this->_mutex.Lock();
        Task t;

        if (!_tasks.empty()) {
            t = _tasks.front();
            _tasks.pop_front();
        }

        /*
        if (!_tasks.empty()) {
            // task queue is still not empty, tell other thread to fetch it
            _cond.Signal();
        } */

        this->_mutex.Unlock();

        LOG_DEBUG("handle task begin");
        if (t) {
            // run the task from queue
            t();
        }
         LOG_DEBUG("handle task end");

        // when return, fetch next task from queue
    }
}

void ThreadPool::Start()
{
    for (auto i = 0; i < _threadsCount; ++i) {
        LOG_DEBUG("Starting thread [%d] ...", i);
        // need to use '&ThreadPool::RunTask' to bind with 'this'
        this->_threads.push_back(new Thread(std::bind(&ThreadPool::RunTask, this)));
    }

     for (auto &it : _threads) {
        it->Start();
    }

    // when 'Start' created all threads done, and fired them, trigger the conditional variable to tell them to work
    _cond.Broadcast();
    _started = true;
}

/* unit test 
int main(int argc, char *argv[])
{
    ThreadPool pool(3);
    pool.Start();

    printf("started: %d\n", pool.Started());
    pool.AddTask([] () {for (int i = 0; i < 5; i++) {
        printf("i = %d\n", i);
        usleep(100 * 1000);
    };});

    sleep(1);
    pool.AddTask([] () {for (int i = 0; i < 5; i++) {
        printf("i = %d\n", i);
        usleep(100 * 1000);
    };});

    sleep(1);
    pool.AddTask([] () {for (int i = 0; i < 5; i++) {
        printf("i = %d\n", i);
        usleep(100 * 1000);
    };});

    sleep(1);
    pool.AddTask([] () {for (int i = 0; i < 5; i++) {
        printf("i = %d\n", i);
        usleep(100 * 1000);
    };});

    sleep(300);

    return 0;
} */