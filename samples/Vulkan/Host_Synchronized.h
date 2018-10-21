#/*
* Copyright (c) 2018 Michael Mathers
*/
#ifndef PENGUIN_SAMPLE_HOST_SYNCHRONIZED_H
#define PENGUIN_SAMPLE_HOST_SYNCHRONIZED_H


#include <mutex>


namespace Penguin::Sample
{
    template <typename T>
    struct Host_Synchronized
    {
        using _mutex_type = std::mutex;
        using _synced_type = T;

        std::mutex mutex;
        T synced_object;
    };
}


#endif // PENGUIN_SAMPLE_HOST_SYNCHRONIZED_H