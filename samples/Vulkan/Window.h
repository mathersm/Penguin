/*
* Copyright (c) 2019 Michael Mathers
*/
#ifndef PENGUIN_SAMPLE_WINDOW_H
#define PENGUIN_SAMPLE_WINDOW_H


#include <Windows.h>


namespace Penguin::Sample
{
    class Application;

    class Window
    {
    public:
        Window(void);
        ~Window(void);

    public:
        void close(void);
        void initialise(LONG_PTR user_data, WNDPROC window_event_handler);
        void show(void);

        RECT        get_client_area(void) const;
        HINSTANCE   get_instance_handle(void) const;
        HWND        get_window_handle(void) const;

    private:
        void    set_user_data(LONG_PTR user_data);

    private:
        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator = (const Window&) = delete;
        Window& operator = (Window&&) = delete;

    private:
        HINSTANCE   instance_handle_;
        HWND        window_handle_;
    };
}


#endif // PENGUIN_SAMPLE_WINDOW_H
