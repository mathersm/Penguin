/*
* Copyright (c) 2018 Michael Mathers
*/
#include <vulkan/vulkan.hpp>
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
    the proper use and destruction of each object. See the �Vulkan Loader
    Specification and Architecture Overview� document for additional
    information.

    [Vulkan 1.1 Specification - Section 33.1 - Layers]
    */
    auto available_instance_layers = vk::enumerateInstanceLayerProperties();
    std::cout << "Supported layers" << '\n';
    for (const auto& layers : available_instance_layers)
    {
        std::cout << "\t" << layers.layerName << " - " << layers.description << '\n';
    }

#if !defined(NDEBUG)
    std::vector<const char*> validation_layers =
    {
        "VK_LAYER_LUNARG_standard_validation"
    };

    for (const auto& layer : validation_layers)
    {
        if (available_instance_layers.end() ==
            std::find_if(available_instance_layers.begin(), available_instance_layers.end(),
                [layer](const auto& instance_layer) { return (0 == std::strcmp(instance_layer.layerName, layer)); }))
        {
            std::cerr << "ERROR! " << layer << " is not available" << '\n';
            return -1;
        }
    }
#endif

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
    // Query supported extensions
    auto available_instance_extentions = vk::enumerateInstanceExtensionProperties();
    std::cout << "Supported extensions" << '\n';
    for (const auto& ext : available_instance_extentions)
    {
        std::cout << "\t" << ext.extensionName << '\n';
    }

#if !defined(NDEBUG)
    std::vector<const char*> extensions;
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    


    /*
    There is no global state in Vulkan. State is stored on a per-application
    basis within a VkInstance object. Creation of VkInstance object also
    initialises the Vulkan library.

    Host access to instance must be externally synchronized.

    [Vulkan 1.0 Specification - Section 3.2 - Instances]
    */
    vk::Instance instance;

    std::string application_name("Vulkan Sample App");
    std::string engine_name("Vulkan Sample Engine");

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
#ifdef NDEBUG
    instance_info.enabledLayerCount = 0;
    instance_info.ppEnabledLayerNames = nullptr;
#else
    instance_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
    instance_info.ppEnabledLayerNames = validation_layers.data();
