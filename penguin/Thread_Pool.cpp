#define PENGUIN_THREAD_POOL_CPP


#include "ThreadPool.h"


namespace Penguin
{

    ThreadPool::ThreadPool(const uint32_t threadCount)
        : queue_(ThreadPool::QUEUE_CAPACITY)
        , threads_(threadCount)
    {
    }

    ThreadPool::~ThreadPool(void)
    {
        // Before closing the thread pool we should make sure that all the threads have finished
        for (auto &thread : this->threads_)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }


    int
    ThreadPool::execute(const Penguin::Runnable &runnable)
    {
        // Offer the runnable to the queue
        this->queue_.put(runnable);

        // If we aren't able to transfer execution to a waiting thread then create a new one
        return 0;
    }


    int
    ThreadPool::execute(Penguin::Runnable &&runnable)
    {
        // Offer the runnable to the queue
        this->queue_.put(runnable);

        // If we aren't able to transfer execution to a waiting thread then create a new one
        return 0;
    }


    int
    ThreadPool::run(void)
    {
        for (auto &thread : this->threads_)
        {
            thread = std::thread(ThreadPool::threadFunc, &this->queue_);
        }
        return 0;
    }


    int
    ThreadPool::threadFunc(Penguin::Queue_T<Penguin::Runnable> *queue_ptr)
    {
        
        // Wait for a runnable to be available on the queue
        Penguin::Runnable &runnable = queue_ptr->take();
        
        return runnable.run();
    }
    








}