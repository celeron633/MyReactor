#include "Timer.hh"

using namespace net;
using namespace base;
using namespace std;

// seq generator for all timers
atomic_int64_t Timer::_timerSeq(0);

Timer::Timer(TimerCallback cb, Timestamp when, int interval, int repeatCount = -1) : _callback(cb), _expiration(when), \
    _interval(interval), _repeatCount(repeatCount), _sequence(Timer::_timerSeq++)
{
    LOG_DEBUG("Timer seq: [%lld]", this->_sequence);
}

Timer::~Timer()
{

}

void Timer::run()
{
    if (_canceled) {
        return;
    }

    // run callback for Timer
    if (_callback) {
        _callback();
    }

    if (this->_repeatCount != -1 && this->_repeatCount > 0) {
        --this->_repeatCount;
    }
    
    // this->_expiration += _interval;
}

