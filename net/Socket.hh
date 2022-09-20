#ifndef __SOCKET_HH__
#define __SOCKET_HH__

#include "Log.hh"
#include "NonCopyable.hh"
#include "INetAddr.hh"

#ifdef __linux__ 
    typedef int SOCKET;
#else
    typedef int SOCKET;
#endif

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
        Close();
    }

    SOCKET fd()
    {
        return _sockFd;
    }

    int Bind(INetAddr& listenAddr);
    int Listen();
    void ReuseAddr();
    int Accept(INetAddr& peerAddr);
    int Close();
    int ShutdownWrite();

    void SetNoDelay();      // TCP no-delay
    void SetReusePort();    // TCP reuse-port
    void SetReuseAddr();    // TCP reuse-addr

    // public static functions
    static SOCKET CreateNonBlockingSocket();
    static void SetSocketNonBlocking(SOCKET fd);
    static void SetSocketReuseAddr(SOCKET fd, bool on = true);
private:
    const SOCKET _sockFd;
};  // end class 'Socket'

};  // end namespace

#endif