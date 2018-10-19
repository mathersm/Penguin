/*
* Copyright (c) 2018 Michael Mathers
*/
#include "Test_Component.h"
#include <iostream>


namespace Test
{
    Test_Component::Test_Component(const std::string& command_line_args)
        : Penguin::Component(command_line_args)
    {
        std::cout << "Test::Test_Component::<ctor>" << '\n';
    }


    Test_Component::~Test_Component(void)
    {
        std::cout << "Test::Test_Component::<dtor>" << '\n';
    }
}
