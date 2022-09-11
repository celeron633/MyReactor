#include <unistd.h>
#include <fcntl.h>

#include "Socket.hh"

using namespace std;
using namespace base;
using namespace net;

#ifndef BACKLOG_SIZE
#define BACKLOG_SIZE 1024
#endif

// namespace net class Socket
int Socket::Listen()
{
    if (listen(this->_sockFd, BACKLOG_SIZE) < 0) {
        perror("listen");
        LOG_FATAL("listen");
    }
    return 0;
}

int Socket::Accept(INetAddr& peerAddr)
{
    sockaddr sAddr;
    socklen_t sAddrLen = sizeof(sAddr);
    int fd = 0; // peer socket fd

    fd = accept(_sockFd, &sAddr, &sAddrLen);
    if (fd < 0) {
        perror("accept");
        LOG_ERROR("accept failed!");
    }
    peerAddr.SetSockAddr(sAddr);
    return fd;
}


// namespace sockets
SOCKET Socket::CreateNonBlockingSocket()
{
    int flags = 0;
    int ret = 0;
    SOCKET fd = 0;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        LOG_FATAL("call socket failed!");
    }
    flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl");
        LOG_FATAL("call fcntl GETFL failed!");
    }
    flags |= O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    if (ret < 0) {
        perror("fcntl");
        LOG_FATAL("call fcntl SETFL failed!");
    }
    return fd;
}

void Socket::SetSocketNonBlocking(SOCKET fd)
{
    int flags = 0;
    int ret = 0;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl");
        LOG_FATAL("call fcntl GETFL failed!");
    }
    flags |= O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    if (ret < 0) {
        perror("fcntl");
        LOG_FATAL("call fcntl SETFL failed!");
    }
}