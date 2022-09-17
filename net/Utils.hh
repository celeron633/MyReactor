#ifndef __UTILS_HH__
#define __UTILS_HH__

#include <sys/types.h>
#include <string.h>

namespace utils {

void hexdump(const char* addr, size_t len);

};

#endif