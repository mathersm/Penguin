/*
* Copyright (c) 2018 Michael Mathers
*/
#include "Application.h"
#include <sstream>
#include <iostream>


namespace
{
    LRESULT CALLBACK window_event_handler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
    {
        auto application = reinterpret_cast<Penguin::Sample::Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        switch (message)
        {
        case WM_DESTROY:
            application->on_shutdown();
            PostQuitMessage(0);
            break;
        }


        return DefWindowProc(hwnd, message, wparam, lparam);
    }
}


namespace Penguin::Sample
{
    Application::Application(void)
        : application_running_(false)
    {
    }


    Application::~Application(void)
    {
    }


    void
    Application::configure(void)
    {
        this->window_ = std::make_shared<Penguin::Sample::Window>();
        this->window_->initialise(reinterpret_cast<LONG_PTR>(this), window_event_handler);

        this->vulkan_renderer_ = std::make_shared<Penguin::Sample::Vulkan_Renderer>();
        this->vulkan_renderer_->initialise(this->window_);
    }


    void
    Application::on_shutdown(void)
    {
        this->application_running_.store(false);
    }


    void
    Application::run(void)
    {
        this->vulkan_renderer_->run();
        this->window_->show();

        MSG message;

        this->application_running_.store(true);

        while (this->application_running_)
        {
            WaitMessage();
            while (PeekMessage(&message, this->window_->get_window_handle(), 0, 0, PM_REMOVE))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }

        std::cout << "Exit value = " << message.wParam << "\n";
    }
}