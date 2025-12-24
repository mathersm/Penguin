/*
* Copyright (c) 2025 Michael Mathers
*/
#include <penguin/Thread_Pool.h>
#include "penguin/Stream.h"
#include <algorithm>
#include <future>
#include <iostream>
#include <numeric>
#include <string>
#include <syncstream>
#include <vector>




namespace
{
    class Test_Task_Simple : public Penguin::Thread_Pool_Task
    {
    public:
        Test_Task_Simple(uint32_t task_id)
            : Thread_Pool_Task()
            , taskID_(task_id)
        {
        }

        virtual ~Test_Task_Simple(void)
        {
        }

        int run(void) override
        {
            std::osyncstream(std::cout) << "Test_Task_Simple(" << std::to_string(this->taskID_) << ")::run - " << std::this_thread::get_id() << '\n';
            return 0;
        }

    private:
        uint32_t taskID_;
    };


    void print_test_start(std::string test_name_text)
    {
        std::osyncstream(std::cout) << "Starting..." << test_name_text.c_str() << '\n';
    }


    void print_test_result(bool result, std::string test_text)
    {
        std::osyncstream(std::cout) << "[" << (result ? " OK " : "FAIL") << "] " << test_text.c_str() << '\n';
    }


    int simple_execute_move(void)
    {
        uint32_t pool_thread_count = 4;
        uint32_t task_thread_count = 20;

        Penguin::Thread_Pool pool(pool_thread_count);

        for (uint32_t n = 0; n < task_thread_count; n++)
        {
            std::shared_ptr<Test_Task_Simple> task = std::make_shared<Test_Task_Simple>(n);
            pool.execute(std::move(task));
        }

        // Wait for a bit so the thread pool isn't destroyed while tasks are running, ending them prematurely
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        return 0;
    }


    int simple_execute_reference(void)
    {
        uint32_t pool_thread_count = 5;
        uint32_t task_thread_count = 40;

        Penguin::Thread_Pool pool(pool_thread_count);

        for (uint32_t n = 0; n < task_thread_count; n++)
        {
            std::shared_ptr<Test_Task_Simple> task = std::make_shared<Test_Task_Simple>(n);
            pool.execute(task);
        }

        // Wait for a bit so the thread pool isn't destroyed while tasks are running, ending them prematurely
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        return 0;
    }


    int simple_execute_reference_two(void)
    {
        uint32_t pool_thread_count = 5;
        uint32_t task_thread_count = 40;

        Penguin::Thread_Pool pool(pool_thread_count);

        // This time create all the tasks up front
        std::vector<std::shared_ptr<Test_Task_Simple>> task_vector;
        for (uint32_t n = 0; n < task_thread_count; n++)
        {
            task_vector.emplace_back(std::make_shared<Test_Task_Simple>(n));
        }
        for (auto& task : task_vector)
        {
            pool.execute(task);
        }

        // Wait for a bit so the thread pool isn't destroyed while tasks are running, ending them prematurely
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        return 0;
    }


    bool test_simple_execute_move(void)
    {
        print_test_start("test_simple_execute_move()");

        bool successful_result = false;
        long long test_timeout_seconds = 20;

        std::future<int> result = std::async(std::launch::async, simple_execute_move);
        std::future_status result_status = result.wait_for(std::chrono::seconds(test_timeout_seconds));
        if (result_status == std::future_status::ready)
        {
            successful_result = true;
        }
        print_test_result(successful_result, "test_simple_execute_move()");
        return successful_result;
    }


    bool test_simple_execute_reference(void)
    {
        print_test_start("test_simple_execute_reference()");

        bool successful_result = false;
        long long test_timeout_seconds = 20;
       
        std::future<int> result = std::async(std::launch::async, simple_execute_reference);
        std::future_status result_status = result.wait_for(std::chrono::seconds(test_timeout_seconds));
        if (result_status == std::future_status::ready)
        {
            successful_result = true;
        }
        print_test_result(successful_result, "test_simple_execute_reference()");
        return successful_result;
    }


    bool test_simple_execute_reference_two(void)
    {
        print_test_start("test_simple_execute_reference_two()");

        bool successful_result = false;
        long long test_timeout_seconds = 20;

        std::future<int> result = std::async(std::launch::async, simple_execute_reference_two);
        std::future_status result_status = result.wait_for(std::chrono::seconds(test_timeout_seconds));
        if (result_status == std::future_status::ready)
        {
            successful_result = true;
        }
        print_test_result(successful_result, "test_simple_execute_reference_two()");
        return successful_result;
    }
}


int main(int argc, char *argv[])
{
    std::osyncstream(std::cout) << "Test_Thread_Pool" << '\n';
    bool pass = true;
    pass &= test_simple_execute_move();
    pass &= test_simple_execute_reference();
    pass &= test_simple_execute_reference_two();
    

    return (pass ? 0 : -1);
}
