/*
 * Copyright (c) 2019 Michael Mathers
 */
#ifndef PENGUIN_TIMER_H
#define PENGUIN_TIMER_H


#include <atomic>
#include <chrono>


namespace Penguin
{
    template <class Rep, class Period>
    class Timer
    {
    public:
        using _clock_type = std::chrono::high_resolution_clock;
        using _representation_type = Rep;
        using _period_type = Period;
        using _duration_type = std::chrono::duration<Rep, Period>;
        using _time_point_type = std::chrono::time_point<_clock_type>;
        using _timer_type = Timer<Rep, Period>;

    public:
        Timer(void);
        ~Timer(void);

    public:
        _representation_type    get_current_duration(void) const;
        _time_point_type        get_current_time(void) const;        
        _representation_type    get_finish_duration(void) const;
        _time_point_type        get_finish_time(void) const;
        _time_point_type        get_start_time(void) const;

        void reset(void);
        void start(void);
        void stop(void);

    private:
        Timer(const _timer_type&) = delete;
        Timer(_timer_type&&) = delete;

    private:
        _timer_type& operator = (const _timer_type&) = delete;
        _timer_type& operator = (_timer_type&&) = delete;

    private:
        std::atomic<bool>   finished_;
        _time_point_type    start_time_;
        _time_point_type    finish_time_;
    };


    template <class Rep, class Period>
    Timer<Rep, Period>::Timer(void)
        : finished_(false)
        , start_time_(_clock_type::now())
        , finish_time_(_clock_type::now())
    {
    }


    template <class Rep, class Period>
    Timer<Rep, Period>::~Timer(void)
    {
        this->finish_time_ = _clock_type::now();
        this->finished_.store(true);
    }


    template <class Rep, class Period>
    typename Timer<Rep, Period>::_representation_type
    Timer<Rep, Period>::get_current_duration(void) const
    {
        return std::chrono::duration_cast<_duration_type>(_clock_type::now() - this->start_time_).count();
    }


    template <class Rep, class Period>
    typename Timer<Rep, Period>::_time_point_type
        Timer<Rep, Period>::get_current_time(void) const
    {
        return _clock_type::now();
    }


    template <class Rep, class Period>
    typename Timer<Rep, Period>::_representation_type
    Timer<Rep, Period>::get_finish_duration(void) const
    {
        if (this->finished_.load())
        {
            return std::chrono::duration_cast<_duration_type>(this->finish_time_ - this->start_time_).count();
        }
        else
        {
            throw std::runtime_error("Timer was not stopped!");
        }
    }


    template <class Rep, class Period>
    typename Timer<Rep, Period>::_time_point_type
    Timer<Rep, Period>::get_finish_time(void) const
    {
        if (this->finished_.load())
        {
            return this->finish_time_;
        }
        else
        {
            throw std::runtime_error("Timer was not stopped!");
        }
    }


    template <class Rep, class Period>
    typename Timer<Rep, Period>::_time_point_type
    Timer<Rep, Period>::get_start_time(void) const
    {
        return this->start_time_;
    }


    template <class Rep, class Period>
    void
    Timer<Rep, Period>::reset(void)
    {
        this->start();
    }

    
    template <class Rep, class Period>
    void
    Timer<Rep, Period>::start(void)
    {
        this->start_time_ = _clock_type::now();
        this->finished_.store(false);
    }


    template <class Rep, class Period>
    void
    Timer<Rep, Period>::stop(void)
    {
        this->finish_time_ = _clock_type::now();
        this->finished_.store(true);
    }
}


#endif // PENGUIN_SCOPED_TIMER_H

