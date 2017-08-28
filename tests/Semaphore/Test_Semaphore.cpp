/*
* Copyright (c) 2017 Michael Mathers
*/
#include <penguin/Semaphore.h>
#include <future>
#include <iostream>
#include <numeric>


namespace
{
    void print_test_result(int result, std::string test_text)
    {
        std::cout << "[" << (result ? "FAIL" : " OK ") << "] " << test_text.c_str() << std::endl;
    }


    int acquire(Penguin::Semaphore* sema)
    {
        sema->acquire();
        return 0;
    }


    int release(Penguin::Semaphore* sema)
    {
        sema->release();
        return 0;
    }


    int test_acquire(void)
    {
        Penguin::Semaphore semaphore(0);

        std::future<int> acquire_result = std::async(std::launch::async, acquire, &semaphore);
        std::future<int> release_result = std::async(std::launch::async, release, &semaphore);

        int result = acquire_result.get();
        print_test_result(result, "test_acquire()");
        return result;
    }


    int test_waiters(void)
    {
        Penguin::Semaphore semaphore(0);

        std::vector<std::future<int>> acquire_results;
        acquire_results.push_back(std::async(std::launch::async, acquire, &semaphore));
        acquire_results.push_back(std::async(std::launch::async, acquire, &semaphore));
        acquire_results.push_back(std::async(std::launch::async, acquire, &semaphore));

        std::this_thread::sleep_for(std::chrono::seconds(2));

        size_t waiters = acquire_results.size();

        int result = !(semaphore.waiters() == waiters);
        
        // Make sure we allow the other threads to come off their waits
        for (size_t i = 0; i < waiters; ++i)
        {
            std::async(std::launch::async, release, &semaphore);
        }

        int acquire_result = std::accumulate(acquire_results.begin(), acquire_results.end(), 0, [](int a, std::future<int>& f) {return a + f.get(); });
        result |= acquire_result;

        print_test_result(result, "test_waiters()");
        return result;
    }


    int test_permits(void)
    {
        Penguin::Semaphore semaphore(2);

        int result = !(semaphore.permits() == 2);

        std::vector<std::future<int>> acquire_results;
        acquire_results.push_back(std::async(std::launch::async, acquire, &semaphore));
        acquire_results.push_back(std::async(std::launch::async, acquire, &semaphore));
        acquire_results.push_back(std::async(std::launch::async, acquire, &semaphore));

        std::this_thread::sleep_for(std::chrono::seconds(2));
        result |= !(semaphore.permits() == 0);
        result |= !(semaphore.waiters() == 1);

        // Make sure we allow the other threads to come off their waits
        size_t waiters = semaphore.waiters();
        for (size_t i = 0; i < waiters; ++i)
        {
            std::async(std::launch::async, release, &semaphore);
        }

        int acquire_result = std::accumulate(acquire_results.begin(), acquire_results.end(), 0, [](int a, std::future<int>& f) {return a + f.get(); });
        result |= acquire_result;

        print_test_result(result, "test_permits()");
        return result;
    }
}


int main(int argc, char *argv[])
{
    std::cout << "Test_Semaphore" << std::endl;
    int result = 0;
    result |= test_acquire();
    result |= test_waiters();
    result |= test_permits();
    return result;
}
