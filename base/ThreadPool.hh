#ifndef __THREAD_POOL_HH__
#define __THREAD_POOL_HH__

#include "Mutex.hh"
#include "Condition.hh"
#include "Thread.hh"

#include <vector>
#include <deque>

using std::deque;
using std::vector;

typedef function<void (void)> Task;

namespace base {

class ThreadPool {
public:
    ThreadPool(int threadsCount = 10);
    ~ThreadPool();

    // add a task to task queue
    bool AddTask(Task t);
    // get active thread count
    int GetThreadCount();
    // start this threadPool to handle pending task
    void Start();
    // whether this threadpool is already started
    bool Started();
protected:
    // consume tasks, need to run in another thread, always use with 'bind'
    void RunTask(void);


private:
    bool _started;

    // holding tasks, need mutex protect
    deque<Task> _tasks;
    // holding threads
    vector<Thread *> _threads;

    // threads count
    int _threadsCount;

    // Concurrency control
    Mutex _mutex;
    Condition _cond;
};

}

#endif