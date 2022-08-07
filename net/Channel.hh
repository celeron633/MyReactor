#ifndef __CHANNEL_HH__
#define __CHANNEL_HH__

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

class EventLoop;

// one socket fd regist to one eventloop
class Channel : NonCopyable {
    using ReadCallback = function<void (Timestamp)>;
    using WriteCallback = function<void ()>;
    using CloseCallback = function<void ()>;
    using ErrorCallback = function<void ()>;

public:
    Channel(EventLoop *loop, int fd);
    ~Channel();

    // EventLoop中loop返回后处理事件
    void HandleEvent(Timestamp timestamp);

    // index
    void SetIndex(int idx)
    {
        this->_index = idx;
    }
    int GetIndex()
    {
        return _index;
    }

    // 返回channel对应的fd
    int GetFd()
    {
        return _fd;
    }
    // 返回channel对应的loop
    EventLoop *GetEventLoop()
    {
        return _loop;
    }

    // events && revents
    int GetEvents()
    {
        return _events;
    }
    int GetRevents()
    {
        return _rEvents;
    }

    // epoll_wait返回后调用
    void SetRevents(int events)
    {
        this->_rEvents = events;
    }

    // 设置Channel对应事件的回调
    void SetReadCallback(ReadCallback cb)
    {
        _readCallback = cb;
    }
    void SetWriteCallback(WriteCallback cb)
    {
        _writeCallback = cb;
    }
    void SetCloseCallback(CloseCallback cb)
    {
        _closeCallback = cb;
    }
    void SetErrorCallback(ErrorCallback cb)
    {
        _errorCallback = cb;
    }

    // 开启/关闭读写
    void EnableRead()
    {
        this->_events |= kReadEvent;
        Update();
    }
    void DisableRead()
    {
        this->_events &= ~kReadEvent;
        Update();
    }
    void EnableWrite()
    {
        this->_events |= kWriteEvent;
        Update();
    }
    void DisableWrite()
    {
        this->_events &= ~kWriteEvent;
        Update();
    }
    void DisableAll()
    {
        this->_events = kNoneEvent;
        Update();
    }

private:
    void Update();

private:
    // EventLoop对象的指针
    EventLoop *_loop;
    // Channel对象对应的fd 
    int _fd;
    // Channel对象的序号
    int _index;

    // 注册到epoll红黑树上去时的fd对应的感兴趣的事件
    int _events;
    // epoll返回的事件
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
};

}

#endif