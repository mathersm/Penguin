/*
* Copyright (c) 2017 Michael Mathers
*/
#include <penguin/Profiler.h>
#include <iostream>
#include <thread>

namespace
{
    int test_delay_one_second(void)
    {
        PENGUIN_PROFILE(__FUNCTION__);

        std::this_thread::sleep_for(std::chrono::seconds(1));
        return 0;
    }

    int test_delay_two_seconds(void)
    {
        PENGUIN_PROFILE(__FUNCTION__);

        std::this_thread::sleep_for(std::chrono::seconds(2));
        return 0;
    }
}


int main(int argc, char *argv[])
{
    std::cout << "Test_Profiler" << std::endl;
    int result = 0;
    result |= test_delay_one_second();
    result |= test_delay_two_seconds();

    PENGUIN_PROFILE_DUMP;

    return result;
}
