#include "Timestamp.hh"

#include <iostream>

using namespace base;
using namespace std;

int main(int argc, char* argv[])
{
    Timestamp ts;
    cout << ts.ConvertToString().c_str() << endl;
    ts.AddMilliseconds(15123123123);
    cout << ts.ConvertToString().c_str() << endl;

    return 0;
}