/*
* Copyright (c) 2018 Michael Mathers
*/
#include "Application.h"
#include <iomanip>
#include <iostream>


namespace
{
    template <typename STREAM, typename T>
    void stream_out(STREAM& stream, const T& output)
    {
        stream << output;
    }

    template <typename STREAM, typename T, typename ...U>
    void stream_out(STREAM& stream, const T& output, const U&... more_output)
    {
        stream << output;
        return stream_out(stream, more_output...);
    }
}


int main(int argc, char *argv[])
{
    try
    {
        Penguin::Sample::Application app;
        app.configure();
        app.run();
    }
    catch (const std::system_error& sys_error)
    {
        // std::cerr << "ERROR: std::system_error (" << sys_error.code().value() << ") - " << sys_error.what() << '\n';
        stream_out(std::cerr, "ERROR: std::system_error (", sys_error.code().value(), ") - ", sys_error.what(), '\n');
        return sys_error.code().value();
    }
    catch (const std::runtime_error& rt_error)
    {
        std::cerr << "ERROR: std::runtime_error - " << rt_error.what() << '\n';
        return -1;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "ERROR: std::exception - " << ex.what() << '\n';
        return -1;
    }

    return 0;
}