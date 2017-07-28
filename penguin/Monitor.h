#ifndef PENGUIN_MONITOR_H
#define PENGUIN_MONITOR_H


#include <mutex>


namespace Penguin
{
    class Monitor
    {
    public:
        typedef std::mutex                              _mutex_type;
        typedef std::condition_variable                 _condition_variable_type;
        typedef std::unique_lock<typename _mutex_type>  _guard_type;

        Monitor(void);
        virtual ~Monitor(void);

        void notify_one(void) noexcept;
        void notify_all(void) noexcept;

        void wait(_guard_type& guard);

        template <class Rep, class Period>
        std::cv_status wait_for(_guard_type& guard, const std::chrono::duration<Rep, Period>& rel_time);
        
        template <class Clock, class Duration>
        std::cv_status wait_until(_guard_type& guard, const std::chrono::time_point<Clock, Duration>& timeout_time);

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
}


#endif // PENGUIN_MONITOR_H
