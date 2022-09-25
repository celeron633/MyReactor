#include "StringUtils.hh"
#include "HexUtils.hh"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    string str = "1|2|3|4|567|8|";
    vector<string> strVec;

    util::splitString(str, strVec, "|");
    cout << strVec.size() << endl;

    for (auto& s : strVec) {
        cout << s.c_str() << endl;
    }

    return 0;
}