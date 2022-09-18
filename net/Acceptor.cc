#include "Acceptor.hh"

using namespace std;
using namespace base;
using namespace net;

Acceptor::Acceptor(EventLoop* loop, INetAddr listenAddr) :  _eventLoop(loop), _listenAddr(listenAddr), _listenSocket(Socket::CreateNonBlockingSocket()), \
    _channel(loop, _listenSocket.fd())
{
    // 设置channel可读回调函数
    this->_channel.SetReadCallback(bind(&Acceptor::handleRead, this, placeholders::_1));
}

Acceptor::~Acceptor()
{

}

void Acceptor::bindAddr(INetAddr& addr)
{
    this->_listenSocket.Bind(addr);
}

void Acceptor::listen()
{
    this->_listenSocket.Listen();
}

void Acceptor::start()
{
    LOG_INFO("listenAddr: %s", this->_listenAddr.GetAddrAndPort().c_str());
    // bind listen addr
    this->bindAddr(_listenAddr);
    // listen to specificed sockaddr
    this->_listenSocket.ReuseAddr();
    listen();
    // regist to epoll for reading event
    this->_channel.EnableRead();
}

// listen的fd可读, 有客户端接上来了, 需要accept
void Acceptor::handleRead(Timestamp evTime)
{
    this->_eventLoop->AssertInEventLoopThread();  // make sure this happens in loop thread

    LOG_INFO("Acceptor::handleRead begin, event timestamp: [%s]", evTime.ConvertToString().c_str());
    int clientConnFd = 0;
    sockaddr sAddr;
    socklen_t sLen = sizeof(sAddr);

    clientConnFd = ::accept(this->_listenSocket.fd(), &sAddr, &sLen);
    if (clientConnFd < 0) {
        perror("accept");
        LOG_ERROR("Acceptor accept failed!");
        return;
    }

    INetAddr inetAddr;
    inetAddr.SetSockAddr(sAddr);
    LOG_INFO("accept OK! addr: [%s]", inetAddr.GetAddrAndPort().c_str());

    if (_newConnectionCallback) {
        _newConnectionCallback(clientConnFd, inetAddr);
    }

    LOG_INFO("Acceptor::handleRead end");
}