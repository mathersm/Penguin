/*
* Copyright (c) 2017 Michael Mathers
*/
#include "Semaphore.h"

namespace Penguin
{
    Semaphore::Semaphore(long permits)
        : permits_(permits)
        , waiters_(0)
    {
    }


    Semaphore::~Semaphore(void)
    {
    }


    void
    Semaphore::acquire(void)
    {
        Penguin::Monitor::_guard_type permit_guard(this->permit_monitor_);
        this->waiters_.fetch_add(1);
        if (this->permits_.load() > 0)
        {
            // We don't need to wait, permits are available
        }
        else
        {
            this->permit_monitor_.wait(permit_guard, [this] {return this->permits_.load() > 0; });
        }
        assert(this->permits_.load() > 0);
        this->permits_.fetch_sub(1);
        this->waiters_.fetch_sub(1);
    }


    void
    Semaphore::release(void)
    {
        Penguin::Monitor::_guard_type permit_guard(this->permit_monitor_);
        this->permits_.fetch_add(1);
        this->permit_monitor_.notify_one();
    }


    long
    Semaphore::permits(void) const
    {
        return this->permits_.load();
    }


    long
    Semaphore::waiters(void) const
    {
        return this->waiters_.load();
    }
}
