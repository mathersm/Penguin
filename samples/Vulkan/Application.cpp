/*
* Copyright (c) 2018 Michael Mathers
*/
#include "Application.h"
#include <sstream>
#include <iostream>


namespace Penguin::Sample
{
    Application::Application(void)
    {
    }


    Application::~Application(void)
    {
    }


    void
    Application::configure(void)
    {
        this->set_vulkan_validation_layers();
        this->set_vulkan_instance_extensions();
        this->create_vulkan_instance();
        this->create_vulkan_physical_device();
        this->set_vulkan_queue_family();
        this->set_vulkan_device_layers();
        this->set_vulkan_device_extensions();
        this->create_vulkan_logical_device();
    }


    void
    Application::create_vulkan_instance(void)
    {
        /*
        There is no global state in Vulkan. State is stored on a per-application
        basis within a VkInstance object. Creation of VkInstance object also
        initialises the Vulkan library.

        Host access to instance must be externally synchronized.

        [Vulkan 1.0 Specification - Section 3.2 - Instances]
        */
        const std::string application_name("Sample Application");
        const std::string engine_name("Sample Engine");

        vk::ApplicationInfo app_info;
        app_info.apiVersion = VK_API_VERSION_1_1;
        app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        app_info.pApplicationName = application_name.c_str();
        app_info.pEngineName = engine_name.c_str();
        app_info.pNext = nullptr;

        vk::InstanceCreateInfo instance_info;
        instance_info.pNext = nullptr;
        instance_info.flags = vk::InstanceCreateFlags();
        instance_info.pApplicationInfo = &app_info;
        instance_info.enabledLayerCount = static_cast<uint32_t>(this->vulkan_validation_layers_.size());
        instance_info.ppEnabledLayerNames = this->vulkan_validation_layers_.data();
        instance_info.enabledExtensionCount = static_cast<uint32_t>(this->vulkan_instance_extensions_.size());
        instance_info.ppEnabledExtensionNames = this->vulkan_instance_extensions_.data();

        std::lock_guard<std::mutex> instance_guard(this->vulkan_instance_mutex_);
        this->vulkan_instance_ = vk::createInstance(instance_info);
    }


    void
    Application::create_vulkan_logical_device(void)
    {
        /*
        Creating a logical device also creates the queues associated with that
        device. The queues to create are described by a set of
        VkDeviceQueueCreateInfo structures that are passed to vkCreateDevice in
        pQueueCreateInfos.

        [Vulkan 1.1 Specification - Section 4.3.2 - Queue Creation]
        */
        float queue_priority = 0.0f;
        vk::DeviceQueueCreateInfo queue_creation_info;
        queue_creation_info.pNext = nullptr;
        queue_creation_info.pQueuePriorities = &queue_priority;
        queue_creation_info.queueCount = 1;
        queue_creation_info.queueFamilyIndex = this->vulkan_queue_family_index_;

        vk::DeviceCreateInfo device_creation_info;
        device_creation_info.enabledLayerCount = static_cast<uint32_t>(this->vulkan_device_layers_.size());
        device_creation_info.ppEnabledLayerNames = this->vulkan_device_layers_.data();
        device_creation_info.enabledExtensionCount = static_cast<uint32_t>(this->vulkan_device_extensions_.size());
        device_creation_info.ppEnabledExtensionNames = vulkan_device_extensions_.data();
        device_creation_info.queueCreateInfoCount = 1;
        device_creation_info.pQueueCreateInfos = &queue_creation_info;

        this->vulkan_logical_device_ = this->vulkan_physical_device_.createDevice(device_creation_info);
    }


    void
    Application::create_vulkan_physical_device(void)
    {
        /*
        Vulkan separates the concept of physical and logical devices. A physical
        device represents a device in a system (maybe several pieces of hardware
        working together). A logical device represents an application view of a
        device.

        [Vulkan 1.0 Specification - Section 4.1 - Physical Devices]
        */

        std::vector<vk::PhysicalDevice> physical_devices;
        {
            std::lock_guard<std::mutex> instance_guard(this->vulkan_instance_mutex_);
            physical_devices = this->vulkan_instance_.enumeratePhysicalDevices();
        }

        for (const auto& device : physical_devices)
        {
            /*
            There are a variety of implementation-dependent limits.

            The VkPhysicalDeviceLimits are properties of the physical device. These
            are available in the limits member of the VkPhysicalDeviceProperties
            structure which is returned from vkGetPhysicalDeviceProperties.

            [Vulkan 1.1 Specification - Section 34.2 - Limits]
            */
            auto device_properties = device.getProperties();

            /*
            The Specification defines a set of optional features that may be
            supported by a Vulkan implementation. Support for features is reported
            and enabled on a per-feature basis. Features are properties of the
            physical device.

            [Vulkan 1.1 Specification - Section 34.1 - Features]
            */
            auto device_features = device.getFeatures();

            if (device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                this->vulkan_physical_device_ = device;
                return;
            }
        }

        throw std::runtime_error("No suitable physical device found");
    }


    void
    Application::run(void)
    {
    }


