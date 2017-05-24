/*
* (c) Copyright 2017
*
* @file     Exception.cpp
* @author   Michael Mathers
*/
#include "Exception.h"


namespace Penguin
{
    TimeoutException::TimeoutException(const std::string &what)
        : std::runtime_error(what)
    {
    }


    TimeoutException::TimeoutException(const char *what)
        : std::runtime_error(what)
    {
    }
}