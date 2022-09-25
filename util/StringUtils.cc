#include "StringUtils.hh"
#include "HexUtils.hh"

#include <iostream>

void util::splitString(string src, vector<string>& strVec, string delimiter)
{
    string subStr = "";
    size_t pos = 0;
    int delimiterLen = delimiter.length();

    if (src.empty()) {
        return;
    }

    if (src.find(delimiter) == src.npos) {
        return;
    }

    pos = src.find(delimiter);
    while (pos != src.npos) {
        subStr = src.substr(0, pos);
        strVec.push_back(subStr);   // copy here
        src = src.substr(pos + delimiterLen, src.npos);
        pos = src.find(delimiter);
    }
    strVec.push_back(src);
    /*
    if (!src.empty()) {
        strVec.push_back(src);
    }
    */
}

void util::trimCRLF(string& str)
{
    string tmpStr = str;

    auto pos = tmpStr.find("\r\n");
    if (pos != tmpStr.npos) {
        tmpStr = tmpStr.substr(0, pos);
    }
    str = tmpStr;
}