    void
    Application::set_vulkan_device_extensions(void)
    {
        std::cout << "No device extensions are being set" << '\n';

        auto available_device_extensions = this->vulkan_physical_device_.enumerateDeviceExtensionProperties();

        for (const auto& extension : this->vulkan_device_extensions_)
        {
            if (available_device_extensions.end() ==
                std::find_if(available_device_extensions.begin(), available_device_extensions.end(),
                    [extension](const auto& ext) { return (0 == std::strcmp(ext.extensionName, extension)); }))
            {
                std::stringstream error_stream;
                error_stream << "Device extension: " << extension << " is not available";
                throw std::runtime_error(error_stream.str().c_str());
            }
        }
    }


    void
    Application::set_vulkan_device_layers(void)
    {
        std::cout << "No device layers are being set" << '\n';

        auto available_device_layers = this->vulkan_physical_device_.enumerateDeviceLayerProperties();

        for (const auto& layer : this->vulkan_device_layers_)
        {
            if (available_device_layers.end() ==
                std::find_if(available_device_layers.begin(), available_device_layers.end(),
                    [layer](const auto& device_layer) { return (0 == std::strcmp(device_layer.layerName, layer)); }))
            {
                std::stringstream error_stream;
                error_stream << "Device layer: " << layer << " is not available";
                throw std::runtime_error(error_stream.str().c_str());
            }
        }
    }


    void
    Application::set_vulkan_instance_extensions(void)
    {
        /*
        Extensions may define new Vulkan commands, structures, and enumerants.
        For compilation purposes, the interfaces defined by registered extensions,
        including new structures and enumerants as well as function pointer types
        for new commands, are defined in the Khronos-supplied vulkan_core.h
        together with the core API. However, commands defined by extensions may not
        be available for static linking - in which case function pointers to these
        commands should be queried at runtime as described in Command Function
        Pointers. Extensions may be provided by layers as well as by a Vulkan
        implementation.

        [Vulkan 1.1 Specification - Section 33.2 - Extensions]
         */

        this->vulkan_instance_extensions_.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    
        auto available_extensions = vk::enumerateInstanceExtensionProperties();

        for (const auto& extension : this->vulkan_instance_extensions_)
        {
            if (available_extensions.end() ==
                std::find_if(available_extensions.begin(), available_extensions.end(),
                    [extension](const auto& ext) { return (0 == std::strcmp(ext.extensionName, extension)); }))
            {
                std::stringstream error_stream;
                error_stream << "Extension: " << extension << " is not available";
                throw std::runtime_error(error_stream.str().c_str());
            }
        }
    }


    void
    Application::set_vulkan_queue_family(void)
    {
        /*
        As discussed in the Physical Device Enumeration section above, the
        vkGetPhysicalDeviceQueueFamilyProperties command is used to retrieve
        details about the queue families and queues supported by a device.

        Each index in the pQueueFamilyProperties array returned by
        vkGetPhysicalDeviceQueueFamilyProperties describes a unique queue
        family on that physical device. These indices are used when creating
        queues, and they correspond directly with the queueFamilyIndex that is
        passed to the vkCreateDevice command via the VkDeviceQueueCreateInfo
        structure as described in the Queue Creation section below.

        Grouping of queue families within a physical device is
        implementation-dependent.

        [Vulkan 1.1 Specification - Section 4.3.1 - Queue Family Properties]
        */
        auto queue_family_properties = this->vulkan_physical_device_.getQueueFamilyProperties();

        for (uint32_t index = 0; index < queue_family_properties.size(); ++index)
        {
            if ((queue_family_properties[index].queueFlags & vk::QueueFlagBits::eGraphics) &&
                queue_family_properties[index].queueCount > 0)
            {
                this->vulkan_queue_family_index_ = index;
                return;
            }
        }

        throw std::runtime_error("Unable to find suitable queue family");
    }


    void
    Application::set_vulkan_validation_layers(void)
    {
        /*
        When a layer is enabled, it inserts itself into the call chain for Vulkan
        commands the layer is interested in. A common use of layers is to validate
        application behavior during development. For example, the implementation
        will not check that Vulkan enums used by the application fall within
        allowed ranges. Instead, a validation layer would do those checks and flag
        issues. This avoids a performance penalty during production use of the
        application because those layers would not be enabled in production.

        Vulkan layers may wrap object handles (i.e. return a different handle value
        to the application than that generated by the implementation). This is
        generally discouraged, as it increases the probability of incompatibilities
        with new extensions. The validation layers wrap handles in order to track
        the proper use and destruction of each object. See the “Vulkan Loader
        Specification and Architecture Overview” document for additional
        information.

        [Vulkan 1.1 Specification - Section 33.1 - Layers]
        */

        this->vulkan_validation_layers_ =
        {
            "VK_LAYER_LUNARG_standard_validation"
        };

        auto available_instance_layers = vk::enumerateInstanceLayerProperties();

        for (const auto& layer : this->vulkan_validation_layers_)
        {
            if (available_instance_layers.end() ==
                std::find_if(available_instance_layers.begin(), available_instance_layers.end(),
                    [layer](const auto& instance_layer) { return (0 == std::strcmp(instance_layer.layerName, layer)); }))
            {
                std::stringstream error_stream;
                error_stream << "Validation layer: " << layer << " is not available";
                throw std::runtime_error(error_stream.str().c_str());
            }
        }
    }
}