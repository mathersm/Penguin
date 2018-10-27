/*
* Copyright (c) 2018 Michael Mathers
*/
#ifndef PENGUIN_SCOPED_TIMER_H
#define PENGUIN_SCOPED_TIMER_H


#include <atomic>
#include <chrono>
#include <functional>
#include <optional>


namespace Penguin
{
    template <class Rep, class Period>
    class Scoped_Timer
    {
    public:
        using _clock_type = std::chrono::high_resolution_clock;
        using _duration_type = std::chrono::duration<Rep, Period>;
        using _time_point_type = std::chrono::time_point<_clock_type>;
        using _timer_type = Scoped_Timer<Rep, Period>;

    public:
        Scoped_Timer(std::function<void(const _timer_type&)> on_finish);
        ~Scoped_Timer(void);

    public:
        std::optional<_duration_type>   get_finish_duration(void) const;
        std::optional<_time_point_type> get_finish_time(void) const;
        _duration_type                  get_intermediate_duration(void) const;
        _time_point_type                get_intermediate_time(void) const;
        _time_point_type                get_start_time(void) const;

    private:
        Scoped_Timer(const _timer_type&) = delete;
        Scoped_Timer(_timer_type&&) = delete;

    private:
        _timer_type& operator = (const _timer_type&) = delete;
        _timer_type& operator = (_timer_type&&) = delete;

    private:
        std::atomic<bool>                       finished_;
        _time_point_type                        start_time_;
        _time_point_type                        finish_time_;
        std::function<void(const _timer_type&)> on_finish_;
    };


    template <class Rep, class Period>
    Scoped_Timer<Rep, Period>::Scoped_Timer(std::function<void(const _timer_type&)> on_finish)
        : finished_(false)
        , start_time_(_clock_type::now())
        , on_finish_(on_finish)
    {
    }


    template <class Rep, class Period>
    Scoped_Timer<Rep, Period>::~Scoped_Timer(void)
    {
        this->finish_time_ = _clock_type::now();
        this->finished_.store(true);
        this->on_finish_(*this);
    }


    template <class Rep, class Period>
    std::optional<typename Scoped_Timer<Rep, Period>::_duration_type>
    Scoped_Timer<Rep, Period>::get_finish_duration(void) const
    {
        if (this->get_finish_time())
        {
            return std::chrono::duration_cast<_duration_type>(this->finish_time_ - this->start_time_);
        }
        else
        {
            return std::nullopt;
        }
    }


    template <class Rep, class Period>
    std::optional<typename Scoped_Timer<Rep, Period>::_time_point_type>
    Scoped_Timer<Rep, Period>::get_finish_time(void) const
    {
        if (this->finished_.load())
        {
            return this->finish_time_;
        }
        else
        {
            return std::nullopt;
        }
    }


    template <class Rep, class Period>
    typename Scoped_Timer<Rep, Period>::_duration_type
    Scoped_Timer<Rep, Period>::get_intermediate_duration(void) const
    {
        return std::chrono::duration_cast<_duration_type>(_clock_type::now() - this->start_time_);
    }


    template <class Rep, class Period>
    typename Scoped_Timer<Rep, Period>::_time_point_type
    Scoped_Timer<Rep, Period>::get_intermediate_time(void) const
    {
        return _clock_type::now();
    }


    template <class Rep, class Period>
    typename Scoped_Timer<Rep, Period>::_time_point_type
    Scoped_Timer<Rep, Period>::get_start_time(void) const
    {
        return this->start_time_;
    }
}


#endif // PENGUIN_SCOPED_TIMER_H
