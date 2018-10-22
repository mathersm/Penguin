/*
* Copyright (c) 2018 Michael Mathers
*/
#ifndef PENGUIN_SAMPLE_VULKAN_APPLICATION_H
#define PENGUIN_SAMPLE_VULKAN_APPLICATION_H


#include "Host_Synchronized.h"
#include "Vulkan_Config.h"
#include <mutex>
#include <string>
#include <vector>


namespace Penguin::Sample
{
    class Application
    {
    public:
        Application(void);
        ~Application(void);

    public:
        void configure(void);
        void run(void);

    private:
        Application(const Application&) = delete;
        Application& operator = (const Application&) = delete;

    private:
        void create_presentation_surface(void);
        void create_vulkan_command_buffers(void);
        void create_vulkan_command_pool(void);
        void create_vulkan_graphics_queue(void);
        void create_vulkan_instance(void);
        void create_vulkan_logical_device(void);
        void create_vulkan_physical_device(void);
        void create_vulkan_swapchain(void);
        vk::SurfaceFormatKHR get_vulkan_swapchain_format(const vk::SurfaceFormatKHR& desired_format) const;
        uint32_t get_vulkan_swapchain_image_count(void) const;
        vk::Extent2D get_vulkan_swapchain_image_size(const vk::Extent2D& desired_image_size) const;
        vk::ImageUsageFlags get_vulkan_swapchain_image_usages(const vk::ImageUsageFlags& desired_image_usages) const;
        vk::SurfaceTransformFlagBitsKHR get_vulkan_swapchain_image_transform(const vk::SurfaceTransformFlagBitsKHR& desired_image_transform) const;
        void set_vulkan_device_extensions(void);
        void set_vulkan_device_layers(void);
        void set_vulkan_instance_extensions(void);
        void set_vulkan_presentation_mode(void);
        void set_vulkan_queue_family(void);
        void set_vulkan_validation_layers(void);

    private:
        std::vector<const char*> vulkan_validation_layers_;
        std::vector<const char*> vulkan_instance_extensions_;
        Penguin::Sample::Host_Synchronized<vk::Instance> vulkan_instance_;
        vk::PhysicalDevice vulkan_physical_device_;
        vk::SurfaceKHR presentation_surface_;
        vk::PresentModeKHR presentation_mode_;
        uint32_t vulkan_queue_family_index_;
        std::vector<const char*> vulkan_device_layers_;
        std::vector<const char*> vulkan_device_extensions_;
        Penguin::Sample::Host_Synchronized<vk::Device> vulkan_logical_device_;
        Penguin::Sample::Host_Synchronized<vk::SwapchainKHR> vulkan_swapchain_;
        vk::Queue vulkan_graphics_queue_;
        Penguin::Sample::Host_Synchronized<vk::CommandPool> vulkan_command_pool_;
        Penguin::Sample::Host_Synchronized<std::vector<vk::CommandBuffer>> vulkan_command_buffers_;
    };
}


#endif // PENGUIN_SAMPLE_VULKAN_APPLICATION_H