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


#endif // PENGUIN_SAMPLE_VULKAN_CONFIG_H
