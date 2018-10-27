/*
* Copyright (c) 2017 Michael Mathers
*/
#include "Monitor.h"


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


    Monitor::operator _mutex_type &() const
    {
        return this->mutex_;
    }

}
