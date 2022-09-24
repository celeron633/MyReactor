#include "Log.hh"

int main(int argc, char* argv[])
{
    LOG_INFO("%d", 1);
    LOG_DEBUG("%d", 2);
    LOG_SYSE("%s", "hello");
    LOG_ERROR("%s:%d:%f", "123", 4, 5.6);

    return 0;
}