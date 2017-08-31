/*
 * Copyright (c) 2017 Michael Mathers
 */
#ifndef PENGUIN_SEMAPHORE_H
#define PENGUIN_SEMAPHORE_H


#include "Penguin_export.h"
#include "Monitor.h"
#include <atomic>


namespace Penguin
{
    class Penguin_Export Semaphore
    {
    public:
        Semaphore(long permits = 0);
        virtual ~Semaphore(void);

        void acquire(void);
        void release(void);

        template <class Rep, class Period>
        std::cv_status try_acquire_for(const std::chrono::duration<Rep, Period>& rel_time);

        template <class Clock, class Duration>
        std::cv_status try_acquire_until(const std::chrono::time_point<Clock, Duration>& timeout_time);

        long permits(void) const;
        long waiters(void) const;

    protected:

    private:
        std::atomic<long> permits_;
        std::atomic<long> waiters_;

        Penguin::Monitor permit_monitor_;

        Semaphore(const Semaphore& other) = delete;
        Semaphore& operator = (const Semaphore& other) = delete;

        Semaphore(Semaphore&& other) = delete;
        Semaphore& operator = (Semaphore&& other) = delete;
    };


    template <class Rep, class Period>
    std::cv_status
    Semaphore::try_acquire_for(const std::chrono::duration<Rep, Period>& rel_time)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        Penguin::Monitor::_guard_type permit_guard(this->permit_monitor_);
        this->waiters_.fetch_add(1);
        if (this->permits_.load() > 0)
        {
            // We don't need to wait, permits are available
        }
        else
        {
            if (false == this->permit_monitor_.wait_for(permit_guard, rel_time, [this] {return this->permits_.load() > 0; }))
            {
                this->waiters_.fetch_sub(1);
                return std::cv_status::timeout;
            }
        }
        assert(this->permits_.load() > 0);
        this->permits_.fetch_sub(1);
        this->waiters_.fetch_sub(1);
        return std::cv_status::no_timeout;
    }


    template <class Clock, class Duration>
    std::cv_status
    Semaphore::try_acquire_until(const std::chrono::time_point<Clock, Duration>& timeout_time)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        Penguin::Monitor::_guard_type permit_guard(this->permit_monitor_);
        this->waiters_.fetch_add(1);
        if (this->permits_.load() > 0)
        {
            // We don't need to wait, permits are available
        }
        else
        {
            if (false == this->permit_monitor_.wait_until(permit_guard, timeout_time, [this] {return this->permits_.load() > 0; }))
            {
                this->waiters_.fetch_sub(1);
                return std::cv_status::timeout;
            }
        }
        assert(this->permits_.load() > 0);
        this->permits_.fetch_sub(1);
        this->waiters_.fetch_sub(1);
        return std::cv_status::no_timeout;
    }
}


#endif // PENGUIN_SEMAPHORE_H