#endif
    instance_info.enabledExtensionCount = 0;
    instance_info.ppEnabledExtensionNames = nullptr;
 
    try
    {
        instance = vk::createInstance(instance_info);
    }
    catch (const std::system_error& error)
    {
        print_error(error);
        return error.code().value();
    }

    
    /*
    Vulkan separates the concept of physical and logical devices. A physical
    device represents a device in a system (maybe several pieces of hardware
    working together). A logical device represents an application view of a
    device.

    [Vulkan 1.0 Specification - Section 4.1 - Physical Devices]
    */
    try
    {
        auto physical_devices = instance.enumeratePhysicalDevices();
        std::cout << "Found " << physical_devices.size() << " physical device(s)" << '\n';

        for (const vk::PhysicalDevice& pd : physical_devices)
        {
            vk::PhysicalDeviceProperties pd_properties = pd.getProperties();
            std::cout << "\t" << pd_properties.deviceName << '\n';
            
           auto q_properties = pd.getQueueFamilyProperties();
           
           for (const auto& qp : q_properties)
           {
               std::cout << "\t\t" << "Queues available: " << qp.queueCount << '\n';
               std::cout << "\t\t" << "Queue types: " << vk::to_string(qp.queueFlags).c_str() << '\n';
           }
        }


        /*
        It is possible to form logical devices from multiple physical devices.
        This is relevant in the case of SLI or Crossfire configurations.
        */
        auto physical_device_groups = instance.enumeratePhysicalDeviceGroups();
        std::cout << "Found " << physical_device_groups.size() << " physical device group(s)" << '\n';

        for (const auto& pdg : physical_device_groups)
        {
            std::cout << "\t" << "Physical device group contains " << pdg.physicalDeviceCount << " device(s)" << '\n';
            for (size_t i = 0; i < pdg.physicalDeviceCount; ++i)
            {
                vk::PhysicalDeviceProperties pd_properties = pdg.physicalDevices[i].getProperties();
                std::cout << "\t\t" << pd_properties.deviceName << '\n';
            }
        }

        
        // Select a physical device to use
        auto physical_device = Penguin::Vulkan::select_physical_device(instance, Penguin::Vulkan::default_selection_algorithm);

        /*
        The Specification defines a set of optional features that may be
        supported by a Vulkan implementation. Support for features is reported
        and enabled on a per-feature basis. Features are properties of the
        physical device.

        [Vulkan 1.1 Specification - Section 34.1 - Features]
        */
        vk::PhysicalDeviceFeatures physical_device_features = physical_device.getFeatures();

        /*
        There are a variety of implementation-dependent limits.

        The VkPhysicalDeviceLimits are properties of the physical device. These
        are available in the limits member of the VkPhysicalDeviceProperties
        structure which is returned from vkGetPhysicalDeviceProperties.

        [Vulkan 1.1 Specification - Section 34.2 - Limits]
        */
        vk::PhysicalDeviceProperties physical_device_properties = physical_device.getProperties();

        auto available_device_extensions = physical_device.enumerateDeviceExtensionProperties();
        std::cout << "Available physical device extensions" << '\n';
        for (const auto& ext : available_device_extensions)
        {
            std::cout << "\t" << ext.extensionName << '\n';
        }

        auto available_device_layers = physical_device.enumerateDeviceLayerProperties();
        std::cout << "Available physical device extensions" << '\n';
        for (const auto& layers : available_device_layers)
        {
            std::cout << "\t" << layers.layerName << " - " << layers.description << '\n';
        }

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
        auto queue_family_properties = physical_device.getQueueFamilyProperties();
        for (const auto& queue_family : queue_family_properties)
        {
            std::cout << "Physical device queue family has "
                      << queue_family.queueCount
                      << " queues "
                      << vk::to_string(queue_family.queueFlags).c_str()
                      << '\n';
        }

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
        queue_creation_info.queueFamilyIndex = 1;

        // Create a logical device
        vk::DeviceCreateInfo device_creation_info;
        device_creation_info.enabledExtensionCount = 0;
        device_creation_info.ppEnabledExtensionNames = nullptr;
        device_creation_info.queueCreateInfoCount = 1;
        device_creation_info.pQueueCreateInfos = &queue_creation_info;

        auto logical_device = physical_device.createDevice(device_creation_info);

    }
    catch (const std::system_error& error)
    {
        print_error(error);
        return error.code().value();
    }


    /*
    print_physical_device_information(physical_device_list, physical_device_count);
    */
    
    instance.destroy();
    return 0;
}



