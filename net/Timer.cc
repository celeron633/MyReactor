#include "Timer.hh"

using namespace net;
using namespace base;
using namespace std;

// seq generator for all timers
atomic_int64_t Timer::_timerSeq(0);

Timer::Timer(TimerCallback cb, Timestamp when, unsigned int interval, int repeatCount) : _callback(cb), \
    _expiration(when), _interval(interval), _repeatCount(repeatCount), _canceled(false), \
    _sequence(Timer::_timerSeq++)
{
    LOG_DEBUG("Timer seq: [%ld]", this->_sequence);
}

Timer::~Timer()
{

}

void Timer::cancel()
{
    this->_canceled = true;
}

bool Timer::canceled()
{
    return _canceled;
}

Timestamp Timer::getExpiration(void)
{
    return _expiration;
}

int Timer::getRepeatCount()
{
    return _repeatCount;
}

int64_t Timer::getSequence()
{
    return this->_sequence;
}

void Timer::run()
{
    if (_canceled) {
        return;
    }

    if (_callback) {
        _callback();
    }

    if (this->_repeatCount != -1 && this->_repeatCount > 0) {
        --this->_repeatCount;
    }

    if (_repeatCount == 0) {
        cancel();
    }
    
    this->_expiration += _interval; // add Timestamp _expiration to next expire
}

