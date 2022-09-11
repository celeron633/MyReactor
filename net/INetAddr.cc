#include "INetAddr.hh"
#include "Log.hh"

using namespace net;
using namespace base;
using namespace std;

INetAddr::INetAddr(string addr, int port)
{
    memset(&_sAddr, 0, sizeof(_sAddr));
    _sAddr.sin_family = AF_INET;
    _sAddr.sin_port = htons((uint16_t)port);
    _sAddr.sin_addr.s_addr = inet_addr(addr.c_str());
}

string INetAddr::GetAddr()
{
    char buf[32] = {0};
    inet_ntop(AF_INET, &_sAddr.sin_addr, buf, sizeof(buf));
    
    return string(buf);
}

string INetAddr::GetAddrAndPort()
{
    char addr[32] = {0};
    char buf[32] = {0};

    inet_ntop(AF_INET, &_sAddr.sin_addr, addr, sizeof(addr));
    sprintf(buf, "%s:%d", addr, ntohs(_sAddr.sin_port));

    return string(buf);
}

sockaddr_in * INetAddr::GetSockAddr()
{
    return &_sAddr;
}

void INetAddr::SetSockAddr(sockaddr& addr)
{
    this->_sAddr = *(sockaddr_in*)&addr;
}

uint16_t INetAddr::GetPort()
{
    return ntohs(_sAddr.sin_port);
}

/* unit test
int main(int argc, char *argv[])
{
    INetAddr addr("127.0.0.1", 8088);
    printf("%s\n", addr.GetAddr().c_str());
    printf("%d\n", addr.GetPort());
    printf("%s\n", addr.GetAddrAndPort().c_str());
    return 0;
} */