/*
* Copyright (c) 2017 Michael Mathers
*/
#include <penguin/Scoped_Timer.h>
#include <iostream>
#include <thread>
#include <type_traits>


namespace
{
    void print_double_duration_ms(const Penguin::Scoped_Timer<double, std::milli>& timer)
    {
        std::cout << "Finish duration = " << timer.get_finish_duration().value().count() << "ms" << '\n';
    }


    void print_int_duration_us(const Penguin::Scoped_Timer<int, std::micro>& timer)
    {
        std::cout << "Finish duration = " << timer.get_finish_duration().value().count() << "us" << '\n';
    }


    template <typename Rep>
    void print_finish_time(const Penguin::Scoped_Timer<Rep, std::nano>& timer)
    {
        std::cout << "Finish time = " << timer.get_finish_time().value().time_since_epoch().count() << "ns" << '\n';
    }


    bool test_microsecond_durations(void)
    {
        Penguin::Scoped_Timer<int, std::micro> timer(print_int_duration_us);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Intermediate duration = " << timer.get_intermediate_duration().count() << "us" << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return true;
    }

    bool test_millisecond_durations(void)
    {
        Penguin::Scoped_Timer<double, std::milli> timer(print_double_duration_ms);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "Intermediate duration = " << timer.get_intermediate_duration().count() << "ms" << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return true;
    }

    bool test_nanosecond_times(void)
    {
        Penguin::Scoped_Timer<unsigned long long, std::nano> timer(print_finish_time<unsigned long long>);
        std::cout << "Start time = " << timer.get_start_time().time_since_epoch().count() << "ns" << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Intermediate time = " << timer.get_intermediate_time().time_since_epoch().count() << "ns" << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return true;
    }

}


int main(int argc, char *argv[])
{
    std::cout << "Test_Scoped_Timer" << std::endl;
    bool tests_passed = true;
    tests_passed &= test_microsecond_durations();
    tests_passed &= test_millisecond_durations();
    tests_passed &= test_nanosecond_times();

    return (tests_passed ? 0 : -1);
}
