/*
* Copyright (c) 2018 Michael Mathers
*/
#ifndef PENGUIN_SAMPLE_VULKAN_APPLICATION_H
#define PENGUIN_SAMPLE_VULKAN_APPLICATION_H


#include "Window.h"
#include "Vulkan_Renderer.h"
#include <atomic>
#include <memory>


namespace Penguin::Sample
{
    class Application
    {
    public:
        Application(void);
        ~Application(void);

    public:
        void configure(void);
        void on_shutdown(void);
        void run(void);

    private:
        Application(const Application&) = delete;
        Application& operator = (const Application&) = delete;

    private:
        std::shared_ptr<Penguin::Sample::Window> window_;
        std::shared_ptr<Penguin::Sample::Vulkan_Renderer> vulkan_renderer_;

        std::atomic<bool>   application_running_;
    };
}


#endif // PENGUIN_SAMPLE_VULKAN_APPLICATION_H