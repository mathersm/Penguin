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
    struct Synchronized_Command_Pool
    {
        std::mutex mutex;
        vk::CommandPool command_pool;
        std::vector<vk::CommandBuffer> command_buffers;
    };


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
        void begin_vulkan_command_buffer_recording(size_t buffer_index);

        void create_presentation_surface(void);
        void create_vulkan_command_buffers(void);
        void create_vulkan_command_pool(void);
        void create_vulkan_graphics_queue(void);
        void create_vulkan_instance(void);
        void create_vulkan_logical_device(void);
        void create_vulkan_physical_device(void);
        void create_vulkan_render_pass(void);
        void create_vulkan_swapchain(void);

        void end_vulkan_command_buffer_recording(size_t buffer_index);

        uint32_t get_vulkan_swapchain_image_index(void);
        uint32_t get_vulkan_swapchain_image_count(void) const;

        void present(void);

        void set_vulkan_queue_family(void);

        std::vector<const char*>        validate_vulkan_device_extensions(const std::vector<const char*>& desired_extensions) const;
        std::vector<const char*>        validate_vulkan_device_layers(const std::vector<const char*>& desired_layers) const;
        std::vector<const char*>        validate_vulkan_instance_extensions(const std::vector<const char*>& desired_extensions) const;
        vk::PresentModeKHR              validate_vulkan_presentation_mode(const vk::PresentModeKHR& desired_mode) const;
        vk::SurfaceFormatKHR            validate_vulkan_swapchain_format(const vk::SurfaceFormatKHR& desired_format) const;
        vk::Extent2D                    validate_vulkan_swapchain_image_size(const vk::Extent2D& desired_image_size) const;
        vk::SurfaceTransformFlagBitsKHR validate_vulkan_swapchain_image_transform(const vk::SurfaceTransformFlagBitsKHR& desired_image_transform) const;
        vk::ImageUsageFlags             validate_vulkan_swapchain_image_usages(const vk::ImageUsageFlags& desired_image_usages) const;
        std::vector<const char*>        validate_vulkan_validation_layers(const std::vector<const char*>& desired_validation_layers) const;

    private:
        Penguin::Sample::Host_Synchronized<vk::Instance> vulkan_instance_;
        vk::PhysicalDevice vulkan_physical_device_;
        vk::SurfaceKHR presentation_surface_;
        uint32_t vulkan_queue_family_index_;
        Penguin::Sample::Host_Synchronized<vk::Device> vulkan_logical_device_;
        Penguin::Sample::Host_Synchronized<vk::SwapchainKHR> vulkan_swapchain_;
        std::vector<vk::Image> vulkan_swapchain_images_;
        uint32_t vulkan_swapchain_image_index_;
        vk::Semaphore vulkan_presentation_semaphore_;
        vk::Queue vulkan_graphics_queue_;
        Penguin::Sample::Synchronized_Command_Pool vulkan_command_pool_;
        vk::RenderPass  vulkan_render_pass_;
    };
}


#endif // PENGUIN_SAMPLE_VULKAN_APPLICATION_H