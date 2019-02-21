/*
* Copyright (c) 2019 Michael Mathers
*/
#include "Window.h"
#include <sstream>
#include <iostream>


namespace Penguin::Sample
{
    Window::Window(void)
    {
    }


    Window::~Window(void)
    {
    }


    void
    Window::close(void)
    {

    }


    RECT
    Window::get_client_area(void) const
    {
        RECT client_area;
        GetClientRect(this->window_handle_, std::addressof(client_area));
        return client_area;
    }


    HINSTANCE
    Window::get_instance_handle(void) const
    {
        return this->instance_handle_;
    }


    HWND
    Window::get_window_handle(void) const
    {
        return this->window_handle_;
    }


    void
    Window::initialise(LONG_PTR user_data, WNDPROC window_event_handler)
    {
        this->instance_handle_ = GetModuleHandle(NULL);

        std::string window_class_name("SampleWindowClass");
        std::string window_title("Sample Window");

        WNDCLASSEX window_class = {};
        window_class.cbSize = sizeof(WNDCLASSEX);
        window_class.style = CS_HREDRAW | CS_VREDRAW;
        window_class.lpfnWndProc = window_event_handler;
        window_class.hInstance = this->instance_handle_;
        window_class.hIcon = LoadIcon(this->instance_handle_, "IDI_ICON");
        window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
        window_class.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
        window_class.lpszClassName = window_class_name.c_str();

        if (!RegisterClassEx(&window_class))
        {
            std::stringstream error_stream;
            error_stream << "Failed to register window class (" << GetLastError() << ")";
            throw std::runtime_error(error_stream.str());
        }

        RECT window_area =
        {
            0, 0, 800, 600
        };
        AdjustWindowRect(std::addressof(window_area), WS_OVERLAPPEDWINDOW, FALSE);

        this->window_handle_ = CreateWindowEx
        (
            0,
            window_class_name.c_str(),
            window_title.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            window_area.right - window_area.left,
            window_area.bottom - window_area.top,
            nullptr,
            nullptr,
            this->instance_handle_,
            nullptr
        );

        if (this->window_handle_ == 0)
        {
            std::stringstream error_stream;
            error_stream << "Failed to create window (" << GetLastError() << ")";
            throw std::runtime_error(error_stream.str());
        }

        this->set_user_data(user_data);
    }


    void Window::set_user_data(LONG_PTR user_data)
    {
        SetWindowLongPtr(this->window_handle_, GWLP_USERDATA, user_data);
    }


    void Window::show(void)
    {
        ShowWindow(this->window_handle_, SW_SHOW);
    }
}