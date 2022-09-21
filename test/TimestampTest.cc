#include "Timestamp.hh"

#include <iostream>

using namespace base;
using namespace std;

int main(int argc, char* argv[])
{
    Timestamp ts;
    Timestamp ts2;
    cout << ts.ConvertToString().c_str() << endl;
    ts.AddMilliseconds(15123123123);
    cout << ts.ConvertToString().c_str() << endl;

    cout << (ts < ts2) << endl;
    cout << (ts2 < ts) << endl;
    cout << (ts >= ts2) << endl;
    cout << (ts2 >= ts) << endl;


    return 0;
}