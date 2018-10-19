/*
* Copyright (c) 2018 Michael Mathers
*/
#ifndef TEST_COMPONENT_H
#define TEST_COMPONENT_H


#include "Test_Component_export.h"
#include <penguin/Component_Container.h>


namespace Test
{
    class Test_Component_Export Test_Component : public Penguin::Component
    {
    public:
        explicit Test_Component(const std::string& command_line_args);
        virtual ~Test_Component(void);
    };
}


extern "C" Test_Component_Export Test::Test_Component* Test_Component_Factory(const std::string& command_line_args)
{
    return new Test::Test_Component(command_line_args);
}


#endif // TEST_COMPONENT_H