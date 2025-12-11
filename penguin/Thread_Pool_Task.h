/*
 * Copyright (c) 2025 Michael Mathers
 */
#ifndef PENGUIN_THREAD_POOL_TASK_H
#define PENGUIN_THREAD_POOL_TASK_H


#include "Penguin_export.h"


namespace Penguin
{
    class Penguin_Export Thread_Pool_Task
    {
    public:
        Thread_Pool_Task(void);
        virtual ~Thread_Pool_Task(void);

        /// TO-DO No interrupt behaviour available as yet
        // virtual int interrupt(void);

        virtual int run(void);
    };
}


#endif // PENGUIN_THREAD_POOL_TASK_H
