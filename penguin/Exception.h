/*
 * Copyright (c) 2017 Michael Mathers
 */
#ifndef PENGUIN_EXCEPTION_H
#define PENGUIN_EXCEPTION_H


#include "Penguin_export.h"
#include <stdexcept>


namespace Penguin
{
    struct Penguin_Export TimeoutException : public std::runtime_error
    {
        TimeoutException(const std::string &what);
        TimeoutException(const char *what);
    };
} // namespace Penguin


#endif // PENGUIN_EXCEPTION_H
