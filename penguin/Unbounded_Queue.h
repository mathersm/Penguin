/*
 * Copyright (c) 2017 Michael Mathers
 */
#ifndef PENGUIN_UNBOUNDED_QUEUE_H
#define PENGUIN_UNBOUNDED_QUEUE_H


#include "Exception.h"
#include "Semaphore.h"
#include <list>


namespace Penguin
{
    template <typename T>
    class Unbounded_Queue
    {
    public:
        Unbounded_Queue(void);
        virtual ~Unbounded_Queue(void);

        size_t size(void) const;

        void push(const T& value);
        void push(T&& value);

        T pop(void);

        template <class Rep, class Period>
        T try_pop_for(const std::chrono::duration<Rep, Period>& rel_time);

        template <class Clock, class Duration>
        T try_pop_until(const std::chrono::time_point<Clock, Duration>& timeout_time);

    protected:

    private:
        Penguin::Semaphore  itemCount_;
        std::list<T>        queue_;

        Unbounded_Queue(const Unbounded_Queue& other) = delete;
        Unbounded_Queue& operator = (const Unbounded_Queue& other) = delete;

        Unbounded_Queue(Unbounded_Queue&& other) = delete;
        Unbounded_Queue& operator = (Unbounded_Queue&& other) = delete;
    };


    template <typename T>
    Unbounded_Queue<T>::Unbounded_Queue(void)
        : itemCount_(0)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);
    }


    template <typename T>
    Unbounded_Queue<T>::~Unbounded_Queue(void)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);
    }


    template <typename T>
    size_t
    Unbounded_Queue<T>::size(void) const
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        assert(itemCount_.permits() == queue_.size());
        return itemCount_.permits();
    }


    template <typename T>
    void
    Unbounded_Queue<T>::push(const T& value)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        this->queue_.push_back(value);
        this->itemCount_.release();
    }


    template <typename T>
    void
    Unbounded_Queue<T>::push(T&& value)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        this->queue_.push_back(value);
        this->itemCount_.release();
    }


    template <typename T>
    T
    Unbounded_Queue<T>::pop(void)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        this->itemCount_.acquire();
        T value = this->queue_.front();
        this->queue_.pop_front();
        return value;
    }


    template <typename T>
    template <class Rep, class Period>
    T
    Unbounded_Queue<T>::try_pop_for(const std::chrono::duration<Rep, Period>& rel_time)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        if (std::cv_status::no_timeout == this->itemCount_.try_acquire_for(rel_time))
        {
            T value = this->queue_.front();
            this->queue_.pop_front();
            return value;
        }
        throw Penguin::Timeout_Exception("Attempt to acquire Semaphore for Unbounded_Queue timed out");
    }


    template <typename T>
    template <class Clock, class Duration>
    T
    Unbounded_Queue<T>::try_pop_until(const std::chrono::time_point<Clock, Duration>& timeout_time)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);

        if (std::cv_status::no_timeout == this->itemCount_.try_acquire_until(timeout_time))
        {
            T value = this->queue_.front();
            this->queue_.pop_front();
            return value;
        }
        throw Penguin::Timeout_Exception("Attempt to acquire Semaphore for Unbounded_Queue timed out");
    }
}


#endif // PENGUIN_UNBOUNDED_QUEUE_H
