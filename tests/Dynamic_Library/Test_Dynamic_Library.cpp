/*
* Copyright (c) 2018 Michael Mathers
*/
#include <penguin/Dynamic_Library.h>
#include <iostream>
#include <sstream>


namespace
{
    void print_test_result(bool result, std::string test_text)
    {
        std::cout << "[" << (result ? "OK" : " FAIL ") << "] " << test_text.c_str() << std::endl;
    }


    bool test_load_and_unload_library(void)
    {
        bool pass = true;

        std::stringstream library_name_stream;
        library_name_stream << "Test_Component" << "." << Penguin::dynamic_library_extension;

        Penguin::Library_Handle library_handle;

        pass &= (NULL != (library_handle = Penguin::load_library(library_name_stream.str())));
        pass &= (NULL != Penguin::load_library("Test_Component"));
        pass &= (NULL == Penguin::load_library("MakingItUp")); // Should fail, doesn't exist
        pass &= (0 == Penguin::unload_library(library_handle));
        pass &= (0 == Penguin::unload_library(library_handle));
        pass &= (0 != Penguin::unload_library(library_handle)); // Should have only been 2 references of this library loaded, last should fail

        print_test_result(pass, "test_load_and_unload_library()");
        return pass;
    }


    bool test_find_function(void)
    {
        bool pass = true;

        Penguin::Library_Handle library_handle;
        
        pass &= (NULL != (library_handle = Penguin::load_library("Test_Component")));
        pass &= (NULL != Penguin::get_library_function(library_handle, "Test_Component_Factory"));
        pass &= (0 == Penguin::unload_library(library_handle));

        print_test_result(pass, "test_find_function()");
        return pass;
    }
}


int main(int argc, char *argv[])
{
    std::cout << "Test_Dynamic_Library" << std::endl;
    bool pass = true;
    pass &= test_load_and_unload_library();
    pass &= test_find_function();

    return (pass ? 0 : -1);
}
