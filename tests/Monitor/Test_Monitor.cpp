/*
* Copyright (c) 2017 Michael Mathers
*/
#include <penguin/Monitor.h>
#include <future>
#include <iostream>
#include <numeric>


namespace
{
    void print_test_result(int result, std::string test_text)
    {
        std::cout << "[" << (result ? "FAIL" : " OK ") << "] " << test_text.c_str() << std::endl;
    }


    int wait(Penguin::Monitor* mon)
    {
        Penguin::Monitor::_guard_type guard(*mon);
        mon->wait(guard);
        return 0;
    }


    std::cv_status wait_for(Penguin::Monitor* mon, const std::chrono::seconds& rel_time)
    {
        Penguin::Monitor::_guard_type guard(*mon);
        bool wait_return_value = mon->wait_for(guard, rel_time, [] {return false; });
        return (wait_return_value ? std::cv_status::no_timeout : std::cv_status::timeout);
    }


    template <class Clock, class Duration>
    std::cv_status wait_until(Penguin::Monitor* mon, const std::chrono::time_point<Clock, Duration>& timeout_time)
    {
        Penguin::Monitor::_guard_type guard(*mon);
        bool wait_return_value = mon->wait_until(guard, timeout_time, [] {return false; });
        return (wait_return_value ? std::cv_status::no_timeout : std::cv_status::timeout);
    }


    int signal(Penguin::Monitor* mon)
    {
        Penguin::Monitor::_guard_type guard(*mon);
        mon->notify_one();
        return 0;
    }


    int broadcast(Penguin::Monitor* mon)
    {
        Penguin::Monitor::_guard_type guard(*mon);
        mon->notify_all();
        return 0;
    }


    int test_notify_one(void)
    {
        Penguin::Monitor monitor;

        std::future<int> wait_result = std::async(std::launch::async, wait, &monitor);
        std::future<int> signal_result = std::async(std::launch::async, signal, &monitor);
        
        std::future_status wait_status = wait_result.wait_for(std::chrono::seconds(1));

        int result = -1;
        switch (wait_status)
        {
        case std::future_status::ready:
            break;
        case std::future_status::timeout:
            // Temporary destructor will wait for signal to finish
            std::async(std::launch::async, signal, &monitor);
            break;
        default:
            print_test_result(result, "test_notify_one()");
            return result;
        }
        
        result = wait_result.get();
        print_test_result(result, "test_notify_one()");
        return result;
    }


    int test_notify_all(void)
    {
        Penguin::Monitor monitor;
        int result = -1;

        std::vector<std::future<int>> wait_results;
        wait_results.push_back(std::async(std::launch::async, wait, &monitor));
        wait_results.push_back(std::async(std::launch::async, wait, &monitor));
        wait_results.push_back(std::async(std::launch::async, wait, &monitor));

        std::future_status wait_status = wait_results[0].wait_for(std::chrono::seconds(1));
        switch (wait_status)
        {
        case std::future_status::ready:
            break;
        case std::future_status::timeout:
            std::async(std::launch::async, broadcast, &monitor);
            break;
        default:
            print_test_result(result, "test_notify_all()");
            return result;
        }

        result = std::accumulate(
            wait_results.begin(),
            wait_results.end(),
            0,
            [](int a, std::future<int>& f) {return a + f.get(); }
        );
        print_test_result(result, "test_notify_all()");
        return result;
    }


    int test_wait_for_timeout(void)
    {
        Penguin::Monitor monitor;

        std::future<std::cv_status> wait_result = std::async(std::launch::async, wait_for, &monitor, std::chrono::seconds(1));
        std::cv_status timeout_result = wait_result.get();

        int result = timeout_result == std::cv_status::no_timeout;
        print_test_result(result, "test_wait_for_timeout()");
        return result;
    }


    int test_wait_until_timeout(void)
    {
        Penguin::Monitor monitor;

        auto time_now = std::chrono::system_clock::now();
        auto timeout = time_now + std::chrono::seconds(1);
        std::future<std::cv_status> wait_result = std::async(std::launch::async, wait_until<std::chrono::system_clock, std::chrono::system_clock::duration>, &monitor, timeout);

        std::cv_status timeout_result = wait_result.get();

        int result = timeout_result == std::cv_status::no_timeout;
        print_test_result(result, "test_wait_until_timeout()");
        return result;
    }
}


int main(int argc, char *argv[])
{
    std::cout << "Test_Monitor" << std::endl;
    int result = 0;
    result |= test_notify_one();
    result |= test_notify_all();
    result |= test_wait_for_timeout();
    result |= test_wait_until_timeout();
    return result;
}
