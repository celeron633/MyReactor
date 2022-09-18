#ifndef __ACCEPTOR_HH__
#define __ACCEPTOR_HH__

#include "INetAddr.hh"
#include "Callbacks.hh"
#include "Socket.hh"
#include "EventLoop.hh"
#include "Channel.hh"

namespace net {

class Acceptor : public NonCopyable {
public:
    Acceptor(EventLoop* loop, INetAddr listenAddr);
    ~Acceptor();

    void start();
    void setNewConnectionCallback(const NewConnectionCallback& cb)
    {
        _newConnectionCallback = cb;
    }
private:
    void handleRead(Timestamp evTime);
    void bindAddr(INetAddr& addr);
    void listen();
private:
    EventLoop* _eventLoop;
    INetAddr _listenAddr;
    Socket _listenSocket;
    Channel _channel;
    NewConnectionCallback _newConnectionCallback;
};

};

#endif