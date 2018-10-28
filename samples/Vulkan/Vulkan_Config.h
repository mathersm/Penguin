/*
* Copyright (c) 2018 Michael Mathers
*/
#ifndef PENGUIN_SAMPLE_VULKAN_CONFIG_H
#define PENGUIN_SAMPLE_VULKAN_CONFIG_H


#if defined(_MSC_VER)
# define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__GNUG__)
# define VK_USE_PLATFORM_XLIB_KHR
#else
# error("Unable to determine target platform")
#endif


#include <vulkan/vulkan.hpp>


namespace Penguin::Sample
{
    const std::string application_name("Sample Application");
    const std::string engine_name("Sample Engine");

    const std::vector<const char*> desired_validation_layers =
    {
        "VK_LAYER_LUNARG_standard_validation"
    };


    const std::vector<const char*> desired_instance_extensions =
    {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
        VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XCB_KHR)
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#else
# error("Unknown Window System Integration(WSI)")
#endif
    };


    const std::vector<const char*> desired_device_layers = 
    {
    };


    const std::vector<const char*> desired_device_extensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };



    const vk::SurfaceFormatKHR desired_format = 
    {
        vk::Format::eR8G8B8A8Unorm,
        vk::ColorSpaceKHR::eSrgbNonlinear
    };

    
    const vk::Extent2D desired_image_size =
    {
        800,
        600
    };


    const vk::ImageUsageFlags desired_image_usages =
        vk::ImageUsageFlagBits::eColorAttachment;
    
    
    const vk::SurfaceTransformFlagBitsKHR desired_image_transform =
        vk::SurfaceTransformFlagBitsKHR::eIdentity;
}


#endif // PENGUIN_SAMPLE_VULKAN_CONFIG_H
