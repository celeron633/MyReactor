#include "NonCopyable.hh"

#include <iostream>

using namespace base;
using namespace std;

/* unit test
class MyClass : NonCopyable {
public:
    MyClass()
    {
        _i = 0;
    }

    MyClass(int i)
    {
        _i = i;
    }
private:
    int _i;
};

int main(int argc, char *argv[])
{
    MyClass m(1);
    MyClass m2(m);
    return 0;
}
*/