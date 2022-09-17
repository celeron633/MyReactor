#include "TcpServer.hh"

using namespace base;
using namespace net;
using namespace std;

TcpServer::TcpServer(EventLoop* loop, INetAddr listenAddr, string serverName) : _serverName(serverName), _listenAddr(listenAddr), \
    _acceptor(loop, listenAddr)
{

}

TcpServer::~TcpServer()
{
    
}

void TcpServer::start()
{
    LOG_INFO("TcpServer::start begin");
    this->_acceptor.setNewConnectionCallback(bind(&TcpServer::HandleNewConnection, this, placeholders::_1, placeholders::_2));
    this->_acceptor.start();
    LOG_INFO("TcpServer::start end");
}

void TcpServer::stop()
{
    LOG_WARN("TcpServer::stop() begin");
}

void TcpServer::HandleNewConnection(int sockFd, INetAddr clientAddr)
{
    LOG_INFO("HandleNewConnection begin! sockFd: [%d], client info: [%s]", sockFd, clientAddr.GetAddrAndPort().c_str());

    LOG_INFO("HandleNewConnection end");
    return;
}