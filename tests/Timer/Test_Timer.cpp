/*
 * Copyright (c) 2019 Michael Mathers
 */
#include <penguin/Timer.h>
#include <iostream>
#include <thread>
#include <type_traits>


namespace
{
    bool test_basic_timer_ms(void)
    {
        Penguin::Timer<unsigned long long, std::milli> timer;
        unsigned long long intermediate_split;
        
        timer.start();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        intermediate_split = timer.get_current_duration();
        std::cout << "Slept for " << intermediate_split << "ms so far.\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        intermediate_split = timer.get_current_duration();
        std::cout << "Slept for " << intermediate_split << "ms so far.\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        timer.stop();
        std::cout << "Slept for " << timer.get_finish_duration() << "ms in total.\n";

        return true;
    }


    bool test_basic_timer_us(void)
    {
        Penguin::Timer<unsigned long long, std::micro> timer;
        unsigned long long intermediate_split;

        timer.start();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        intermediate_split = timer.get_current_duration();
        std::cout << "Slept for " << intermediate_split << "us so far.\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        intermediate_split = timer.get_current_duration();
        std::cout << "Slept for " << intermediate_split << "us so far.\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        timer.stop();
        std::cout << "Slept for " << timer.get_finish_duration() << "us in total.\n";

        return true;
    }

}


int main(int argc, char *argv[])
{
    std::cout << "Test_Timer" << std::endl;
    bool tests_passed = true;
    tests_passed &= test_basic_timer_ms();
    tests_passed &= test_basic_timer_us();

    return (tests_passed ? 0 : -1);
}
