/*
* Copyright (c) 2017 Michael Mathers
*/
#include <vulkan/vulkan.h>
#include <iomanip>
#include <iostream>


namespace
{
    void print_physical_device_information(VkPhysicalDevice* device_list, uint32_t device_count)
    {
        std::cout << "Printing information for " << device_count << " physical devices" << '\n';
        for (uint32_t i = 0; i < device_count; ++i)
        {
            VkPhysicalDeviceProperties device_properties;
            vkGetPhysicalDeviceProperties(device_list[i], &device_properties);

            std::cout << "Physical device " << i << ":" << '\n';
            std::cout << std::setw(48) << "API version: " << device_properties.apiVersion << '\n';
            std::cout << std::setw(48) << "Device ID: " << device_properties.deviceID << '\n';
            std::cout << std::setw(48) << "Device name: " << device_properties.deviceName << '\n';
            std::cout << std::setw(48) << "Device type: ";
            switch (device_properties.deviceType)
            {
            case VK_PHYSICAL_DEVICE_TYPE_CPU:
                std::cout << "CPU" << '\n';
                break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                std::cout << "Discrete GPU" << '\n';
                break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                std::cout << "Integrated GPU" << '\n';
                break;
            case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                std::cout << "Other" << '\n';
                break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                std::cout << "Virtual GPU" << '\n';
                break;
            default:
                std::cerr << "Unknown" << '\n';
                break;
            }
            
            std::cout << std::setw(48) << "Driver version: " << device_properties.driverVersion << '\n';
            std::cout << std::setw(48) << "Vendor ID: " << device_properties.vendorID << '\n';

            std::cout << std::setw(48) << "Buffer image granularity: " << std::dec << device_properties.limits.bufferImageGranularity << '\n';
            std::cout << std::setw(48) << "Discrete queue priorities: " << device_properties.limits.discreteQueuePriorities << '\n';
            std::cout << std::setw(48) << "Framebuffer colour sample counts: 0x" << std::hex << device_properties.limits.framebufferColorSampleCounts << '\n';
            std::cout << std::setw(48) << "Framebuffer depth sample counts: 0x" << std::hex << device_properties.limits.framebufferDepthSampleCounts << '\n';
            std::cout << std::setw(48) << "Framebuffer no attachment sample counts: 0x" << std::hex << device_properties.limits.framebufferNoAttachmentsSampleCounts << '\n';
            std::cout << std::setw(48) << "Framebuffer stencil sample counts: 0x" << std::hex << device_properties.limits.framebufferStencilSampleCounts << '\n';
            std::cout << std::setw(48) << "Line width granularity: " << std::dec << device_properties.limits.lineWidthGranularity << '\n';
            std::cout << std::setw(48) << "Line width range: [" << device_properties.limits.lineWidthRange[0] << ", " << device_properties.limits.lineWidthRange[1] << "]" << '\n';
            std::cout << std::setw(48) << "Max bound descriptor sets: " << device_properties.limits.maxBoundDescriptorSets << '\n';
            std::cout << std::setw(48) << "Max clip distances: " << device_properties.limits.maxClipDistances << '\n';
            std::cout << std::setw(48) << "Max colour attachments: " << device_properties.limits.maxColorAttachments << '\n';
            std::cout << std::setw(48) << "Max combined clip and cull distances: " << device_properties.limits.maxCombinedClipAndCullDistances << '\n';
            std::cout << std::setw(48) << "Max compute shader memory size: " << device_properties.limits.maxComputeSharedMemorySize << '\n';
            std::cout << std::setw(48) << "Max compute work group count: [" << device_properties.limits.maxComputeWorkGroupCount[0] << ", " << device_properties.limits.maxComputeWorkGroupCount[1] << ", " << device_properties.limits.maxComputeWorkGroupCount[2] << "]" << '\n';
            std::cout << std::setw(48) << "Max compute work group invocations: " << device_properties.limits.maxComputeWorkGroupInvocations << '\n';
            std::cout << std::setw(48) << "Max compute work group size: [" << device_properties.limits.maxComputeWorkGroupSize[0] << ", " << device_properties.limits.maxComputeWorkGroupSize[1] << ", " << device_properties.limits.maxComputeWorkGroupSize[2] << "]" << '\n';
            std::cout << std::setw(48) << "Max cull distances: " << device_properties.limits.maxCullDistances << '\n';
            std::cout << std::setw(48) << "Max descriptor set input attachments: " << device_properties.limits.maxDescriptorSetInputAttachments << '\n';
            std::cout << std::setw(48) << "Max descriptor set sampled images: " << device_properties.limits.maxDescriptorSetSampledImages << '\n';
            std::cout << std::setw(48) << "Max descriptor set samplers: " << device_properties.limits.maxDescriptorSetSamplers << '\n';
            std::cout << std::setw(48) << "Max descriptor set storage buffers: " << device_properties.limits.maxDescriptorSetStorageBuffers << '\n';
            std::cout << std::setw(48) << "Max descriptor set dynamic storage buffers : " << device_properties.limits.maxDescriptorSetStorageBuffersDynamic << '\n';
            std::cout << std::setw(48) << "Max descriptor set storage images: " << device_properties.limits.maxDescriptorSetStorageImages << '\n';
            std::cout << std::setw(48) << "Max descriptor set uniform buffers: " << device_properties.limits.maxDescriptorSetUniformBuffers << '\n';
            std::cout << std::setw(48) << "Max descriptor set dynamic uniform buffers: " << device_properties.limits.maxDescriptorSetUniformBuffersDynamic << '\n';
            std::cout << std::setw(48) << "Max draw indexed index value: " << device_properties.limits.maxDrawIndexedIndexValue << '\n';
            std::cout << std::setw(48) << "Max draw indirect count: " << device_properties.limits.maxDrawIndirectCount << '\n';
            std::cout << std::setw(48) << "Max fragment combined output resources: " << device_properties.limits.maxFragmentCombinedOutputResources << '\n';
            std::cout << std::setw(48) << "Max fragment dual source attachments: " << device_properties.limits.maxFragmentDualSrcAttachments << '\n';
            std::cout << std::setw(48) << "Max fragment input components: " << device_properties.limits.maxFragmentInputComponents << '\n';
            std::cout << std::setw(48) << "Max fragment output attachments: " << device_properties.limits.maxFragmentOutputAttachments << '\n';
            std::cout << std::setw(48) << "Max framebuffer width: " << device_properties.limits.maxFramebufferWidth << '\n';
            std::cout << std::setw(48) << "Max framebuffer height: " << device_properties.limits.maxFramebufferHeight << '\n';
            std::cout << std::setw(48) << "Max framebuffer layers: " << device_properties.limits.maxFramebufferLayers << '\n';
            std::cout << std::setw(48) << "Max geometry input components: " << device_properties.limits.maxGeometryInputComponents << '\n';
            std::cout << std::setw(48) << "Max geometry output components: " << device_properties.limits.maxGeometryOutputComponents << '\n';
            std::cout << std::setw(48) << "Max geometry output vertices: " << device_properties.limits.maxGeometryOutputVertices << '\n';
            std::cout << std::setw(48) << "Max geometry shader invocations: " << device_properties.limits.maxGeometryShaderInvocations << '\n';
            std::cout << std::setw(48) << "Max geometry total output components: " << device_properties.limits.maxGeometryTotalOutputComponents << '\n';
            std::cout << std::setw(48) << "Make image array layers: " << device_properties.limits.maxImageArrayLayers << '\n';
            std::cout << std::setw(48) << "Make image dimension 1D: " << device_properties.limits.maxImageDimension1D << '\n';
            std::cout << std::setw(48) << "Make image dimension 2D: " << device_properties.limits.maxImageDimension2D << '\n';
            std::cout << std::setw(48) << "Make image dimension 3D: " << device_properties.limits.maxImageDimension3D << '\n';
            std::cout << std::setw(48) << "Make image dimension cube: " << device_properties.limits.maxImageDimensionCube << '\n';
            std::cout << std::setw(48) << "Make interpolation offset: " << device_properties.limits.maxInterpolationOffset << '\n';
            std::cout << std::setw(48) << "Make memory allocation count: " << device_properties.limits.maxMemoryAllocationCount << '\n';
            std::cout << std::setw(48) << "Max per stage descriptor input attachments: " << device_properties.limits.maxPerStageDescriptorInputAttachments << '\n';
            std::cout << std::setw(48) << "Max per stage descriptor sampled images: " << device_properties.limits.maxPerStageDescriptorSampledImages << '\n';
            std::cout << std::setw(48) << "Max per stage descriptor samplers: " << device_properties.limits.maxPerStageDescriptorSamplers << '\n';
            std::cout << std::setw(48) << "Max per stage descriptor storage buffers: " << device_properties.limits.maxPerStageDescriptorStorageBuffers << '\n';
            std::cout << std::setw(48) << "Max per stage descriptor storage images: " << device_properties.limits.maxPerStageDescriptorStorageImages << '\n';
            std::cout << std::setw(48) << "Max per stage descriptor uniform buffers: " << device_properties.limits.maxPerStageDescriptorUniformBuffers << '\n';
            std::cout << std::setw(48) << "Max per stage resources: " << device_properties.limits.maxPerStageResources << '\n'; 
            std::cout << std::setw(48) << "Max push constants size: " << device_properties.limits.maxPushConstantsSize << '\n';
            std::cout << std::setw(48) << "Max sampler mask words: " << device_properties.limits.maxSampleMaskWords << '\n';
            std::cout << std::setw(48) << "Max sampler allocation count: " << device_properties.limits.maxSamplerAllocationCount << '\n';
            std::cout << std::setw(48) << "Max sampler anisotropy: " << device_properties.limits.maxSamplerAnisotropy << '\n';
            std::cout << std::setw(48) << "Max sampler LOD bias: " << device_properties.limits.maxSamplerLodBias << '\n';
            std::cout << std::setw(48) << "Max storage buffer range: " << device_properties.limits.maxStorageBufferRange << '\n';
            std::cout << std::setw(48) << "Max tessellation control per patch output components: " << device_properties.limits.maxTessellationControlPerPatchOutputComponents << '\n';
            std::cout << std::setw(48) << "Max tessellation control per vertex input components: " << device_properties.limits.maxTessellationControlPerVertexInputComponents << '\n';
            std::cout << std::setw(48) << "Max tessellation control per vertex output components: " << device_properties.limits.maxTessellationControlPerVertexOutputComponents << '\n';
            std::cout << std::setw(48) << "Max tessellation control total output components: " << device_properties.limits.maxTessellationControlTotalOutputComponents << '\n';
            std::cout << std::setw(48) << "Max tessellation evaluation input components: " << device_properties.limits.maxTessellationEvaluationInputComponents << '\n';
            std::cout << std::setw(48) << "Max tessellation evaluation output components: " << device_properties.limits.maxTessellationEvaluationOutputComponents << '\n';
            std::cout << std::setw(48) << "Max tessellation general level: " << device_properties.limits.maxTessellationGenerationLevel << '\n';
            std::cout << std::setw(48) << "Max tessellation patch size: " << device_properties.limits.maxTessellationPatchSize << '\n';
            std::cout << std::setw(48) << "Max texel buffer elements: " << device_properties.limits.maxTexelBufferElements << '\n';
            std::cout << std::setw(48) << "Max texel gather offset: " << device_properties.limits.maxTexelGatherOffset << '\n';
            std::cout << std::setw(48) << "Max texel offset: " << device_properties.limits.maxTexelOffset << '\n';
            std::cout << std::setw(48) << "Max vertex input attributes: " << device_properties.limits.maxVertexInputAttributes << '\n';
            std::cout << std::setw(48) << "Max vertex input attribute offset: " << device_properties.limits.maxVertexInputAttributeOffset << '\n';
            std::cout << std::setw(48) << "Max vertex input bindings: " << device_properties.limits.maxVertexInputBindings << '\n';
            std::cout << std::setw(48) << "Max vertex input binding stride: " << device_properties.limits.maxVertexInputBindingStride << '\n';
            std::cout << std::setw(48) << "Max vertex output components: " << device_properties.limits.maxVertexOutputComponents << '\n';
            std::cout << std::setw(48) << "Max viewport dimensions: [" << device_properties.limits.maxViewportDimensions[0] << ", " << device_properties.limits.maxViewportDimensions[1] << "]" << '\n';
            std::cout << std::setw(48) << "Max viewports: " << device_properties.limits.maxViewports << '\n';
            std::cout << std::setw(48) << "Min interpolation offset: " << device_properties.limits.minInterpolationOffset << '\n';
            std::cout << std::setw(48) << "Min memory map alignment: " << device_properties.limits.minMemoryMapAlignment << '\n';
            std::cout << std::setw(48) << "Min storage offset alignment offset: " << device_properties.limits.minStorageBufferOffsetAlignment << '\n';
            std::cout << std::setw(48) << "Min texel buffer alignment offset: " << device_properties.limits.minTexelBufferOffsetAlignment << '\n';
            std::cout << std::setw(48) << "Min texel gather offset: " << device_properties.limits.minTexelGatherOffset << '\n';
            std::cout << std::setw(48) << "Min uniform buffer alignment offset: " << device_properties.limits.minUniformBufferOffsetAlignment << '\n';
            std::cout << std::setw(48) << "Mipmap precision bits: " << device_properties.limits.mipmapPrecisionBits << '\n';
            std::cout << std::setw(48) << "Non-coherent atom size: " << device_properties.limits.nonCoherentAtomSize << '\n';
            std::cout << std::setw(48) << "Optimal buffer copy offset alignment: " << device_properties.limits.optimalBufferCopyOffsetAlignment << '\n';
            std::cout << std::setw(48) << "Optimal buffer copy row pitch alignment: " << device_properties.limits.optimalBufferCopyRowPitchAlignment << '\n';
            std::cout << std::setw(48) << "Point size granularity: " << device_properties.limits.pointSizeGranularity << '\n';
            std::cout << std::setw(48) << "Point size range: [" << device_properties.limits.pointSizeRange[0] << ", " << device_properties.limits.pointSizeRange[1] << "]" << '\n';
            std::cout << std::setw(48) << "Sampled image colour sample counts: 0x" << std::hex << device_properties.limits.sampledImageColorSampleCounts << '\n';
            std::cout << std::setw(48) << "Sampled image depth sample counts: 0x" << std::hex << device_properties.limits.sampledImageDepthSampleCounts << '\n';
            std::cout << std::setw(48) << "Sampled image integer sample counts: 0x" << std::hex << device_properties.limits.sampledImageIntegerSampleCounts << '\n';
            std::cout << std::setw(48) << "Sampled image stencil sample counts: 0x" << std::hex << device_properties.limits.sampledImageStencilSampleCounts << '\n';
            std::cout << std::setw(48) << "Sparse address space size: " << std::dec << device_properties.limits.sparseAddressSpaceSize << '\n';
            std::cout << std::setw(48) << "Standard sample locations: " << device_properties.limits.standardSampleLocations << '\n';
            std::cout << std::setw(48) << "Storage image sample counts: 0x" << std::hex << device_properties.limits.storageImageSampleCounts << '\n';
            std::cout << std::setw(48) << "Strict lines: " << std::dec << device_properties.limits.strictLines << '\n';
            std::cout << std::setw(48) << "Sub pixel interpolation offset bits: " << device_properties.limits.subPixelInterpolationOffsetBits << '\n';
            std::cout << std::setw(48) << "Sub pixel precision bits: " << device_properties.limits.subPixelPrecisionBits << '\n';
            std::cout << std::setw(48) << "Sub texel precision bits: " << device_properties.limits.subTexelPrecisionBits << '\n';
            std::cout << std::setw(48) << "Timestamp compute and graphics: " << device_properties.limits.timestampComputeAndGraphics << '\n';
            std::cout << std::setw(48) << "Timestamp period: " << device_properties.limits.timestampPeriod << '\n';
            std::cout << std::setw(48) << "Viewport bounds range: [" << device_properties.limits.viewportBoundsRange[0] << ", " << device_properties.limits.viewportBoundsRange[1] << "]" << '\n';
            std::cout << std::setw(48) << "Viewport sub pixel bits: " << device_properties.limits.viewportSubPixelBits << '\n';

            std::cout << std::setw(48) << "Residency aligned mip size: " << device_properties.sparseProperties.residencyAlignedMipSize << '\n';
            std::cout << std::setw(48) << "Non resident strict: " << device_properties.sparseProperties.residencyNonResidentStrict << '\n';
            std::cout << std::setw(48) << "Residency standard 2D block shape: " << device_properties.sparseProperties.residencyStandard2DBlockShape << '\n';
            std::cout << std::setw(48) << "Residency standard 2D multisample block shape: " << device_properties.sparseProperties.residencyStandard2DMultisampleBlockShape << '\n';
            std::cout << std::setw(48) << "Residency standard 3D block shape: " << device_properties.sparseProperties.residencyStandard3DBlockShape << '\n';

            std::cout << std::setw(48) << "Pipeline cache UUID: " << std::hex << device_properties.pipelineCacheUUID[0] << " ";
            for (uint16_t i = 1; i < 15; ++i)
            {
                std::cout << device_properties.pipelineCacheUUID[i] << " ";
            }
            std::cout << device_properties.pipelineCacheUUID[15] << '\n';
        }
    }
}


