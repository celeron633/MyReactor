#include "Connector.hh"

#include <assert.h>

using namespace net;
using namespace base;
using namespace std;

Connector::Connector(EventLoop *loop, INetAddr serverAddr) : _loop(loop), _serverAddr(serverAddr), \
    _status(DISCONNECTED)
{
    _connectorChannel.reset(NULL);
    LOG_INFO("Connector object constructed! serverAddr: [%s]", this->_serverAddr.GetAddrAndPort().c_str());
    // set a default callback for connected
    this->_successCallback = bind(&Connector::defaultConnectToServerSuccessCallback, this, std::placeholders::_1);
}

Connector::~Connector()
{
    LOG_INFO("Connector object destructed!");
}

void Connector::defaultConnectToServerSuccessCallback(int sockFd)
{
    LOG_INFO("sock: [%d] connected to server [%s]!", sockFd, this->_serverAddr.GetAddrAndPort().c_str());
}


void Connector::start()
{
    this->_loop->QueueInLoop(bind(&Connector::startInLoop, this));
}

void Connector::startInLoop()
{
    this->_loop->AssertInEventLoopThread();

    // run connect process
    connect();
}

void Connector::connect()
{
    int ret = 0;
    int sockFd = 0;

    sockFd = Socket::CreateNonBlockingSocket();
    LOG_DEBUG("connector sockFd: %d", sockFd);

    sockaddr addr = *(sockaddr*)this->_serverAddr.GetSockAddr();
    socklen_t addrLen = sizeof(addr);
    ret = ::connect(sockFd, &addr, addrLen);    // call ::connect
    LOG_DEBUG("::connect() ret: %d", ret);

    int saveErrno = ret < 0 ? errno : 0;
    // printf("%d", errno);

    switch (saveErrno) {
        // this cases are okay, when the sockFd is ready to write, we connected to server successfully
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            connecting(sockFd);
            break;
        case EAGAIN:
        case EADDRINUSE:
            retry();
            break;
        default:
            perror("connect");
            LOG_SYSE("call ::connect failed!");
            close(sockFd);

            if (_failedCallback) {
                _failedCallback();
            }
            break;
    }
}

void Connector::connecting(int sockFd)
{
    setStatus(CONNECTING);

    // create a cheannl and regist to reactor
    this->_connectorChannel.reset(new Channel(this->_loop, sockFd));
    this->_connectorChannel->SetWriteCallback(bind(&Connector::socketIsReadyToWrite, this, sockFd));
    this->_connectorChannel->SetErrorCallback(bind(_failedCallback));
    this->_connectorChannel->EnableWrite(); // regist write event to reactor
}

void Connector::socketIsReadyToWrite(int sockFd)
{
    setStatus(CONNECTED);
    // remove channel after connected, avoid busy loop
    this->_connectorChannel->DisableAll();
    this->removeChannel();

    // call callback when connectedToServer
    if (_successCallback) {
        _successCallback(sockFd);
    }
}

// TODO: finish retry
void Connector::retry()
{
    LOG_INFO("Connector retry begin");
    LOG_INFO("Connector retry end");
}

void Connector::setStatus(int stat)
{
    this->_status = stat;
}

int Connector::getStatus()
{
    return _status;
}

void Connector::removeChannel()
{
    assert(_connectorChannel.get() != NULL);
    this->_connectorChannel->Remove();
    this->_connectorChannel.reset();
}

void Connector::setConnectToServerSuccessCallback(const ConnectToServerSuccessCallback& cb)
{
    this->_successCallback = cb;
}

void Connector::setConnectToServerFailedCallback(const ConnectToServerFailedCallback& cb)
{
    this->_failedCallback = cb;
}
