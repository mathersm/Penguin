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
        
        // Penguin::load_library("test");

        print_test_result(result, "test_load()");
        return result;
    }


    int test_find_function(void)
    {
        int result = 0;

        // Penguin::get_function("Penguin", "Test");

        print_test_result(result, "test_load()");
        return result;
    }
}


int main(int argc, char *argv[])
{
    std::cout << "Test_Version" << std::endl;
    int result = 0;
    result |= test_load_library();
    result |= test_find_function();

    return result;
}
