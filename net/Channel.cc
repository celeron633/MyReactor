#include "Channel.hh"
#include "Log.hh"
#include "EventLoop.hh"

#include "Common.hh"

#include <poll.h>

using namespace base;
using namespace net;
using namespace std;

// event attribute for epoll
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;
const int Channel::kNoneEvent = 0;

Channel::Channel(EventLoop* loop, int fd) : _loop(loop), _fd(fd), _index(kInitialIndex), _events(0), _rEvents(0), _isWriteEnabled(false)
{
    LOG_INFO("Channel construct");

    // the following should not be done by constructor
    // 关注读取事件
    // this->EnableRead();
    // 不关注可写事件
    // this->DisableWrite();
}

Channel::~Channel()
{
    LOG_INFO("Channel destruct");
}

bool Channel::IsWriteEnabled()
{
    return _isWriteEnabled;
}

void Channel::SetIndex(int index)
{
    this->_index = index;
}

int Channel::GetIndex()
{
    return _index;
}

// 返回channel对应的fd
int Channel::GetFd()
{
    return _fd;
}
// 返回channel对应的loop
EventLoop* Channel::GetEventLoop()
{
    return _loop;
}

// events && revents
int Channel::GetEvents()
{
    return _events;
}
int Channel::GetrEvents()
{
    return _rEvents;
}

// epoll_wait返回后调用
void Channel::SetrEvents(int events)
{
    this->_rEvents = events;
}

// 设置Channel对应事件的回调
void Channel::SetReadCallback(const ReadCallback& cb)
{
    _readCallback = cb;
}
void Channel::SetWriteCallback(const WriteCallback& cb)
{
    _writeCallback = cb;
}
void Channel::SetCloseCallback(const CloseCallback& cb)
{
    _closeCallback = cb;
}
void Channel::SetErrorCallback(const ErrorCallback& cb)
{
    _errorCallback = cb;
}

// 开启/关闭读写
void Channel::EnableRead()
{
    this->_events |= kReadEvent;
    Update();
}
void Channel::DisableRead()
{
    this->_events &= ~kReadEvent;
    Update();
}
void Channel::EnableWrite()
{
    this->_events |= kWriteEvent;
    Update();
    _isWriteEnabled = true;
}
void Channel::DisableWrite()
{
    this->_events &= ~kWriteEvent;
    Update();
    _isWriteEnabled = false;
}
void Channel::DisableAll()
{
    this->_events = kNoneEvent;
    // this->_index = kDeleteIndex; // done this in EpollPoller
    Update();
}

void Channel::HandleEvent(Timestamp timestamp)
{
    LOG_DEBUG("HandleEvent begin");
    LOG_DEBUG("timestamp: [%s], fd: [%d]", timestamp.ConvertToString().c_str(), this->_fd);

    // 连接关闭
    if (_rEvents & EPOLLHUP && !(_rEvents & EPOLLIN)) {
        LOG_INFO("close event");
        if (_closeCallback) {
            _closeCallback();
        }
    }
    // 错误
    if (_rEvents & (EPOLLERR | POLLNVAL)) {
        LOG_INFO("error event");
        if (_errorCallback) {
            _errorCallback();
        }
    }
    // 读取
    if (_rEvents & (EPOLLIN | EPOLLPRI | POLLRDHUP)) {
        LOG_INFO("read event");
        if (_readCallback) {
            _readCallback(timestamp);
        }
    }
    // 可写
    if (_rEvents & (EPOLLOUT | EPOLLPRI)) {
        LOG_DEBUG("write event");
        if (_writeCallback) {
            _writeCallback();
        }
    }

    LOG_DEBUG("HandleEvent end");
}

void Channel::Update()
{
    _loop->UpdateChannel(this);
}

void Channel::Remove()
{
    _loop->RemoveChannel(this);
}

string Channel::Events2String(int events)
{
    string evStr = "";

    if (events & kReadEvent) {
        evStr += "|EV_READ";
    }
    if (events & kWriteEvent) {
        evStr += "|EV_WRITE";
    }
    evStr += "|";

    return evStr;
}

const char* Channel::Index2String(int index)
{
    switch (index) {
        case kInitialIndex:
            return "initial";
        case kAddedIndex:
            return "added";
        case kDeleteIndex:
            return "deleted";
        default:
            return "unknown";
    }
    return "unknown";
}