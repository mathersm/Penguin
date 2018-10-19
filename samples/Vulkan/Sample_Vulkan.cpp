/*
* Copyright (c) 2018 Michael Mathers
*/
#include "Application.h"
#include <iomanip>
#include <iostream>


namespace
{
    void print_physical_device_information(VkPhysicalDevice* device_list, uint32_t device_count);

    void print_error(const std::system_error& error)
    {
        std::cerr << "ERROR (" << error.code().value() << ") - " << error.what() << '\n';
    }
}


namespace Penguin::Vulkan
{
    auto default_selection_algorithm = [](const std::vector<vk::PhysicalDevice>& physical_devices)
    {
        for (auto device : physical_devices)
        {
            // Simply return the first device available
            return device;
        }
        throw std::runtime_error("Failed to find suitable physical device");
    };


    template <typename T>
    vk::PhysicalDevice select_physical_device(const vk::Instance& instance, T&& selection_algorithm)
    {
        return selection_algorithm(instance.enumeratePhysicalDevices());
    }
}


static VKAPI_ATTR vk::Bool32 VKAPI_CALL vulkan_debug_callback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
    vk::DebugUtilsMessageTypeFlagsEXT message_type,
    const vk::DebugUtilsMessengerCallbackDataEXT* callback_data_ptr,
    void* user_data_ptr)
{
    std::cerr << "Validation layer: ";

    switch (message_severity)
    {
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
        std::cerr << "(ERROR) ";
        break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
        std::cerr << "(WARNING) ";
        break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
        std::cerr << "(INFO) ";
        break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
        std::cerr << "(VERBOSE) ";
        break;
    default:
        break;
    }

    std::cerr << callback_data_ptr->pMessage << '\n';
    return VK_FALSE;
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
        std::cerr << "ERROR: std::system_error (" << sys_error.code().value() << ") - " << sys_error.what() << '\n';
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