#ifndef __CHANNEL_HH__
#define __CHANNEL_HH__

#include "Callbacks.hh"
#include "NonCopyable.hh"
#include "Mutex.hh"
#include "Timestamp.hh"

#include <unistd.h>
#include <sys/epoll.h>

#include <iostream>
#include <functional>

using base::Timestamp;
using base::NonCopyable;
using std::function;

namespace net {

// class declaration
class EventLoop;

// one socket fd regist to one eventloop
class Channel : NonCopyable {
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    // EventLoop中loop返回后处理事件
    void HandleEvent(Timestamp timestamp);

    // 是否关注了可写事件
    bool IsWriteEnabled();

    // channel index
    void SetIndex(int index);
    int GetIndex();

    // 返回channel对应的fd
    int GetFd();
    // 返回channel对应的loop
    EventLoop* GetEventLoop();

    // events && revents
    int GetEvents();
    int GetrEvents();

    // epoll_wait返回后调用
    void SetrEvents(int events);

    // 设置Channel对应事件的回调
    void SetReadCallback(const ReadCallback& cb);
    void SetWriteCallback(const WriteCallback& cb);
    void SetCloseCallback(const CloseCallback& cb);
    void SetErrorCallback(const ErrorCallback& cb);

    // 开启/关闭读写
    void EnableRead();
    void DisableRead();
    void EnableWrite();
    void DisableWrite();
    void DisableAll();
public:
    void Remove();
private:
    void Update();
public:
   static string Events2String(int events);

private:
    // EventLoop对象的指针
    EventLoop* _loop;
    // Channel对象对应的fd 
    int _fd;
    // Channel对象的序号
    int _index;

    // 注册到epoll红黑树上去时的fd对应的感兴趣的事件
    int _events;
    // epoll返回的事件  epoll_wait
    int _rEvents;

    // callbacks for channel read/write
    ReadCallback _readCallback;
    WriteCallback _writeCallback;
    CloseCallback _closeCallback;
    ErrorCallback _errorCallback;

    // 静态常量 epoll对应读写的bit
    static const int kReadEvent;
    static const int kWriteEvent;
    static const int kNoneEvent;

    // 是否注册了对写事件的关注
    bool _isWriteEnabled;
};

}

#endif