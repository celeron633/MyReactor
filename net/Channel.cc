#include "Channel.hh"
#include "Log.hh"
#include "EventLoop.hh"

#include <sys/epoll.h>

using namespace base;
using namespace net;
using namespace std;

// event attribute for epoll
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;
const int Channel::kNoneEvent = 0;

Channel::Channel(EventLoop *loop, int fd) : _loop(loop), _fd(fd), _index(-1), _events(0), _rEvents(0)
{
    LOG_INFO("Channel construct");
}

Channel::~Channel()
{
    LOG_INFO("Channel destruct");
}

void Channel::HandleEvent(Timestamp timestamp)
{
    LOG_DEBUG("HandleEvent begin");
    LOG_DEBUG("timestamp: [%s]", timestamp.ConvertToString().c_str());

    // 连接关闭
    if (_rEvents & EPOLLHUP && !(_rEvents & EPOLLIN)) {
        LOG_INFO("close event");
        if (_closeCallback) {
            _closeCallback();
        }
    }
    // 错误
    if (_rEvents & EPOLLERR) {
        LOG_INFO("error event");
        if (_errorCallback) {
            _errorCallback();
        }
    }
    // 读取
    if (_rEvents & (EPOLLIN | EPOLLPRI)) {
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
    // _loop->UpdateChannel(this);
}