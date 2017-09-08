/*
* Copyright (c) 2017 Michael Mathers
*/
#include <penguin/Version.h>
#include <iostream>
#include <sstream>


namespace
{
    void print_test_result(int result, std::string test_text)
    {
        std::cout << "[" << (result ? "FAIL" : " OK ") << "] " << test_text.c_str() << std::endl;
    }


    int test_version_string(void)
    {
        std::string version;
        std::stringstream string_stream;

        string_stream << Penguin::PENGUIN_MAJOR_VERSION;
        string_stream << ".";
        string_stream << Penguin::PENGUIN_MINOR_VERSION;
        string_stream << ".";
        string_stream << Penguin::PENGUIN_PATCH_VERSION;
        
        std::string crafted_version(string_stream.str());
        std::string constexpr_version(Penguin::PENGUIN_VERSION);

        int result = crafted_version.compare(constexpr_version);

        print_test_result(result, "test_version_string()");
        return result;
    }
}


int main(int argc, char *argv[])
{
    std::cout << "Test_Version" << std::endl;
    int result = 0;
    result |= test_version_string();

    return result;
}
