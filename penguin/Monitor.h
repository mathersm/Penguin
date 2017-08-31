/*
 * Copyright (c) 2017 Michael Mathers
 */
#ifndef PENGUIN_MONITOR_H
#define PENGUIN_MONITOR_H


#include "Penguin_export.h"
#include "Profiler.h"
#include <cassert>
#include <mutex>
#include <condition_variable>


namespace Penguin
{
    class Penguin_Export Monitor
    {
    public:
        typedef std::mutex                      _mutex_type;
        typedef std::condition_variable         _condition_variable_type;
        typedef std::unique_lock<_mutex_type>   _guard_type;

        Monitor(void);
        virtual ~Monitor(void);

        void notify_one(void) noexcept;
        void notify_all(void) noexcept;

        void wait(_guard_type& guard);

        template <class Predicate>
        void wait(_guard_type& guard, Predicate predicate);

        template <class Rep, class Period>
        std::cv_status wait_for(_guard_type& guard, const std::chrono::duration<Rep, Period>& rel_time);

        template <class Rep, class Period, class Predicate>
        bool wait_for(_guard_type& guard, const std::chrono::duration<Rep, Period>& rel_time, Predicate predicate);

        template <class Clock, class Duration>
        std::cv_status wait_until(_guard_type& guard, const std::chrono::time_point<Clock, Duration>& timeout_time);

        template <class Clock, class Duration, class Predicate>
        bool wait_until(_guard_type& guard, const std::chrono::time_point<Clock, Duration>& timeout_time, Predicate predicate);

        operator _mutex_type& () const;

    protected:

    private:
        mutable _mutex_type         mutex_;
        _condition_variable_type    condition_variable_;

        Monitor(const Monitor& other) = delete;
        Monitor& operator = (const Monitor& other) = delete;

        Monitor(Monitor&& other) = delete;
        Monitor& operator = (Monitor&& other) = delete;
    };


    template <class Predicate>
    void
    Monitor::wait(_guard_type& guard, Predicate predicate)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        assert(guard.owns_lock());
        this->condition_variable_.wait(guard, predicate);
    }


    template <class Rep, class Period>
    std::cv_status
    Monitor::wait_for(_guard_type& guard, const std::chrono::duration<Rep, Period>& rel_time)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        assert(guard.owns_lock());
        return this->condition_variable_.wait_for(guard, rel_time);
    }


    template <class Rep, class Period, class Predicate>
    bool
    Monitor::wait_for(_guard_type& guard, const std::chrono::duration<Rep, Period>& rel_time, Predicate predicate)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        assert(guard.owns_lock());
        return this->condition_variable_.wait_for(guard, rel_time, predicate);
    }


    template <class Clock, class Duration>
    std::cv_status
    Monitor::wait_until(_guard_type& guard, const std::chrono::time_point<Clock, Duration>& timeout_time)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        assert(guard.owns_lock());
        return this->condition_variable_.wait_until(guard, timeout_time);
    }


    template <class Clock, class Duration, class Predicate>
    bool
    Monitor::wait_until(_guard_type& guard, const std::chrono::time_point<Clock, Duration>& timeout_time, Predicate predicate)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        assert(guard.owns_lock());
        return this->condition_variable_.wait_until(guard, timeout_time, predicate);
    }
}


#endif // PENGUIN_MONITOR_H
