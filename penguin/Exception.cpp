/*
* Copyright (c) 2017 Michael Mathers
*/
#include "Exception.h"
#include "Profiler.h"


namespace Penguin
{
    Timeout_Exception::Timeout_Exception(const std::string &what)
        : std::runtime_error(what)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);
    }


    Timeout_Exception::Timeout_Exception(const char *what)
        : std::runtime_error(what)
    {
        PENGUIN_CORE_PROFILE(__FUNCTION__);
    }
}