/*
* Copyright (c) 2018 Michael Mathers
*/
#include <penguin/Component_Container.h>
#include <iostream>
#include <sstream>


namespace
{
    void print_test_result(bool result, std::string test_text)
    {
        std::cout << "[" << (result ? "OK" : " FAIL ") << "] " << test_text.c_str() << std::endl;
    }


    bool test_load_component(void)
    {
        bool pass = true;

        Penguin::Component_Container container;
        pass &= (0 == container.load_component("ComponentA", "Test_Component", "Test_Component_Factory", ""));

        print_test_result(pass, "test_load_component()");
        return pass;
    }


    bool test_load_component_duplicate_key(void)
    {
        bool pass = true;

        Penguin::Component_Container container;
        pass &= (0 == container.load_component("ComponentA", "Test_Component", "Test_Component_Factory", ""));
        pass &= (-1 == container.load_component("ComponentA", "Test_Component", "Test_Component_Factory", ""));

        print_test_result(pass, "test_load_component_duplicate_key()");
        return pass;
    }


    bool test_unload_component(void)
    {
        bool pass = true;

        Penguin::Component_Container container;
        pass &= (0 == container.load_component("ComponentA", "Test_Component", "Test_Component_Factory", ""));
        pass &= (0 == container.unload_component("ComponentA"));

        print_test_result(pass, "test_unload_component()");
        return pass;
    }

    bool test_unload_component_not_found(void)
    {
        bool pass = true;

        Penguin::Component_Container container;
        pass &= (-1 == container.unload_component("ComponentB"));

        print_test_result(pass, "test_unload_component_not_found()");
        return pass;
    }
}


int main(int argc, char *argv[])
{
    std::cout << "Test_Component_Container" << std::endl;
    bool pass = true;
    pass &= test_load_component();
    pass &= test_load_component_duplicate_key();
    pass &= test_unload_component();
    pass &= test_unload_component_not_found();

    return (pass ? 0 : -1);
}
