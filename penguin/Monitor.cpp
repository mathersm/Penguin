#include "Monitor.h"
#include <cassert>


namespace Penguin
{
    Monitor::Monitor(void)
    {
    }


    Monitor::~Monitor(void)
    {
    }


    void
    Monitor::notify_one(void) noexcept
    {
        this->condition_variable_.notify_one();
    }


    void
    Monitor::notify_all(void) noexcept
    {
        this->condition_variable_.notify_all();
    }


    void
    Monitor::wait(_guard_type& guard)
    {
        assert(guard.owns_lock());
        this->condition_variable_.wait(guard);
    }


    template <class Rep, class Period>
    std::cv_status
    Monitor::wait_for(_guard_type& guard, const std::chrono::duration<Rep, Period>& rel_time)
    {
        assert(guard.owns_lock());
        return this->condition_variable_.wait_for(guard, rel_time);
    }


    template <class Clock, class Duration>
    std::cv_status
    Monitor::wait_until(_guard_type& guard, const std::chrono::time_point<Clock, Duration>& timeout_time)
    {
        assert(guard.owns_lock());
        return this->condition_variable_.wait_until(guard, timeout_time);
    }


    Monitor::operator _mutex_type &() const
    {
        return this->mutex_;
    }

}