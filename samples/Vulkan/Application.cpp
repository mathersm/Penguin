/*
* Copyright (c) 2018 Michael Mathers
*/
#include "Application.h"
#include <sstream>
#include <iostream>


namespace
{
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    LRESULT CALLBACK window_event_handler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
#endif
}


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
        this->create_vulkan_instance();
        this->create_vulkan_physical_device();
        this->create_presentation_surface();
        this->set_vulkan_queue_family();
        this->create_vulkan_logical_device();
        this->create_vulkan_swapchain();
        this->create_vulkan_graphics_queue();
        this->create_vulkan_command_pool();
        this->create_vulkan_command_buffers();
    }


    void
    Application::create_presentation_surface(void)
    {
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        HINSTANCE instance_handle = GetModuleHandle(NULL);

        std::string window_class_name("Sample App Window");

        WNDCLASSEX window_class = {};
        window_class.cbSize = sizeof(WNDCLASSEX);
        window_class.lpfnWndProc = window_event_handler;
        window_class.hInstance = instance_handle;
        window_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        window_class.lpszClassName = window_class_name.c_str();
        
        if (!RegisterClassEx(&window_class))
        {
            std::stringstream error_stream;
            error_stream << "Failed to register window class (" << GetLastError() << ")";
            throw std::runtime_error(error_stream.str());
        }
        
        HWND window_handle = CreateWindowEx
        (
            0,
            window_class_name.c_str(),
            "Presentation Window",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            100, 100,
            800, 600,
            NULL,
            NULL,
            instance_handle,
            NULL
        );

        if (window_handle == 0)
        {
            std::stringstream error_stream;
            error_stream << "Failed to create window (" << GetLastError() << ")";
            throw std::runtime_error(error_stream.str());
        }

        vk::Win32SurfaceCreateInfoKHR surface_creation_info;
        surface_creation_info.hinstance = instance_handle;
        surface_creation_info.hwnd = window_handle;

        {
            std::lock_guard<std::mutex> instance_guard(this->vulkan_instance_.mutex);
            this->presentation_surface_ = this->vulkan_instance_.synced_object.createWin32SurfaceKHR(surface_creation_info);
        }

        ShowWindow(window_handle, SW_SHOW);

#else
#error("No implementation available other than for Windows")
#endif
    }


    void
    Application::create_vulkan_command_buffers(void)
    {
        /*
        Host access to command buffers must be externally synchronized.

        [Vulkan 1.1.88 Specification - Section 5.3 - Command Buffer Allocation and Management]
        */
        std::lock_guard<std::mutex> command_pool_guard(this->vulkan_command_pool_.mutex);

        vk::CommandBufferAllocateInfo buffer_allocation_info;
        buffer_allocation_info.commandBufferCount = 1;
        buffer_allocation_info.commandPool = this->vulkan_command_pool_.synced_object;
        buffer_allocation_info.level = vk::CommandBufferLevel::ePrimary;

        {
            std::scoped_lock guard(this->vulkan_logical_device_.mutex, this->vulkan_command_buffers_.mutex);
            this->vulkan_command_buffers_.synced_object = this->vulkan_logical_device_.synced_object.allocateCommandBuffers(buffer_allocation_info);
        }
    }


    void
    Application::create_vulkan_command_pool(void)
    {
        /*
        Command pools are opaque objects that command buffer memory is allocated
        from, and which allow the implementation to amortize the cost of resource
        creation across multiple command buffers. Command pools are externally
        synchronized, meaning that a command pool must not be used concurrently
        in multiple threads. That includes use via recording commands on any
        command buffers allocated from the pool, as well as operations that
        allocate, free, and reset command buffers or the pool itself.

        Host access to command pools must be externally synchronized.

        [Vulkan 1.1.88 Specification - Section 5.2 - Command Pools]
        */
        vk::CommandPoolCreateInfo pool_info;
        pool_info.queueFamilyIndex = this->vulkan_queue_family_index_;
        pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

        std::scoped_lock sync_guard(this->vulkan_logical_device_.mutex, this->vulkan_command_pool_.mutex);
        this->vulkan_command_pool_.synced_object = this->vulkan_logical_device_.synced_object.createCommandPool(pool_info);
    }


    void
    Application::create_vulkan_graphics_queue(void)
    {
        std::lock_guard<std::mutex> logical_device_guard(this->vulkan_logical_device_.mutex);
        this->vulkan_graphics_queue_ = this->vulkan_logical_device_.synced_object.getQueue(this->vulkan_queue_family_index_, 0);
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
        vk::ApplicationInfo app_info;
        app_info.apiVersion = VK_API_VERSION_1_1;
        app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        app_info.pApplicationName = application_name.c_str();
        app_info.pEngineName = engine_name.c_str();
        app_info.pNext = nullptr;

        auto validation_layers = this->validate_vulkan_validation_layers(desired_validation_layers);
        auto extensions = this->validate_vulkan_instance_extensions(desired_instance_extensions);

        vk::InstanceCreateInfo instance_info;
        instance_info.pNext = nullptr;
        instance_info.flags = vk::InstanceCreateFlags();
        instance_info.pApplicationInfo = &app_info;
        instance_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        instance_info.ppEnabledLayerNames = validation_layers.data();
        instance_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instance_info.ppEnabledExtensionNames = extensions.data();

        std::lock_guard<std::mutex> instance_guard(this->vulkan_instance_.mutex);
        this->vulkan_instance_.synced_object = vk::createInstance(instance_info);
    }


    void
    Application::create_vulkan_logical_device(void)
    {
        /*
        Creating a logical device also creates the queues associated with that
        device. The queues to create are described by a set of
        VkDeviceQueueCreateInfo structures that are passed to vkCreateDevice in
        pQueueCreateInfos.

        Host access to logical devices must be externally synchronized.

        [Vulkan 1.1 Specification - Section 4.3.2 - Queue Creation]
        */
        float queue_priority = 0.0f;
        vk::DeviceQueueCreateInfo queue_creation_info;
        queue_creation_info.pNext = nullptr;
        queue_creation_info.pQueuePriorities = &queue_priority;
        queue_creation_info.queueCount = 1;
        queue_creation_info.queueFamilyIndex = this->vulkan_queue_family_index_;

        auto device_layers = this->validate_vulkan_device_layers(desired_device_layers);
        auto device_extensions = this->validate_vulkan_device_extensions(desired_device_extensions);

        vk::DeviceCreateInfo device_creation_info;
        device_creation_info.enabledLayerCount = static_cast<uint32_t>(device_layers.size());
        device_creation_info.ppEnabledLayerNames = device_layers.data();
        device_creation_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        device_creation_info.ppEnabledExtensionNames = device_extensions.data();
        device_creation_info.queueCreateInfoCount = 1;
        device_creation_info.pQueueCreateInfos = &queue_creation_info;

        std::lock_guard<std::mutex> logical_device_guard(this->vulkan_logical_device_.mutex);
        this->vulkan_logical_device_.synced_object = this->vulkan_physical_device_.createDevice(device_creation_info);
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
            std::lock_guard<std::mutex> instance_guard(this->vulkan_instance_.mutex);
            physical_devices = this->vulkan_instance_.synced_object.enumeratePhysicalDevices();
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
    Application::create_vulkan_swapchain(void)
    {
        /*
        A swapchain is an abstraction for an array of presentable images that
        are associated with a surface. The presentable images are represented
        by VkImage objects created by the platform. One image (which can be an
        array image for multiview/stereoscopic-3D surfaces) is displayed at a
        time, but multiple images can be queued for presentation. An
        application renders to the image, and then queues the image for
        presentation to the surface.

        A native window cannot be associated with more than one non-retired
        swapchain at a time. Further, swapchains cannot be created for native
        windows that have a non-Vulkan graphics API surface associated with
        them.

        Note
        - The presentation engine is an abstraction for the platform’s
          compositor or display engine.
        - The presentation engine may be synchronous or asynchronous with
          respect to the application and/or logical device.
        - Some implementations may use the device’s graphics queue or dedicated
          presentation hardware to perform presentation.

        The presentable images of a swapchain are owned by the presentation
        engine. An application can acquire use of a presentable image from the
        presentation engine. Use of a presentable image must occur only after
        the image is returned by vkAcquireNextImageKHR, and before it is
        presented by vkQueuePresentKHR. This includes transitioning the image
        layout and rendering commands.

        An application can acquire use of a presentable image with
        vkAcquireNextImageKHR. After acquiring a presentable image and before
        modifying it, the application must use a synchronization primitive to
        ensure that the presentation engine has finished reading from the
        image. The application can then transition the image’s layout, queue
        rendering commands to it, etc. Finally, the application presents the
        image with vkQueuePresentKHR, which releases the acquisition of the
        image.

        The presentation engine controls the order in which presentable images
        are acquired for use by the application.

        Note
        - This allows the platform to handle situations which require
          out-of-order return of images after presentation. At the same time,
          it allows the application to generate command buffers referencing all
          of the images in the swapchain at initialization time, rather than in
          its main loop.

        Host access to swapchain must be externally synchronized.

        Vulkan 1.1 Specification - Section 31.8 - WSI Swapchain]
        */
        vk::SurfaceFormatKHR swapchain_format = this->validate_vulkan_swapchain_format(desired_format);

        vk::SwapchainCreateInfoKHR swapchain_creation_info;
        swapchain_creation_info.flags = vk::SwapchainCreateFlagsKHR();
        swapchain_creation_info.surface = this->presentation_surface_;
        swapchain_creation_info.minImageCount = this->get_vulkan_swapchain_image_count();
        swapchain_creation_info.imageFormat = swapchain_format.format;
        swapchain_creation_info.imageColorSpace = swapchain_format.colorSpace;
        swapchain_creation_info.imageExtent = this->validate_vulkan_swapchain_image_size(desired_image_size);
        swapchain_creation_info.imageArrayLayers = 1;
        swapchain_creation_info.imageUsage = this->validate_vulkan_swapchain_image_usages(desired_image_usages);
        swapchain_creation_info.imageSharingMode = vk::SharingMode::eExclusive;
        swapchain_creation_info.queueFamilyIndexCount = 0;
        swapchain_creation_info.pQueueFamilyIndices = nullptr;
        swapchain_creation_info.preTransform = this->validate_vulkan_swapchain_image_transform(desired_image_transform);
        swapchain_creation_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        swapchain_creation_info.presentMode = this->validate_vulkan_presentation_mode(vk::PresentModeKHR::eMailbox);
        swapchain_creation_info.clipped = VK_TRUE;
        
        {
            std::scoped_lock guard(this->vulkan_logical_device_.mutex, this->vulkan_swapchain_.mutex);
            swapchain_creation_info.oldSwapchain = this->vulkan_swapchain_.synced_object;

            this->vulkan_swapchain_.synced_object = this->vulkan_logical_device_.synced_object.createSwapchainKHR(swapchain_creation_info);
            this->vulkan_swapchain_images_ = this->vulkan_logical_device_.synced_object.getSwapchainImagesKHR(this->vulkan_swapchain_.synced_object);
        }

    }


    uint32_t
    Application::get_vulkan_swapchain_image_count(void) const
    {
        vk::SurfaceCapabilitiesKHR surface_capabilites = this->vulkan_physical_device_.getSurfaceCapabilitiesKHR(this->presentation_surface_);

        /*
        maxImageCount is the maximum number of images the specified device
        supports for a swapchain created for the surface, and will be either 0,
        or greater than or equal to minImageCount. A value of 0 means that
        there is no limit on the number of images, though there may be limits
        related to the total amount of memory used by presentable images.

        [Vulkan 1.1 Specification - Section 31.5 - Surface Queries]
        */
        uint32_t swapchain_image_count = surface_capabilites.minImageCount + 1;
        if (surface_capabilites.maxImageCount > 0)
        {
            swapchain_image_count = std::min<uint32_t>(swapchain_image_count, surface_capabilites.maxImageCount);
        }

        return swapchain_image_count;
    }


    void
    Application::run(void)
    {
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
                if (VK_TRUE == this->vulkan_physical_device_.getSurfaceSupportKHR(index, this->presentation_surface_))
                {
                    this->vulkan_queue_family_index_ = index;
                    return;
                }
            }
        }

        throw std::runtime_error("Unable to find suitable queue family");
    }


    std::vector<const char*>
    Application::validate_vulkan_device_extensions(const std::vector<const char*>& desired_extensions) const
    {
        auto available_device_extensions = this->vulkan_physical_device_.enumerateDeviceExtensionProperties();
        std::vector<const char*> enabled_extensions;

        for (const auto& extension : desired_extensions)
        {
            if (available_device_extensions.end() ==
                std::find_if(available_device_extensions.begin(), available_device_extensions.end(),
                    [extension](const auto& ext) { return (0 == std::strcmp(ext.extensionName, extension)); }))
            {
                std::cerr << "ERROR! Device extension (" << extension << ") is not available." << '\n';
            }
            else
            {
                enabled_extensions.push_back(extension);
            }
        }
        return enabled_extensions;
    }


    std::vector<const char*>
    Application::validate_vulkan_device_layers(const std::vector<const char*>& desired_layers) const
    {
        auto available_device_layers = this->vulkan_physical_device_.enumerateDeviceLayerProperties();
        std::vector<const char*> enabled_layers;

        for (const auto& layer : desired_layers)
        {
            if (available_device_layers.end() ==
                std::find_if(available_device_layers.begin(), available_device_layers.end(),
                    [layer](const auto& device_layer) { return (0 == std::strcmp(device_layer.layerName, layer)); }))
            {
                std::cerr << "ERROR! Device layer (" << layer << ") is not available." << '\n';
            }
            else
            {
                enabled_layers.push_back(layer);
            }
        }

        return enabled_layers;
    }


    std::vector<const char*>
    Application::validate_vulkan_instance_extensions(const std::vector<const char*>& desired_extensions) const
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
        auto available_extensions = vk::enumerateInstanceExtensionProperties();
        std::vector<const char*> enabled_extensions;

        for (const auto& extension : desired_extensions)
        {
            if (available_extensions.end() ==
                std::find_if(available_extensions.begin(), available_extensions.end(),
                    [extension](const auto& ext) { return (0 == std::strcmp(ext.extensionName, extension)); }))
            {
                std::cerr << "ERROR! Extension (" << extension << ") is not available." << '\n';
            }
            else
            {
                enabled_extensions.push_back(extension);
            }
        }

        return enabled_extensions;
    }


    vk::PresentModeKHR
    Application::validate_vulkan_presentation_mode(const vk::PresentModeKHR& desired_mode) const
    {
        std::vector<vk::PresentModeKHR> presentation_modes = this->vulkan_physical_device_.getSurfacePresentModesKHR(this->presentation_surface_);
        if (presentation_modes.end() == std::find(presentation_modes.begin(), presentation_modes.end(), desired_mode))
        {
            std::cerr << "ERROR! Unable to use presentation mode (" << vk::to_string(desired_mode) << "). Falling back to FIFO." << '\n';
            return vk::PresentModeKHR::eFifo;
        }
        else
        {
            return desired_mode;
        }
    }


    vk::SurfaceFormatKHR
    Application::validate_vulkan_swapchain_format(const vk::SurfaceFormatKHR& desired_format) const
    {
        auto supported_formats = this->vulkan_physical_device_.getSurfaceFormatsKHR(this->presentation_surface_);

        if (supported_formats.size() == 1 && supported_formats[0].format == vk::Format::eUndefined)
        {
            return desired_format;
        }

        auto find_result = std::find(supported_formats.begin(), supported_formats.end(), desired_format);
        if (find_result != supported_formats.end())
        {
            return desired_format;
        }

        auto find_alternative = std::find_if(supported_formats.begin(), supported_formats.end(), [desired_format](const auto& format) { return desired_format.format == format.format; });
        if (find_alternative != supported_formats.end())
        {
            std::cerr << "ERROR! Desired swapchain format is not available, selecting alternative colour space = " << vk::to_string(find_alternative->colorSpace) << '\n';
            return *find_alternative;
        }

        std::cerr << "ERROR! Desired swapchain format is not available, selecting alternative format = " << vk::to_string(supported_formats[0].format) << ":" << vk::to_string(supported_formats[0].colorSpace) << '\n';
        return supported_formats[0];
    }


    vk::Extent2D
    Application::validate_vulkan_swapchain_image_size(const vk::Extent2D& desired_image_size) const
    {
        vk::SurfaceCapabilitiesKHR surface_capabilites = this->vulkan_physical_device_.getSurfaceCapabilitiesKHR(this->presentation_surface_);
        /*
       currentExtent is the current width and height of the surface, or the
       special value (0xFFFFFFFF, 0xFFFFFFFF) indicating that the surface size
       will be determined by the extent of a swapchain targeting the surface.

       [Vulkan 1.1 Specification - Section 31.5 - Surface Queries]
       */
        vk::Extent2D swapchain_image_size = desired_image_size;

        if (surface_capabilites.currentExtent.width == 0xFFFFFFFF)
        {
            swapchain_image_size.width = std::clamp(swapchain_image_size.width, surface_capabilites.minImageExtent.width, surface_capabilites.maxImageExtent.width);
            swapchain_image_size.height = std::clamp(swapchain_image_size.height, surface_capabilites.minImageExtent.height, surface_capabilites.maxImageExtent.height);
        }
        else
        {
            swapchain_image_size = surface_capabilites.currentExtent;
        }

        return swapchain_image_size;
    }


    vk::SurfaceTransformFlagBitsKHR
    Application::validate_vulkan_swapchain_image_transform(const vk::SurfaceTransformFlagBitsKHR& desired_image_transform) const
    {
        vk::SurfaceCapabilitiesKHR surface_capabilites = this->vulkan_physical_device_.getSurfaceCapabilitiesKHR(this->presentation_surface_);

        vk::SurfaceTransformFlagsKHR swapchain_image_transform = desired_image_transform & surface_capabilites.supportedTransforms;
        if (vk::SurfaceTransformFlagsKHR(desired_image_transform) != swapchain_image_transform)
        {
            std::cerr << "ERROR! Swapchain does not support image transform = " << vk::to_string(desired_image_transform) << '\n';
            return surface_capabilites.currentTransform;
        }
        return desired_image_transform;
    }


    vk::ImageUsageFlags
    Application::validate_vulkan_swapchain_image_usages(const vk::ImageUsageFlags& desired_image_usages) const
    {
        vk::SurfaceCapabilitiesKHR surface_capabilites = this->vulkan_physical_device_.getSurfaceCapabilitiesKHR(this->presentation_surface_);

        vk::ImageUsageFlags swapchain_image_usages = desired_image_usages & surface_capabilites.supportedUsageFlags;
        if (desired_image_usages != swapchain_image_usages)
        {
            std::cerr << "ERROR! Swapchain does not support image usage = " << vk::to_string(desired_image_usages ^ swapchain_image_usages) << '\n';
        }
        return swapchain_image_usages;
    }


    std::vector<const char*>
    Application::validate_vulkan_validation_layers(const std::vector<const char*>& desired_validation_layers) const
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
        auto available_instance_layers = vk::enumerateInstanceLayerProperties();
        std::vector<const char*> enabled_validation_layers;

        for (const auto& layer : desired_validation_layers)
        {
            if (available_instance_layers.end() ==
                std::find_if(available_instance_layers.begin(), available_instance_layers.end(),
                    [layer](const auto& instance_layer) { return (0 == std::strcmp(instance_layer.layerName, layer)); }))
            {
                std::cerr << "ERROR: Validation layer (" << layer << ") is not available." << '\n';
            }
            else
            {
                enabled_validation_layers.push_back(layer);
            }
        }

        return enabled_validation_layers;
    }
}