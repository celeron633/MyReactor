#include "StringUtils.hh"
#include "HexUtils.hh"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    /* string str = "1|2|3|4|567|8|";
    vector<string> strVec;

    util::splitString(str, strVec, "|");
    cout << strVec.size() << endl;

    for (auto& s : strVec) {
        cout << s.c_str() << endl;
    } */

    vector<string> strVec;

    string str = "hello world\r\nbfg-9000\r\nak-47\r\n\r\n";
    cout << str.length() << endl;
    cout << str.size() << endl;

    // util::trimCRLF(str);
    util::splitString(str, strVec, "\r\n");

    //
    for (auto it : strVec) {
        util::hexdump(it.data(), it.length());
    }

    return 0;
}