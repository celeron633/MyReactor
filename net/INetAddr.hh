#ifndef __INET_ADDR_HH__
#define __INET_ADDR_HH__

#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#include <arpa/inet.h>
#include <unistd.h>

using std::string;

namespace net {

class INetAddr {
public:
    INetAddr(string addr = "0.0.0.0", int port = 0);

    string GetAddr();
    string GetAddrAndPort();

    sockaddr_in *GetSockAddr();
    void SetSockAddr(sockaddr &addr);

    uint16_t GetPort();
private:
    sockaddr_in _sAddr;
};

}

#endif