//namespace
//{
//    void print_physical_device_information(VkPhysicalDevice* device_list, uint32_t device_count)
//    {
//        std::cout << "Printing information for " << device_count << " physical devices" << '\n';
//        for (uint32_t i = 0; i < device_count; ++i)
//        {
//            VkPhysicalDeviceProperties device_properties;
//            vkGetPhysicalDeviceProperties(device_list[i], &device_properties);
//
//            std::cout << "Physical device " << i << ":" << '\n';
//            std::cout << std::setw(48) << "API version: " << device_properties.apiVersion << '\n';
//            std::cout << std::setw(48) << "Device ID: " << device_properties.deviceID << '\n';
//            std::cout << std::setw(48) << "Device name: " << device_properties.deviceName << '\n';
//            std::cout << std::setw(48) << "Device type: ";
//            switch (device_properties.deviceType)
//            {
//            case VK_PHYSICAL_DEVICE_TYPE_CPU:
//                std::cout << "CPU" << '\n';
//                break;
//            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
//                std::cout << "Discrete GPU" << '\n';
//                break;
//            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
//                std::cout << "Integrated GPU" << '\n';
//                break;
//            case VK_PHYSICAL_DEVICE_TYPE_OTHER:
//                std::cout << "Other" << '\n';
//                break;
//            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
//                std::cout << "Virtual GPU" << '\n';
//                break;
//            default:
//                std::cerr << "Unknown" << '\n';
//                break;
//            }
//
//            std::cout << std::setw(48) << "Driver version: " << device_properties.driverVersion << '\n';
//            std::cout << std::setw(48) << "Vendor ID: " << device_properties.vendorID << '\n';
//
//            std::cout << std::setw(48) << "Buffer image granularity: " << std::dec << device_properties.limits.bufferImageGranularity << '\n';
//            std::cout << std::setw(48) << "Discrete queue priorities: " << device_properties.limits.discreteQueuePriorities << '\n';
//            std::cout << std::setw(48) << "Framebuffer colour sample counts: 0x" << std::hex << device_properties.limits.framebufferColorSampleCounts << '\n';
//            std::cout << std::setw(48) << "Framebuffer depth sample counts: 0x" << std::hex << device_properties.limits.framebufferDepthSampleCounts << '\n';
//            std::cout << std::setw(48) << "Framebuffer no attachment sample counts: 0x" << std::hex << device_properties.limits.framebufferNoAttachmentsSampleCounts << '\n';
//            std::cout << std::setw(48) << "Framebuffer stencil sample counts: 0x" << std::hex << device_properties.limits.framebufferStencilSampleCounts << '\n';
//            std::cout << std::setw(48) << "Line width granularity: " << std::dec << device_properties.limits.lineWidthGranularity << '\n';
//            std::cout << std::setw(48) << "Line width range: [" << device_properties.limits.lineWidthRange[0] << ", " << device_properties.limits.lineWidthRange[1] << "]" << '\n';
//            std::cout << std::setw(48) << "Max bound descriptor sets: " << device_properties.limits.maxBoundDescriptorSets << '\n';
//            std::cout << std::setw(48) << "Max clip distances: " << device_properties.limits.maxClipDistances << '\n';
//            std::cout << std::setw(48) << "Max colour attachments: " << device_properties.limits.maxColorAttachments << '\n';
//            std::cout << std::setw(48) << "Max combined clip and cull distances: " << device_properties.limits.maxCombinedClipAndCullDistances << '\n';
//            std::cout << std::setw(48) << "Max compute shader memory size: " << device_properties.limits.maxComputeSharedMemorySize << '\n';
//            std::cout << std::setw(48) << "Max compute work group count: [" << device_properties.limits.maxComputeWorkGroupCount[0] << ", " << device_properties.limits.maxComputeWorkGroupCount[1] << ", " << device_properties.limits.maxComputeWorkGroupCount[2] << "]" << '\n';
//            std::cout << std::setw(48) << "Max compute work group invocations: " << device_properties.limits.maxComputeWorkGroupInvocations << '\n';
//            std::cout << std::setw(48) << "Max compute work group size: [" << device_properties.limits.maxComputeWorkGroupSize[0] << ", " << device_properties.limits.maxComputeWorkGroupSize[1] << ", " << device_properties.limits.maxComputeWorkGroupSize[2] << "]" << '\n';
//            std::cout << std::setw(48) << "Max cull distances: " << device_properties.limits.maxCullDistances << '\n';
//            std::cout << std::setw(48) << "Max descriptor set input attachments: " << device_properties.limits.maxDescriptorSetInputAttachments << '\n';
//            std::cout << std::setw(48) << "Max descriptor set sampled images: " << device_properties.limits.maxDescriptorSetSampledImages << '\n';
//            std::cout << std::setw(48) << "Max descriptor set samplers: " << device_properties.limits.maxDescriptorSetSamplers << '\n';
//            std::cout << std::setw(48) << "Max descriptor set storage buffers: " << device_properties.limits.maxDescriptorSetStorageBuffers << '\n';
//            std::cout << std::setw(48) << "Max descriptor set dynamic storage buffers : " << device_properties.limits.maxDescriptorSetStorageBuffersDynamic << '\n';
//            std::cout << std::setw(48) << "Max descriptor set storage images: " << device_properties.limits.maxDescriptorSetStorageImages << '\n';
//            std::cout << std::setw(48) << "Max descriptor set uniform buffers: " << device_properties.limits.maxDescriptorSetUniformBuffers << '\n';
//            std::cout << std::setw(48) << "Max descriptor set dynamic uniform buffers: " << device_properties.limits.maxDescriptorSetUniformBuffersDynamic << '\n';
//            std::cout << std::setw(48) << "Max draw indexed index value: " << device_properties.limits.maxDrawIndexedIndexValue << '\n';
//            std::cout << std::setw(48) << "Max draw indirect count: " << device_properties.limits.maxDrawIndirectCount << '\n';
//            std::cout << std::setw(48) << "Max fragment combined output resources: " << device_properties.limits.maxFragmentCombinedOutputResources << '\n';
//            std::cout << std::setw(48) << "Max fragment dual source attachments: " << device_properties.limits.maxFragmentDualSrcAttachments << '\n';
//            std::cout << std::setw(48) << "Max fragment input components: " << device_properties.limits.maxFragmentInputComponents << '\n';
//            std::cout << std::setw(48) << "Max fragment output attachments: " << device_properties.limits.maxFragmentOutputAttachments << '\n';
//            std::cout << std::setw(48) << "Max framebuffer width: " << device_properties.limits.maxFramebufferWidth << '\n';
//            std::cout << std::setw(48) << "Max framebuffer height: " << device_properties.limits.maxFramebufferHeight << '\n';
//            std::cout << std::setw(48) << "Max framebuffer layers: " << device_properties.limits.maxFramebufferLayers << '\n';
//            std::cout << std::setw(48) << "Max geometry input components: " << device_properties.limits.maxGeometryInputComponents << '\n';
//            std::cout << std::setw(48) << "Max geometry output components: " << device_properties.limits.maxGeometryOutputComponents << '\n';
//            std::cout << std::setw(48) << "Max geometry output vertices: " << device_properties.limits.maxGeometryOutputVertices << '\n';
//            std::cout << std::setw(48) << "Max geometry shader invocations: " << device_properties.limits.maxGeometryShaderInvocations << '\n';
//            std::cout << std::setw(48) << "Max geometry total output components: " << device_properties.limits.maxGeometryTotalOutputComponents << '\n';
//            std::cout << std::setw(48) << "Make image array layers: " << device_properties.limits.maxImageArrayLayers << '\n';
//            std::cout << std::setw(48) << "Make image dimension 1D: " << device_properties.limits.maxImageDimension1D << '\n';
//            std::cout << std::setw(48) << "Make image dimension 2D: " << device_properties.limits.maxImageDimension2D << '\n';
//            std::cout << std::setw(48) << "Make image dimension 3D: " << device_properties.limits.maxImageDimension3D << '\n';
//            std::cout << std::setw(48) << "Make image dimension cube: " << device_properties.limits.maxImageDimensionCube << '\n';
//            std::cout << std::setw(48) << "Make interpolation offset: " << device_properties.limits.maxInterpolationOffset << '\n';
//            std::cout << std::setw(48) << "Make memory allocation count: " << device_properties.limits.maxMemoryAllocationCount << '\n';
//            std::cout << std::setw(48) << "Max per stage descriptor input attachments: " << device_properties.limits.maxPerStageDescriptorInputAttachments << '\n';
//            std::cout << std::setw(48) << "Max per stage descriptor sampled images: " << device_properties.limits.maxPerStageDescriptorSampledImages << '\n';
//            std::cout << std::setw(48) << "Max per stage descriptor samplers: " << device_properties.limits.maxPerStageDescriptorSamplers << '\n';
//            std::cout << std::setw(48) << "Max per stage descriptor storage buffers: " << device_properties.limits.maxPerStageDescriptorStorageBuffers << '\n';
//            std::cout << std::setw(48) << "Max per stage descriptor storage images: " << device_properties.limits.maxPerStageDescriptorStorageImages << '\n';
//            std::cout << std::setw(48) << "Max per stage descriptor uniform buffers: " << device_properties.limits.maxPerStageDescriptorUniformBuffers << '\n';
//            std::cout << std::setw(48) << "Max per stage resources: " << device_properties.limits.maxPerStageResources << '\n';
//            std::cout << std::setw(48) << "Max push constants size: " << device_properties.limits.maxPushConstantsSize << '\n';
//            std::cout << std::setw(48) << "Max sampler mask words: " << device_properties.limits.maxSampleMaskWords << '\n';
//            std::cout << std::setw(48) << "Max sampler allocation count: " << device_properties.limits.maxSamplerAllocationCount << '\n';
//            std::cout << std::setw(48) << "Max sampler anisotropy: " << device_properties.limits.maxSamplerAnisotropy << '\n';
//            std::cout << std::setw(48) << "Max sampler LOD bias: " << device_properties.limits.maxSamplerLodBias << '\n';
//            std::cout << std::setw(48) << "Max storage buffer range: " << device_properties.limits.maxStorageBufferRange << '\n';
//            std::cout << std::setw(48) << "Max tessellation control per patch output components: " << device_properties.limits.maxTessellationControlPerPatchOutputComponents << '\n';
//            std::cout << std::setw(48) << "Max tessellation control per vertex input components: " << device_properties.limits.maxTessellationControlPerVertexInputComponents << '\n';
//            std::cout << std::setw(48) << "Max tessellation control per vertex output components: " << device_properties.limits.maxTessellationControlPerVertexOutputComponents << '\n';
//            std::cout << std::setw(48) << "Max tessellation control total output components: " << device_properties.limits.maxTessellationControlTotalOutputComponents << '\n';
//            std::cout << std::setw(48) << "Max tessellation evaluation input components: " << device_properties.limits.maxTessellationEvaluationInputComponents << '\n';
//            std::cout << std::setw(48) << "Max tessellation evaluation output components: " << device_properties.limits.maxTessellationEvaluationOutputComponents << '\n';
//            std::cout << std::setw(48) << "Max tessellation general level: " << device_properties.limits.maxTessellationGenerationLevel << '\n';
//            std::cout << std::setw(48) << "Max tessellation patch size: " << device_properties.limits.maxTessellationPatchSize << '\n';
//            std::cout << std::setw(48) << "Max texel buffer elements: " << device_properties.limits.maxTexelBufferElements << '\n';
//            std::cout << std::setw(48) << "Max texel gather offset: " << device_properties.limits.maxTexelGatherOffset << '\n';
//            std::cout << std::setw(48) << "Max texel offset: " << device_properties.limits.maxTexelOffset << '\n';
//            std::cout << std::setw(48) << "Max vertex input attributes: " << device_properties.limits.maxVertexInputAttributes << '\n';
//            std::cout << std::setw(48) << "Max vertex input attribute offset: " << device_properties.limits.maxVertexInputAttributeOffset << '\n';
//            std::cout << std::setw(48) << "Max vertex input bindings: " << device_properties.limits.maxVertexInputBindings << '\n';
//            std::cout << std::setw(48) << "Max vertex input binding stride: " << device_properties.limits.maxVertexInputBindingStride << '\n';
//            std::cout << std::setw(48) << "Max vertex output components: " << device_properties.limits.maxVertexOutputComponents << '\n';
//            std::cout << std::setw(48) << "Max viewport dimensions: [" << device_properties.limits.maxViewportDimensions[0] << ", " << device_properties.limits.maxViewportDimensions[1] << "]" << '\n';
//            std::cout << std::setw(48) << "Max viewports: " << device_properties.limits.maxViewports << '\n';
//            std::cout << std::setw(48) << "Min interpolation offset: " << device_properties.limits.minInterpolationOffset << '\n';
//            std::cout << std::setw(48) << "Min memory map alignment: " << device_properties.limits.minMemoryMapAlignment << '\n';
//            std::cout << std::setw(48) << "Min storage offset alignment offset: " << device_properties.limits.minStorageBufferOffsetAlignment << '\n';
//            std::cout << std::setw(48) << "Min texel buffer alignment offset: " << device_properties.limits.minTexelBufferOffsetAlignment << '\n';
//            std::cout << std::setw(48) << "Min texel gather offset: " << device_properties.limits.minTexelGatherOffset << '\n';
//            std::cout << std::setw(48) << "Min uniform buffer alignment offset: " << device_properties.limits.minUniformBufferOffsetAlignment << '\n';
//            std::cout << std::setw(48) << "Mipmap precision bits: " << device_properties.limits.mipmapPrecisionBits << '\n';
//            std::cout << std::setw(48) << "Non-coherent atom size: " << device_properties.limits.nonCoherentAtomSize << '\n';
//            std::cout << std::setw(48) << "Optimal buffer copy offset alignment: " << device_properties.limits.optimalBufferCopyOffsetAlignment << '\n';
//            std::cout << std::setw(48) << "Optimal buffer copy row pitch alignment: " << device_properties.limits.optimalBufferCopyRowPitchAlignment << '\n';
//            std::cout << std::setw(48) << "Point size granularity: " << device_properties.limits.pointSizeGranularity << '\n';
//            std::cout << std::setw(48) << "Point size range: [" << device_properties.limits.pointSizeRange[0] << ", " << device_properties.limits.pointSizeRange[1] << "]" << '\n';
//            std::cout << std::setw(48) << "Sampled image colour sample counts: 0x" << std::hex << device_properties.limits.sampledImageColorSampleCounts << '\n';
//            std::cout << std::setw(48) << "Sampled image depth sample counts: 0x" << std::hex << device_properties.limits.sampledImageDepthSampleCounts << '\n';
//            std::cout << std::setw(48) << "Sampled image integer sample counts: 0x" << std::hex << device_properties.limits.sampledImageIntegerSampleCounts << '\n';
//            std::cout << std::setw(48) << "Sampled image stencil sample counts: 0x" << std::hex << device_properties.limits.sampledImageStencilSampleCounts << '\n';
//            std::cout << std::setw(48) << "Sparse address space size: " << std::dec << device_properties.limits.sparseAddressSpaceSize << '\n';
//            std::cout << std::setw(48) << "Standard sample locations: " << device_properties.limits.standardSampleLocations << '\n';
//            std::cout << std::setw(48) << "Storage image sample counts: 0x" << std::hex << device_properties.limits.storageImageSampleCounts << '\n';
//            std::cout << std::setw(48) << "Strict lines: " << std::dec << device_properties.limits.strictLines << '\n';
//            std::cout << std::setw(48) << "Sub pixel interpolation offset bits: " << device_properties.limits.subPixelInterpolationOffsetBits << '\n';
//            std::cout << std::setw(48) << "Sub pixel precision bits: " << device_properties.limits.subPixelPrecisionBits << '\n';
//            std::cout << std::setw(48) << "Sub texel precision bits: " << device_properties.limits.subTexelPrecisionBits << '\n';
//            std::cout << std::setw(48) << "Timestamp compute and graphics: " << device_properties.limits.timestampComputeAndGraphics << '\n';
//            std::cout << std::setw(48) << "Timestamp period: " << device_properties.limits.timestampPeriod << '\n';
//            std::cout << std::setw(48) << "Viewport bounds range: [" << device_properties.limits.viewportBoundsRange[0] << ", " << device_properties.limits.viewportBoundsRange[1] << "]" << '\n';
//            std::cout << std::setw(48) << "Viewport sub pixel bits: " << device_properties.limits.viewportSubPixelBits << '\n';
//
//            std::cout << std::setw(48) << "Residency aligned mip size: " << device_properties.sparseProperties.residencyAlignedMipSize << '\n';
//            std::cout << std::setw(48) << "Non resident strict: " << device_properties.sparseProperties.residencyNonResidentStrict << '\n';
//            std::cout << std::setw(48) << "Residency standard 2D block shape: " << device_properties.sparseProperties.residencyStandard2DBlockShape << '\n';
//            std::cout << std::setw(48) << "Residency standard 2D multisample block shape: " << device_properties.sparseProperties.residencyStandard2DMultisampleBlockShape << '\n';
//            std::cout << std::setw(48) << "Residency standard 3D block shape: " << device_properties.sparseProperties.residencyStandard3DBlockShape << '\n';
//
//            std::cout << std::setw(48) << "Pipeline cache UUID: " << std::hex << device_properties.pipelineCacheUUID[0] << " ";
//            for (uint16_t i = 1; i < 15; ++i)
//            {
//                std::cout << std::hex << device_properties.pipelineCacheUUID[i] << " ";
//            }
//            std::cout << std::hex << device_properties.pipelineCacheUUID[15] << '\n';
//        }
//    }
//}