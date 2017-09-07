/*
 * Copyright (c) 2017 Michael Mathers
 */
#ifndef PENGUIN_EXCEPTION_H
#define PENGUIN_EXCEPTION_H


#include "Penguin_export.h"
#include <stdexcept>


namespace Penguin
{
    struct Penguin_Export Timeout_Exception : public std::runtime_error
    {
        explicit Timeout_Exception(const std::string &what);
        explicit Timeout_Exception(const char *what);
    };
} // namespace Penguin


#endif // PENGUIN_EXCEPTION_H
