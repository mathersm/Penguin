/*
 * Copyright (c) 2018 Michael Mathers
 */
#ifndef PENGUIN_COMPONENT_H
#define PENGUIN_COMPONENT_H


#include "Penguin_export.h"
#include <string>


namespace Penguin
{
    class Penguin_Export Component
    {
    public:
        explicit Component(const std::string& command_line_args);
        virtual ~Component(void);

    private:
        Component(void) = delete;
        Component(const Component&) = delete;
        Component& operator = (const Component&) = delete;
    };
}


#endif // PENGUIN_COMPONENT_H

