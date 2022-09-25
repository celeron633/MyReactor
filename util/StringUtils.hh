#ifndef __STRING_UTILS_HH__
#define __STRING_UTILS_HH__

#include <string>
#include <cstring>
#include <vector>

using std::vector;
using std::string;

namespace util {
    void splitString(string src, vector<string>& strVec, string delimiter = "|");
}

#endif