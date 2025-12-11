/*
 * Copyright (c) 2025 Michael Mathers
 */
#ifndef PENGUIN_THREAD_POOL_H
#define PENGUIN_THREAD_POOL_H


#include "Penguin_export.h"
#include "Unbounded_Queue.h"
#include <thread>
#include <vector>


namespace Penguin
{
    class Penguin_Export Thread_Pool_Task
    {
    public:
        Thread_Pool_Task(void);
        virtual ~Thread_Pool_Task(void);

        virtual int interrupt(void);
        virtual int run(void);
        void set_stop_token(const std::stop_token& stop_token);

    private:
        std::stop_token stopToken_;
    };


    class Penguin_Export Thread_Pool
    {
    public:
        using _queue_type = Penguin::Unbounded_Queue<std::shared_ptr<Thread_Pool_Task>>;

        static constexpr std::chrono::milliseconds RETRIEVAL_TIMEOUT = std::chrono::milliseconds(5000);

        explicit Thread_Pool(const uint32_t initial_thread_count);
        virtual ~Thread_Pool(void);

        virtual int execute(const std::shared_ptr<Thread_Pool_Task>& task);
        virtual int execute(std::shared_ptr<Thread_Pool_Task>&& task);


    protected:
        static int thread_function(std::stop_token stop_token, Thread_Pool* pool_ptr);

    private:
        // Not copyable
        Thread_Pool(const Thread_Pool& other) = delete;
        Thread_Pool& operator = (const Thread_Pool& other) = delete;

        // Not moveable
        Thread_Pool(Thread_Pool&& other) = delete;
        Thread_Pool& operator = (Thread_Pool&& other) = delete;

        volatile bool               shutdown_;
        _queue_type                 taskQueue_;
        std::vector<std::jthread>   threads_;    
    };

}


#endif // PENGUIN_THREAD_POOL_H
