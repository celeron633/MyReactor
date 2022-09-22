#ifndef __HEX_UTILS_HH__
#define __HEX_UTILS_HH__

#include <sys/types.h>
#include <string.h>

namespace util {

void hexdump(const char* addr, size_t len);

};

#endif