int main(int argc, char *argv[])
{
    VkResult result;

    /*
    There is no global state in Vulkan. State is stored on a per-application
    basis within a VkInstance object. Creation of VkInstance object also
    initialises the Vulkan library.
    [Vulkan 1.0 Specification - Section 3.2 - Instances]
    */
    VkInstance instance;

    std::string application_name("Vulkan Sample App");
    std::string engine_name("Vulkan Sample Engine");

    VkApplicationInfo app_info = {};
    app_info.apiVersion = VK_API_VERSION_1_0;
    app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.pApplicationName = application_name.c_str();
    app_info.pEngineName = engine_name.c_str();
    app_info.pNext = nullptr;
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    VkInstanceCreateInfo instance_info = {};
    instance_info.enabledExtensionCount = 0;
    instance_info.enabledLayerCount = 0;
    instance_info.flags = 0;
    instance_info.pApplicationInfo = &app_info;
    instance_info.pNext = nullptr;
    instance_info.ppEnabledExtensionNames = nullptr;
    instance_info.ppEnabledLayerNames = nullptr;
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    result = vkCreateInstance(&instance_info, nullptr, &instance);
    switch (result)
    {
    case VK_SUCCESS:
        break;
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        std::cerr << "Vulkan instance couldn't be created, incompatible driver detected!" << '\n';
        return EXIT_FAILURE;
    default:
        std::cerr << "Vulkan instance couldn't be created, unknown error!" << '\n';
        return EXIT_FAILURE;
    }


    /*
    Vulkan separates the concept of physical and logical devices. A physical
    device represents a device in a system (maybe several pieces of hardware
    working together). A logical device represents an application view of a
    device.
    [Vulkan 1.0 Specification - Section 4.1 - Physical Devices]
    */
    uint32_t physical_device_count;
    result = vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Unable to determine number of available physical devices, unknown error!" << '\n';
        vkDestroyInstance(instance, nullptr);
        return EXIT_FAILURE;
    }

    VkPhysicalDevice* physical_device_list = new VkPhysicalDevice[physical_device_count];
    if (physical_device_list == nullptr)
    {
        std::cerr << "Unable to allocate memory for a physical device array!" << '\n';
        vkDestroyInstance(instance, nullptr);
        return EXIT_FAILURE;
    }

    result = vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_device_list);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Unable to enumerate available physical devices, unknown error!" << '\n';
        delete[] physical_device_list;
        vkDestroyInstance(instance, nullptr);
        return EXIT_FAILURE;
    }

    print_physical_device_information(physical_device_list, physical_device_count);

    delete[] physical_device_list;
    vkDestroyInstance(instance, nullptr);

    return EXIT_SUCCESS;
}
