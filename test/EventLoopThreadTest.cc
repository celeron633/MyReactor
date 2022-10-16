#include "EventLoopThread.hh"

#include <iostream>

int main()
{
    net::EventLoopThread thread;
    thread.start();
    thread.join();
    return 0;
}