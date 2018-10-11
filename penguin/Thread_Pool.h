#ifndef PENGUIN_THREAD_POOL_H
#define PENGUIN_THREAD_POOL_H


#include "Penguin_export.h"
#include "Executor.h"
#include "Queue_T.h"
#include <thread>
#include <vector>


namespace Penguin
{
    class PENGUIN_Export Thread_Pool : public Penguin::Executor
    {
    public:
        static const uint32_t QUEUE_CAPACITY = 5;

        ThreadPool(const uint32_t threadCount);
        virtual ~ThreadPool(void);

        virtual int execute(const Penguin::Runnable &runnable);
        virtual int execute(Penguin::Runnable &&runnable);

        int run(void);

    protected:
        static int threadFunc(Penguin::Queue_T<Penguin::Runnable> *queue_ptr);

    private:
        // Not copyable
        ThreadPool(const ThreadPool &other) = delete;
        ThreadPool & operator = (const ThreadPool &other) = delete;

        // No moveable
        ThreadPool(ThreadPool &&other) = delete;
        ThreadPool & operator = (ThreadPool &&other) = delete;

        Penguin::Queue_T<Penguin::Runnable> queue_;
        std::vector<std::thread>            threads_;
    };

}


#endif // PENGUIN_THREAD_POOL_H