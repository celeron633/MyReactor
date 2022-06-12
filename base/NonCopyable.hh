#ifndef __NON_COPYABLE_HH__
#define __NON_COPYABLE_HH__

namespace base {

class NonCopyable {
public:
    NonCopyable() = default;
    ~NonCopyable() = default;
private:
    // disallow copy construct
    NonCopyable(const NonCopyable &r) = delete;
    // disallow '=' operator assign
    NonCopyable & operator=(const NonCopyable &r) = delete;
};

}

#endif
