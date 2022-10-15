#include "EventLoopThread.hh"

#include <iostream>

int main()
{
    EventLoopThread thread;
    thread.start();
    thread.join();
    return 0;
}