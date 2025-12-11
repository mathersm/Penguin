/*
* Copyright (c) 2025 Michael Mathers
*/
#include <penguin/Thread_Pool.h>
#include <algorithm>
#include <future>
#include <iostream>
#include <numeric>
#include <vector>


namespace
{
    class Test_Task_Simple : public Penguin::Thread_Pool_Task
    {
    public:
        Test_Task_Simple(void)
        {
            // std::cout << "Test_Task_Simple::<ctor> - " << std::this_thread::get_id() << "\n";
        }

        virtual ~Test_Task_Simple(void)
        {
            // std::cout << "Test_Task_Simple::<dtor> - " << std::this_thread::get_id() << "\n";
        }

        int run(void) override
        {
            std::cout << "Test_Task_Simple::run - " << std::this_thread::get_id() << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            return 0;
        }
    };


    void print_test_result(bool result, std::string test_text)
    {
        std::cout << "[" << (result ? " OK " : "FAIL") << "] " << test_text.c_str() << std::endl;
    }


    int simple_execute_move(void)
    {
        Penguin::Thread_Pool pool(4);

        for (int n = 0; n < 20; n++)
        {
            std::shared_ptr<Test_Task_Simple> task = std::make_shared<Test_Task_Simple>();
            pool.execute(std::move(task));
        }

        return 0;
    }


    int simple_execute_reference(void)
    {
        Penguin::Thread_Pool pool(4);

        for (int n = 0; n < 20; n++)
        {
            std::shared_ptr<Test_Task_Simple> task = std::make_shared<Test_Task_Simple>();
            pool.execute(task);
        }

        // This time create all the tasks up front
        std::vector<std::shared_ptr<Test_Task_Simple>> task_vector;
        for (int n = 0; n < 50; n++)
        {
            task_vector.emplace_back(new Test_Task_Simple());
        }
        for (auto& task : task_vector)
        {
            pool.execute(task);
        }

        return 0;
    }


    bool test_simple_execute_move(void)
    {
        bool successful_result = false;
        long long test_timeout_seconds = 10;

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
        bool successful_result = false;
        long long test_timeout_seconds = 10;
       
        std::future<int> result = std::async(std::launch::async, simple_execute_reference);
        std::future_status result_status = result.wait_for(std::chrono::seconds(test_timeout_seconds));
        if (result_status == std::future_status::ready)
        {
            successful_result = true;
        }
        print_test_result(successful_result, "test_simple_execute_reference()");
        return successful_result;
    }
}


int main(int argc, char *argv[])
{
    std::cout << "Test_Thread_Pool" << std::endl;
    bool pass = true;
    pass &= test_simple_execute_reference();
    pass &= test_simple_execute_move();

    return (pass ? 0 : -1);
}
