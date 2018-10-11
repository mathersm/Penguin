#ifndef PENGUIN_EXECUTOR_H
#define PENGUIN_EXECUTOR_H


#include "Penguin_export.h"
#include "penguin/Runnable.h"


namespace Penguin
{
    class PENGUIN_Export Executor
    {
    public:
        virtual int execute(const Penguin::Runnable &runnable) = 0;
        virtual int execute(Penguin::Runnable &&runnable) = 0;
    };
}


#endif // PENGUIN_EXECUTOR_H