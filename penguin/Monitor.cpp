/*
* Copyright (c) 2017 Michael Mathers
*/
#include "Monitor.h"


namespace Penguin
{
    Monitor::Monitor(void)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);
    }


    Monitor::~Monitor(void)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);
    }


    void
    Monitor::notify_one(void) noexcept
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        this->condition_variable_.notify_one();
    }


    void
    Monitor::notify_all(void) noexcept
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        this->condition_variable_.notify_all();
    }


    void
    Monitor::wait(_guard_type& guard)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        assert(guard.owns_lock());
        this->condition_variable_.wait(guard);
    }


    Monitor::operator _mutex_type &() const
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        return this->mutex_;
    }

}