/*
* Copyright (c) 2017 Michael Mathers
*/
#include "Exception.h"


namespace Penguin
{
    Timeout_Exception::Timeout_Exception(const std::string &what)
        : std::runtime_error(what)
    {
    }


    Timeout_Exception::Timeout_Exception(const char *what)
        : std::runtime_error(what)
    {
    }
}