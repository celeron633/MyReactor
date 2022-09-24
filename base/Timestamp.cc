#include "Timestamp.hh"

#include <iostream>

using namespace base;
using namespace std;

Timestamp::Timestamp()
{
    gettimeofday(&_timeval, NULL);
}

Timestamp::~Timestamp()
{

}

void Timestamp::SetToNow()
{
    gettimeofday(&_timeval, NULL);
}

string Timestamp::ConvertToString()
{
    string timeStr;
    char buf[128] = {0};

    // gettimeofday(&_timeval, NULL);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S.", localtime(&(_timeval.tv_sec)));
    timeStr.append(buf);
    // add ms
    sprintf(buf, "%03ld", _timeval.tv_usec / 1000);
    timeStr.append(buf);

    return timeStr;
}

string Timestamp::GetCurrentTimestamp()
{
    string timeStr;
    char buf[128] = {0};
    timeval tv;

    gettimeofday(&tv, NULL);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S.", localtime(&(tv.tv_sec)));
    timeStr.append(buf);
    sprintf(buf, "%03ld", tv.tv_usec / 1000);
    timeStr.append(buf);

    return timeStr;
}

uint64_t Timestamp::GetMilliseconds() const
{
    return this->_timeval.tv_sec * (uint64_t)1000 + (this->_timeval.tv_usec / (uint64_t)1000);
}

void Timestamp::AddMilliseconds(uint64_t i)
{
    // FIXME: in-accurate
    uint32_t sec = i / 1000;
    uint32_t uSec = (i % 1000) * 1000;

    this->_timeval.tv_sec += sec;
    if ((uSec + _timeval.tv_usec) < 1000 * 1000) {
        _timeval.tv_usec = uSec + _timeval.tv_usec;
    } else {
        this->_timeval.tv_sec += 1;
        _timeval.tv_usec = (uSec + _timeval.tv_usec) - (1000 * 1000);
    }
}

Timestamp& Timestamp::operator+=(int milliseconds)
{
    AddMilliseconds(milliseconds);
    return *this;
}

bool Timestamp::operator<(const Timestamp& r) {
    return this->GetMilliseconds() < r.GetMilliseconds();
}

bool Timestamp::operator>=(const Timestamp& r) {
    return !(this->GetMilliseconds() < r.GetMilliseconds());
}

bool Timestamp::operator==(const Timestamp& r) {
    return this->GetMilliseconds() == r.GetMilliseconds();
}

/* unit test
// moved to test dir
int main(int argc, char *argv[])
{
    Timestamp t;
    t.SetToNow();
    cout << t.ConvertToString().c_str() << endl;

    return 0;
}
*/