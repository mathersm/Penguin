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


    int try_acquire_for(Penguin::Semaphore* sema)
    {
        std::cv_status result = sema->try_acquire_for(std::chrono::seconds(3));
        return (result == std::cv_status::no_timeout ? 0 : -1);
    }

    
    int try_acquire_until(Penguin::Semaphore* sema)
    {
        std::cv_status result = sema->try_acquire_until(std::chrono::system_clock::now() + std::chrono::seconds(3));
        return (result == std::cv_status::no_timeout ? 0 : -1);
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


    int test_try_acquire_for(void)
    {
        Penguin::Semaphore semaphore(0);

        int result = !(semaphore.permits() == 0);

        std::vector<std::future<int>> acquire_results;
        acquire_results.push_back(std::async(std::launch::async, try_acquire_for, &semaphore));
        acquire_results.push_back(std::async(std::launch::async, try_acquire_for, &semaphore));

        std::this_thread::sleep_for(std::chrono::seconds(1));
        result |= !(semaphore.permits() == 0);
        result |= !(semaphore.waiters() == 2);

        // Release one thread
        std::async(std::launch::async, release, &semaphore);
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        result |= !(semaphore.permits() == 0);
        result |= !(semaphore.waiters() == 1);

        // Wait for remaining threads attempting to acquire to time out
        int acquire_result = std::accumulate(acquire_results.begin(), acquire_results.end(), 0, [](int a, std::future<int>& f) {return a + f.get(); });
        result |= !(semaphore.permits() == 0);
        result |= !(semaphore.waiters() == 0);
        result |= !(acquire_result == -1);

        print_test_result(result, "test_try_acquire_for()");
        return result;
    }


    int test_try_acquire_until(void)
    {
        Penguin::Semaphore semaphore(0);

        int result = !(semaphore.permits() == 0);

        std::vector<std::future<int>> acquire_results;
        acquire_results.push_back(std::async(std::launch::async, try_acquire_until, &semaphore));
        acquire_results.push_back(std::async(std::launch::async, try_acquire_until, &semaphore));

        std::this_thread::sleep_for(std::chrono::seconds(1));
        result |= !(semaphore.permits() == 0);
        result |= !(semaphore.waiters() == 2);

        // Release one thread
        std::async(std::launch::async, release, &semaphore);

        std::this_thread::sleep_for(std::chrono::seconds(1));
        result |= !(semaphore.permits() == 0);
        result |= !(semaphore.waiters() == 1);

        // Wait for remaining threads attempting to acquire to time out
        int acquire_result = std::accumulate(acquire_results.begin(), acquire_results.end(), 0, [](int a, std::future<int>& f) {return a + f.get(); });
        result |= !(semaphore.permits() == 0);
        result |= !(semaphore.waiters() == 0);
        result |= !(acquire_result == -1);

        print_test_result(result, "test_try_acquire_until()");
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
    result |= test_try_acquire_for();
    result |= test_try_acquire_until();
    return result;
}
