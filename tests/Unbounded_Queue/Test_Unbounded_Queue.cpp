/*
* Copyright (c) 2017 Michael Mathers
*/
#include <penguin/Unbounded_Queue.h>
#include <future>
#include <iostream>
#include <numeric>


namespace
{
    void print_test_result(int result, std::string test_text)
    {
        std::cout << "[" << (result ? "FAIL" : " OK ") << "] " << test_text.c_str() << std::endl;
    }


    int push(Penguin::Unbounded_Queue<int>* queue, int n)
    {
        queue->push(n);
        return n;
    }


    int pop(Penguin::Unbounded_Queue<int>* queue)
    {
        return queue->pop();
    }


    int try_pop_for(Penguin::Unbounded_Queue<int>* queue)
    {
        try
        {
            return queue->try_pop_for(std::chrono::seconds(3));
        }
        catch (const Penguin::Timeout_Exception& ex)
        {
            ex;
            return -1;
        }
    }


    int try_pop_until(Penguin::Unbounded_Queue<int>* queue)
    {
        try
        {
            return queue->try_pop_until(std::chrono::system_clock::now() + std::chrono::seconds(3));
        }
        catch (const Penguin::Timeout_Exception& ex)
        {
            ex;
            return -1;
        }
    }


    int test_push(void)
    {
        bool successful_result = true;
        Penguin::Unbounded_Queue<int> queue;

        for (int n = 0; n < 3; n++)
        {
            queue.push(n);
            successful_result &= (queue.size() == n + 1);
        }

        successful_result &= (0 == queue.pop());

        int result = (successful_result ? 0 : -1);
        print_test_result(result, "test_push()");
        return result;
    }


    int test_pop(void)
    {
        bool successful_result = true;
        Penguin::Unbounded_Queue<int> queue;

        std::vector<std::future<int>> push_results;
        push_results.push_back(std::async(std::launch::async, push, &queue, 1));
        push_results.push_back(std::async(std::launch::async, push, &queue, 2));
        
        std::vector<std::future<int>> pop_results;
        pop_results.push_back(std::async(std::launch::async, pop, &queue));
        pop_results.push_back(std::async(std::launch::async, pop, &queue));
        
        int push_result = std::accumulate(push_results.begin(), push_results.end(), 0, [](int a, std::future<int>& f) {return a + f.get(); });
        successful_result &= (push_result == 3);

        int pop_result = std::accumulate(pop_results.begin(), pop_results.end(), 0, [](int a, std::future<int>& f) {return a + f.get(); });
        successful_result &= (pop_result == 3);

        // Spawn a task that waits to pop with nothing in the queue
        std::future<int> blocked_pop = std::async(std::launch::async, pop, &queue);
        successful_result &= (std::future_status::timeout == blocked_pop.wait_for(std::chrono::seconds(2)));

        std::future<int> unblock_push = std::async(std::launch::async, push, &queue, 3);
        successful_result &= (3 == blocked_pop.get());

        int result = (successful_result ? 0 : -1);
        print_test_result(result, "test_pop()");
        return result;
    }


    int test_try_pop_for(void)
    {
        bool successful_result = true;
        Penguin::Unbounded_Queue<int> queue;

        std::vector<std::future<int>> pop_for_results;
        pop_for_results.push_back(std::async(std::launch::async, try_pop_for, &queue));
        pop_for_results.push_back(std::async(std::launch::async, try_pop_for, &queue));

        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Add one item
        std::async(std::launch::async, push, &queue, 1);

        // Wait for remaining threads attempting to pop from the queue to time out
        int pop_for_result = std::accumulate(pop_for_results.begin(), pop_for_results.end(), 0, [](int a, std::future<int>& f) {return a + f.get(); });
        successful_result &= (0 == queue.size());
        successful_result &= (pop_for_result == 0);

        int result = (successful_result ? 0 : -1);
        print_test_result(result, "test_try_pop_for()");
        return result;
    }


    int test_try_pop_until(void)
    {
        bool successful_result = true;
        Penguin::Unbounded_Queue<int> queue;

        std::vector<std::future<int>> pop_until_results;
        pop_until_results.push_back(std::async(std::launch::async, try_pop_until, &queue));
        pop_until_results.push_back(std::async(std::launch::async, try_pop_until, &queue));

        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Add one item
        std::async(std::launch::async, push, &queue, 1);

        // Wait for remaining threads attempting to pop from the queue to time out
        int pop_for_result = std::accumulate(pop_until_results.begin(), pop_until_results.end(), 0, [](int a, std::future<int>& f) {return a + f.get(); });
        successful_result &= (0 == queue.size());
        successful_result &= (pop_for_result == 0);

        int result = (successful_result ? 0 : -1);
        print_test_result(result, "test_try_pop_until()");
        return result;
    }
}


int main(int argc, char *argv[])
{
    std::cout << "Test_Unbounded_Queue" << std::endl;
    int result = 0;
    result |= test_push();
    result |= test_pop();
    result |= test_try_pop_for();
    result |= test_try_pop_until();

    return result;
}
