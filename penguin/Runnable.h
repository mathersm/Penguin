/*
* (c) Copyright 2017
*
* @file     Runnable.h
* @author   Michael Mathers
*/
#ifndef PENGUIN_RUNNABLE_H
#define PENGUIN_RUNNABLE_H


#include "Penguin_export.h"


namespace Penguin
{
    class PENGUIN_Export Runnable
    {
    public:
        virtual int32_t run(void);
    };
}


#endif // PENGUIN_RUNNABLE_H
