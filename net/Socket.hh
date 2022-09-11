#ifndef __SOCKET_HH__
#define __SOCKET_HH__

#include "Log.hh"
#include "NonCopyable.hh"
#include "INetAddr.hh"

typedef int SOCKET;

namespace net {
// non-copyable
class Socket : public base::NonCopyable {
public:
    Socket() : _sockFd(-1)
    {
        
    }

    explicit Socket(int sockFd) : _sockFd(sockFd)
    {

    }

    ~Socket()
    {

    }

    SOCKET fd()
    {
        return _sockFd;
    }

    int Listen();
    int Accept(INetAddr& peerAddr);

    void SetNoDelay();      // TCP no-delay
    void SetReusePort();    // TCP reuse-port
    void SetReuseAddr();    // TCP reuse-addr

    static SOCKET CreateNonBlockingSocket();
    static void SetSocketNonBlocking(SOCKET fd);
private:
    const SOCKET _sockFd;
};  // end class 'Socket'

};  // end namespace

#endif