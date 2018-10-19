/*
* Copyright (c) 2018 Michael Mathers
*/
#include <penguin/Dynamic_Library.h>
#include <iostream>
#include <sstream>


namespace
{
    void print_test_result(int result, std::string test_text)
    {
        std::cout << "[" << (result ? "FAIL" : " OK ") << "] " << test_text.c_str() << std::endl;
    }


    int test_load_library(void)
    {
        int result = 0;

        std::stringstream library_name_stream;
        library_name_stream << "Penguin" << "." << Penguin::dynamic_library_extension;

        result |= (NULL == Penguin::load_library(library_name_stream.str()));
        result |= (NULL == Penguin::load_library("Penguin"));

        print_test_result(result, "test_load_library()");
        return result;
    }


    int test_find_function(void)
    {
        int result = 0;

        Penguin::Library_Handle library_handle = Penguin::load_library("Penguin");
        
        result |= (NULL == Penguin::get_library_function(library_handle, "test_library_function"));

        print_test_result(result, "test_find_function()");
        return result;
    }
}


int main(int argc, char *argv[])
{
    std::cout << "Test_Dynamic_Library" << std::endl;
    int result = 0;
    result |= test_load_library();
    result |= test_find_function();

    return result;
}
