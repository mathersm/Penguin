/*
* Copyright (c) 2019 Michael Mathers
*/
#include "Vulkan_Renderer.h"
#include <sstream>
#include <iostream>
#include <fstream>



namespace Penguin::Sample
{
    Shader_Source::Shader_Source(void)
        : size(0)
        , code(nullptr)
    {
        // Does nothing
    }


    Shader_Source::~Shader_Source(void)
    {
        if (this->code)
        {
            delete[] code;
        }
    }


    Shader_Source::Shader_Source(Shader_Source&& source)
    {
        this->size = std::move(source.size);
        this->code = std::move(source.code);
        source.size = 0;
        source.code = nullptr;
    }


    Shader_Source&
    Shader_Source::operator = (Shader_Source&& source)
    {
        this->size = std::move(source.size);
        this->code = std::move(source.code);
        source.size = 0;
        source.code = nullptr;
        return *this;
    }



    Vulkan_Renderer::Vulkan_Renderer(void)
    {
    }


    Vulkan_Renderer::~Vulkan_Renderer(void)
    {
    }


    void
        Vulkan_Renderer::begin_command_buffer_recording(size_t buffer_index)
    {
        vk::CommandBufferBeginInfo begin_info;
        // begin_info.flags;
        begin_info.pInheritanceInfo = nullptr;
        begin_info.pNext = nullptr;

        std::lock_guard guard(this->command_pool_.mutex);
        this->command_pool_.command_buffers[buffer_index].begin(begin_info);
    }


    void
        Vulkan_Renderer::create_presentation_surface(void)
    {
#if defined(VK_USE_PLATFORM_WIN32_KHR)
            vk::Win32SurfaceCreateInfoKHR surface_creation_info;
            surface_creation_info.hinstance = this->window_->get_instance_handle();
            surface_creation_info.hwnd = this->window_->get_window_handle();
            {
                std::lock_guard<std::mutex> instance_guard(this->instance_.mutex);
                this->presentation_surface_ = this->instance_.synced_object.createWin32SurfaceKHR(surface_creation_info);
            }
#else
#error("No implementation available other than for Windows")
#endif
    }


    void
        Vulkan_Renderer::create_command_buffers(void)
    {
        /*
        Host access to command buffers must be externally synchronized.

        [Vulkan 1.1.88 Specification - Section 5.3 - Command Buffer Allocation and Management]
        */
        std::lock_guard<std::mutex> command_pool_guard(this->command_pool_.mutex);

        vk::CommandBufferAllocateInfo buffer_allocation_info;
        buffer_allocation_info.commandBufferCount = 1;
        buffer_allocation_info.commandPool = this->command_pool_.command_pool;
        buffer_allocation_info.level = vk::CommandBufferLevel::ePrimary;

        {
            std::lock_guard guard(this->logical_device_.mutex);
            this->command_pool_.command_buffers = this->logical_device_.synced_object.allocateCommandBuffers(buffer_allocation_info);
        }
    }


    void
        Vulkan_Renderer::create_command_pool(void)
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
        pool_info.queueFamilyIndex = this->queue_family_index_;
        pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

        std::scoped_lock sync_guard(this->logical_device_.mutex, this->command_pool_.mutex);
        this->command_pool_.command_pool = this->logical_device_.synced_object.createCommandPool(pool_info);
    }


    void
    Vulkan_Renderer::create_framebuffer(void)
    {
        /*
        Render passes operate in conjunction with framebuffers. Framebuffers
        represent a collection of specific memory attachments that a render
        pass instance uses
        
        [Vulkan 1.1.94 Specification - Section 7.3 - Framebuffers]
        */
        RECT window_area = this->window_->get_client_area();

        vk::FramebufferCreateInfo framebuffer_info;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.height = window_area.bottom - window_area.top;
        framebuffer_info.layers = 1;
    }


    void
        Vulkan_Renderer::create_graphics_queue(void)
    {
        std::lock_guard<std::mutex> logical_device_guard(this->logical_device_.mutex);
        this->graphics_queue_ = this->logical_device_.synced_object.getQueue(this->queue_family_index_, 0);
    }


    void
        Vulkan_Renderer::create_instance(void)
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

        auto validation_layers = this->validate_validation_layers(desired_validation_layers);
        auto extensions = this->validate_instance_extensions(desired_instance_extensions);

        vk::InstanceCreateInfo instance_info;
        instance_info.pNext = nullptr;
        instance_info.flags = vk::InstanceCreateFlags();
        instance_info.pApplicationInfo = &app_info;
        instance_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        instance_info.ppEnabledLayerNames = validation_layers.data();
        instance_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instance_info.ppEnabledExtensionNames = extensions.data();

        std::lock_guard<std::mutex> instance_guard(this->instance_.mutex);
        this->instance_.synced_object = vk::createInstance(instance_info);
    }


    void
        Vulkan_Renderer::create_logical_device(void)
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
        queue_creation_info.queueFamilyIndex = this->queue_family_index_;

        auto device_layers = this->validate_device_layers(desired_device_layers);
        auto device_extensions = this->validate_device_extensions(desired_device_extensions);

        vk::DeviceCreateInfo device_creation_info;
        device_creation_info.enabledLayerCount = static_cast<uint32_t>(device_layers.size());
        device_creation_info.ppEnabledLayerNames = device_layers.data();
        device_creation_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        device_creation_info.ppEnabledExtensionNames = device_extensions.data();
        device_creation_info.queueCreateInfoCount = 1;
        device_creation_info.pQueueCreateInfos = &queue_creation_info;

        std::lock_guard<std::mutex> logical_device_guard(this->logical_device_.mutex);
        this->logical_device_.synced_object = this->physical_device_.createDevice(device_creation_info);
    }


    void
        Vulkan_Renderer::create_physical_device(void)
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
            std::lock_guard<std::mutex> instance_guard(this->instance_.mutex);
            physical_devices = this->instance_.synced_object.enumeratePhysicalDevices();
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
                this->physical_device_ = device;
                return;
            }
        }

        throw std::runtime_error("No suitable physical device found");
    }


    vk::PipelineColorBlendStateCreateInfo
    Vulkan_Renderer::define_color_blend_state(const std::vector<vk::PipelineColorBlendAttachmentState>& attachment_state) const
    {
        vk::PipelineColorBlendStateCreateInfo color_blend_state;
        color_blend_state.attachmentCount = static_cast<uint32_t>(attachment_state.size());
        color_blend_state.blendConstants;
        color_blend_state.flags;
        color_blend_state.logicOp;
        color_blend_state.logicOpEnable;
        color_blend_state.pAttachments = attachment_state.data();
        color_blend_state.pNext = nullptr;
        return color_blend_state;
    }


    vk::PipelineDepthStencilStateCreateInfo
    Vulkan_Renderer::define_depth_stencil_state(void) const
    {
        vk::PipelineDepthStencilStateCreateInfo depth_stencil_state;
        depth_stencil_state.back.compareMask;
        depth_stencil_state.back.compareOp = vk::CompareOp::eAlways;
        depth_stencil_state.back.depthFailOp;
        depth_stencil_state.back.failOp = vk::StencilOp::eKeep;
        depth_stencil_state.back.passOp = vk::StencilOp::eKeep;
        depth_stencil_state.back.reference;
        depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
        depth_stencil_state.depthCompareOp = vk::CompareOp::eLessOrEqual;
        depth_stencil_state.depthTestEnable = VK_TRUE;
        depth_stencil_state.depthWriteEnable = VK_TRUE;
        depth_stencil_state.flags;
        depth_stencil_state.front = depth_stencil_state.back;
        depth_stencil_state.maxDepthBounds;
        depth_stencil_state.minDepthBounds;
        depth_stencil_state.pNext = nullptr;
        depth_stencil_state.stencilTestEnable = VK_FALSE;
        return depth_stencil_state;
    }


    vk::PipelineDynamicStateCreateInfo
    Vulkan_Renderer::define_dynamic_state(const std::vector<vk::DynamicState>& dynamic_state) const
    {
        vk::PipelineDynamicStateCreateInfo dynamic_state_info;
        dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(dynamic_state.size());
        dynamic_state_info.flags;
        dynamic_state_info.pDynamicStates = dynamic_state.data();
        dynamic_state_info.pNext = nullptr;
        return dynamic_state_info;
    }


    vk::PipelineInputAssemblyStateCreateInfo
    Vulkan_Renderer::define_input_assembly_state(void) const
    {
        vk::PipelineInputAssemblyStateCreateInfo input_assembly_state;
        input_assembly_state.flags;
        input_assembly_state.pNext = nullptr;
        input_assembly_state.primitiveRestartEnable;
        input_assembly_state.topology = vk::PrimitiveTopology::eTriangleList;
        return input_assembly_state;
    }


    vk::PipelineMultisampleStateCreateInfo
    Vulkan_Renderer::define_multisample_state(void) const
    {
        vk::PipelineMultisampleStateCreateInfo multisample_state;
        multisample_state.alphaToCoverageEnable;
        multisample_state.alphaToOneEnable;
        multisample_state.flags;
        multisample_state.minSampleShading;
        multisample_state.pNext = nullptr;
        multisample_state.pSampleMask = nullptr;
        multisample_state.rasterizationSamples = vk::SampleCountFlagBits::e1;
        multisample_state.sampleShadingEnable;
        return multisample_state;
    }


    vk::PipelineRasterizationStateCreateInfo
    Vulkan_Renderer::define_rasterization_state(void) const
    {
        vk::PipelineRasterizationStateCreateInfo rasterization_state;
        rasterization_state.cullMode = vk::CullModeFlagBits::eNone;
        rasterization_state.depthBiasClamp;
        rasterization_state.depthBiasConstantFactor;
        rasterization_state.depthBiasEnable = VK_FALSE;
        rasterization_state.depthBiasSlopeFactor;
        rasterization_state.depthClampEnable = VK_FALSE;
        rasterization_state.flags;
        rasterization_state.frontFace = vk::FrontFace::eCounterClockwise;
        rasterization_state.lineWidth = 1.0f;
        rasterization_state.pNext = nullptr;
        rasterization_state.polygonMode = vk::PolygonMode::eFill;
        rasterization_state.rasterizerDiscardEnable = VK_FALSE;
        return rasterization_state;
    }


    std::vector<vk::VertexInputAttributeDescription>
    Vulkan_Renderer::define_vertex_input_attributes(void) const
    {
        std::vector<vk::VertexInputAttributeDescription> vertex_input_attributes;
        vertex_input_attributes.push_back(vk::VertexInputAttributeDescription());
        vertex_input_attributes.push_back(vk::VertexInputAttributeDescription());
        vertex_input_attributes[0].binding = 0;
        vertex_input_attributes[0].format = vk::Format::eR32G32B32Sfloat;
        vertex_input_attributes[0].location = 0;
        vertex_input_attributes[0].offset = offsetof(Penguin::Sample::Vertex, position);
        vertex_input_attributes[1].binding = 0;
        vertex_input_attributes[1].format = vk::Format::eR32G32B32Sfloat;
        vertex_input_attributes[1].location = 1;
        vertex_input_attributes[1].offset = offsetof(Penguin::Sample::Vertex, color);
        return vertex_input_attributes;
    }


    std::vector<vk::VertexInputBindingDescription>
    Vulkan_Renderer::define_vertex_input_binding(void) const
    {
        std::vector<vk::VertexInputBindingDescription> vertex_input_binding;
        vertex_input_binding.push_back(vk::VertexInputBindingDescription());
        vertex_input_binding[0].binding = 0;
        vertex_input_binding[0].inputRate = vk::VertexInputRate::eVertex;
        vertex_input_binding[0].stride = sizeof(Penguin::Sample::Vertex);
        return vertex_input_binding;
    }


    vk::PipelineVertexInputStateCreateInfo
    Vulkan_Renderer::define_vertex_input_state(const std::vector<vk::VertexInputBindingDescription>& vertex_input_binding, const std::vector<vk::VertexInputAttributeDescription>& vertex_input_attributes) const
    {
        vk::PipelineVertexInputStateCreateInfo vertex_input_state;
        vertex_input_state.flags;
        vertex_input_state.pNext = nullptr;
        vertex_input_state.pVertexAttributeDescriptions = vertex_input_attributes.data();
        vertex_input_state.pVertexBindingDescriptions = vertex_input_binding.data();
        vertex_input_state.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_input_attributes.size());
        vertex_input_state.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_input_binding.size());
        return vertex_input_state;
    }


    vk::PipelineViewportStateCreateInfo
    Vulkan_Renderer::define_viewport_state(void) const
    {
        vk::PipelineViewportStateCreateInfo viewport_state;
        viewport_state.flags;
        viewport_state.pNext = nullptr;
        viewport_state.pScissors = nullptr;
        viewport_state.pViewports = nullptr;
        viewport_state.scissorCount = 1;
        viewport_state.viewportCount = 1;
        return viewport_state;
    }


    void
    Vulkan_Renderer::create_pipeline(void)
    {
        vk::PipelineDepthStencilStateCreateInfo depth_stencil_state = this->define_depth_stencil_state();
        vk::PipelineInputAssemblyStateCreateInfo input_assembly_state = this->define_input_assembly_state();
        vk::PipelineMultisampleStateCreateInfo multisample_state = this->define_multisample_state();
        vk::PipelineRasterizationStateCreateInfo rasterization_state = this->define_rasterization_state();
        vk::PipelineViewportStateCreateInfo viewport_state = this->define_viewport_state();

        std::vector<vk::PipelineColorBlendAttachmentState> blend_attachment_states;
        blend_attachment_states.push_back(vk::PipelineColorBlendAttachmentState());
        blend_attachment_states[0].alphaBlendOp;
        blend_attachment_states[0].blendEnable = VK_FALSE;
        blend_attachment_states[0].colorBlendOp;
        blend_attachment_states[0].colorWriteMask = vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eR;
        blend_attachment_states[0].dstAlphaBlendFactor;
        blend_attachment_states[0].dstColorBlendFactor;
        blend_attachment_states[0].srcAlphaBlendFactor;
        blend_attachment_states[0].srcColorBlendFactor;
        vk::PipelineColorBlendStateCreateInfo color_blend_state = this->define_color_blend_state(blend_attachment_states);
 
        std::vector<vk::DynamicState> dynamic_state;
        dynamic_state.push_back(vk::DynamicState::eScissor);
        dynamic_state.push_back(vk::DynamicState::eViewport);
        vk::PipelineDynamicStateCreateInfo dynamic_state_info = this->define_dynamic_state(dynamic_state);

        vk::PipelineVertexInputStateCreateInfo vertex_input_state = 
            this->define_vertex_input_state(
                this->define_vertex_input_binding(),
                this->define_vertex_input_attributes()
            );

        std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;
        shader_stages.push_back(vk::PipelineShaderStageCreateInfo());
        shader_stages.push_back(vk::PipelineShaderStageCreateInfo());
        shader_stages[0].flags;
        shader_stages[0].module = this->vertex_shader_module_;
        shader_stages[0].pName = "main";
        shader_stages[0].pNext = nullptr;
        shader_stages[0].pSpecializationInfo;
        shader_stages[0].stage = vk::ShaderStageFlagBits::eVertex;
        shader_stages[1].flags;
        shader_stages[1].module = this->fragment_shader_module_;
        shader_stages[1].pName = "main";
        shader_stages[1].pNext = nullptr;
        shader_stages[1].pSpecializationInfo;
        shader_stages[1].stage = vk::ShaderStageFlagBits::eFragment;

        vk::GraphicsPipelineCreateInfo pipeline_info;
        pipeline_info.basePipelineHandle;
        pipeline_info.basePipelineIndex;
        pipeline_info.flags; // reserved for extension use
        pipeline_info.layout;
        pipeline_info.pColorBlendState = std::addressof(color_blend_state);
        pipeline_info.pDepthStencilState = std::addressof(depth_stencil_state);
        pipeline_info.pDynamicState = std::addressof(dynamic_state_info);
        pipeline_info.pInputAssemblyState = std::addressof(input_assembly_state);
        pipeline_info.pMultisampleState = std::addressof(multisample_state);
        pipeline_info.pNext = nullptr;
        pipeline_info.pRasterizationState = std::addressof(rasterization_state);
        pipeline_info.pStages = shader_stages.data();
        pipeline_info.pTessellationState;
        pipeline_info.pVertexInputState = std::addressof(vertex_input_state);
        pipeline_info.pViewportState = std::addressof(viewport_state);
        pipeline_info.renderPass = this->render_pass_;
        pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
        pipeline_info.subpass;

        std::lock_guard<std::mutex> logical_device_guard(this->logical_device_.mutex);
        this->pipeline_ = this->logical_device_.synced_object.createGraphicsPipeline(this->pipeline_cache_, pipeline_info);
    }


    void
    Vulkan_Renderer::create_pipeline_cache(void)
    {
        vk::PipelineCacheCreateInfo pipeline_cache_info;
        pipeline_cache_info.flags;
        pipeline_cache_info.initialDataSize;
        pipeline_cache_info.pInitialData;
        pipeline_cache_info.pNext = nullptr;

        std::lock_guard<std::mutex> logical_device_guard(this->logical_device_.mutex);
        this->pipeline_cache_ = this->logical_device_.synced_object.createPipelineCache(pipeline_cache_info);
    }


    void
        Vulkan_Renderer::create_render_pass(void)
    {
        /*
        An attachment description describes the properties of an attachment
        including its format, sample count, and how its contents are treated at
        the beginning and end of each render pass instance.

        [Vulkan 1.1.94 Specification - Section 7 - Render Pass]
        */

        std::vector<vk::AttachmentDescription> attachments;
        attachments.push_back(vk::AttachmentDescription());
        attachments.push_back(vk::AttachmentDescription());
        
        // Color
        attachments[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;
        attachments[0].flags;
        attachments[0].format;
        attachments[0].initialLayout = vk::ImageLayout::eUndefined;
        attachments[0].loadOp = vk::AttachmentLoadOp::eClear;
        attachments[0].samples = vk::SampleCountFlagBits::e1;
        attachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        attachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        attachments[0].storeOp = vk::AttachmentStoreOp::eStore;

        // Depth stencil
        attachments[0].finalLayout = vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal;
        attachments[0].flags;
        attachments[0].format;
        attachments[0].initialLayout = vk::ImageLayout::eUndefined;
        attachments[0].loadOp = vk::AttachmentLoadOp::eClear;
        attachments[0].samples = vk::SampleCountFlagBits::e1;
        attachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        attachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        attachments[0].storeOp = vk::AttachmentStoreOp::eDontCare;

        vk::AttachmentDescription colour_attachment;
        colour_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
        colour_attachment.flags;
        colour_attachment.format = vk::Format::eB8G8R8A8Unorm;
        colour_attachment.initialLayout = vk::ImageLayout::eUndefined;
        colour_attachment.loadOp = vk::AttachmentLoadOp::eLoad;
        colour_attachment.samples = vk::SampleCountFlagBits::e1;
        colour_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colour_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colour_attachment.storeOp = vk::AttachmentStoreOp::eStore;

        vk::AttachmentReference colour_attachment_reference;
        colour_attachment_reference.attachment = 0;
        colour_attachment_reference.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::AttachmentReference depth_attachment_reference;
        depth_attachment_reference.attachment = 1;
        depth_attachment_reference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::SubpassDescription subpass_description;
        subpass_description.colorAttachmentCount = 1;
        subpass_description.flags;
        subpass_description.inputAttachmentCount = 0;
        subpass_description.pColorAttachments = &colour_attachment_reference;
        subpass_description.pDepthStencilAttachment = &depth_attachment_reference;
        subpass_description.pInputAttachments = nullptr;
        subpass_description.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass_description.pPreserveAttachments = nullptr;
        subpass_description.preserveAttachmentCount = 0;
        subpass_description.pResolveAttachments = nullptr;

        std::array<vk::SubpassDependency, 2> subpass_dependencies;
        subpass_dependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;
        subpass_dependencies[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
        subpass_dependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        subpass_dependencies[0].dstSubpass = 0;
        subpass_dependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
        subpass_dependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
        subpass_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        subpass_dependencies[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;
        subpass_dependencies[1].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
        subpass_dependencies[1].dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
        subpass_dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        subpass_dependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
        subpass_dependencies[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        subpass_dependencies[1].srcSubpass = 0;

        /*
        A render pass represents a collection of attachments, subpasses, and
        dependencies between the subpasses, and describes how the attachments
        are used over the course of the subpasses. The use of a render pass in
        a command buffer is a render pass instance.

        [Vulkan 1.1.94 Specification - Section 7 - Render Pass]
        */

        vk::RenderPassCreateInfo render_pass_info;
        render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        render_pass_info.dependencyCount = static_cast<uint32_t>(subpass_dependencies.size());
        render_pass_info.flags;
        render_pass_info.pAttachments = attachments.data();
        render_pass_info.pDependencies = subpass_dependencies.data();
        render_pass_info.pNext = nullptr;
        render_pass_info.pSubpasses = &subpass_description;
        render_pass_info.subpassCount = 1;

        std::lock_guard<std::mutex> logical_device_guard(this->logical_device_.mutex);
        this->render_pass_ = this->logical_device_.synced_object.createRenderPass(render_pass_info);
    }


    void
    Vulkan_Renderer::create_shader_modules(void)
    {
        /*
        Shader modules contain shader code and one or more entry points.
        Shaders are selected from a shader module by specifying an entry point
        as part of pipeline creation. The stages of a pipeline can use shaders
        that come from different modules. The shader code defining a shader
        module must be in the SPIR-V format, as described by the Vulkan
        Environment for SPIR-V appendix.

        [Vulkan 1.1.94 Specification - Section 8.1 - Shader Modules]
        */

        Penguin::Sample::Shader_Source vertex_source = this->load_shader_code("../samples/Vulkan/vert.spv");
        vk::ShaderModuleCreateInfo vertex_shader_module_info;
        vertex_shader_module_info.codeSize = vertex_source.size;
        vertex_shader_module_info.flags;
        vertex_shader_module_info.pCode = reinterpret_cast<uint32_t*>(vertex_source.code);
        vertex_shader_module_info.pNext = nullptr;

        Penguin::Sample::Shader_Source fragment_source = this->load_shader_code("../samples/Vulkan/frag.spv");
        vk::ShaderModuleCreateInfo fragment_shader_module_info;
        fragment_shader_module_info.codeSize = fragment_source.size;
        fragment_shader_module_info.flags;
        fragment_shader_module_info.pCode = reinterpret_cast<uint32_t*>(fragment_source.code);
        fragment_shader_module_info.pNext = nullptr;

        std::lock_guard<std::mutex> logical_device_guard(this->logical_device_.mutex);
        this->vertex_shader_module_ = this->logical_device_.synced_object.createShaderModule(vertex_shader_module_info);
        this->fragment_shader_module_ = this->logical_device_.synced_object.createShaderModule(fragment_shader_module_info);
    }


    void
    Vulkan_Renderer::create_swapchain(void)
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
        vk::SurfaceFormatKHR swapchain_format = this->validate_swapchain_format(desired_format);
 
        vk::SwapchainCreateInfoKHR swapchain_creation_info;
        swapchain_creation_info.flags = vk::SwapchainCreateFlagsKHR();
        swapchain_creation_info.surface = this->presentation_surface_;
        swapchain_creation_info.minImageCount = this->get_swapchain_image_count();
        swapchain_creation_info.imageFormat = swapchain_format.format;
        swapchain_creation_info.imageColorSpace = swapchain_format.colorSpace;
        swapchain_creation_info.imageExtent = this->validate_swapchain_image_size(this->rect_to_extent(this->window_->get_client_area()));
        swapchain_creation_info.imageArrayLayers = 1;
        swapchain_creation_info.imageUsage = this->validate_swapchain_image_usages(desired_image_usages);
        swapchain_creation_info.imageSharingMode = vk::SharingMode::eExclusive;
        swapchain_creation_info.queueFamilyIndexCount = 0;
        swapchain_creation_info.pQueueFamilyIndices = nullptr;
        swapchain_creation_info.preTransform = this->validate_swapchain_image_transform(desired_image_transform);
        swapchain_creation_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        swapchain_creation_info.presentMode = this->validate_presentation_mode(vk::PresentModeKHR::eMailbox);
        swapchain_creation_info.clipped = VK_TRUE;

        {
            std::scoped_lock guard(this->logical_device_.mutex, this->swapchain_.mutex);
            swapchain_creation_info.oldSwapchain = this->swapchain_.synced_object;

            this->swapchain_.synced_object = this->logical_device_.synced_object.createSwapchainKHR(swapchain_creation_info);
            this->swapchain_images_ = this->logical_device_.synced_object.getSwapchainImagesKHR(this->swapchain_.synced_object);
        }

    }


    void
        Vulkan_Renderer::end_command_buffer_recording(size_t buffer_index)
    {
        std::lock_guard guard(this->command_pool_.mutex);
        this->command_pool_.command_buffers[buffer_index].end();
    }


    uint32_t
        Vulkan_Renderer::get_swapchain_image_index(void)
    {
        vk::Fence     fence;

        std::scoped_lock guard(this->logical_device_.mutex, this->swapchain_.mutex);
        auto image_result = this->logical_device_.synced_object.acquireNextImageKHR(
            this->swapchain_.synced_object,
            acquire_swapchain_image_timeout_nanoseconds,
            this->presentation_semaphore_,
            fence
        );

        switch (image_result.result)
        {
        case vk::Result::eSuccess:
            return image_result.value;
        case vk::Result::eSuboptimalKHR:
            std::cerr << "ERROR! Swapchain is in a suboptimal state and should be recreated." << '\n';
            return image_result.value;
        case vk::Result::eErrorOutOfDateKHR:
            std::cerr << "ERROR! Swapchain is out of date, needs to be destroyed and recreated." << '\n';
            return 0xFFFFFFFF;
        default:
            std::cerr << "ERROR! Acquiring image from swapchain generated an unusual result (" << vk::to_string(image_result.result) << ")." << '\n';
            return 0xFFFFFFFF;
        }
    }


    uint32_t
        Vulkan_Renderer::get_swapchain_image_count(void) const
    {
        vk::SurfaceCapabilitiesKHR surface_capabilites = this->physical_device_.getSurfaceCapabilitiesKHR(this->presentation_surface_);

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
    Vulkan_Renderer::initialise(std::shared_ptr<Window> window)
    {
        this->window_ = window;

        this->create_instance();
        this->create_physical_device();
        this->create_presentation_surface();
        this->set_queue_family();
        this->create_logical_device();
        this->create_swapchain();
        this->create_graphics_queue();
        this->create_command_pool();
        this->create_command_buffers();
        this->create_shader_modules();
        this->create_render_pass();
        this->create_pipeline_cache();
        this->create_pipeline();
    }


    Penguin::Sample::Shader_Source
    Vulkan_Renderer::load_shader_code(const std::string& path)
    {
        Shader_Source shader_source;
        std::ifstream file_stream(path, std::ios::binary | std::ios::in | std::ios::ate);

        if (file_stream.is_open())
        {
            shader_source.size = file_stream.tellg();
            file_stream.seekg(0, std::ios::beg);
            shader_source.code = new char[shader_source.size];
            file_stream.read(shader_source.code, shader_source.size);
            file_stream.close();
            return shader_source;
        }
        else
        {
            std::stringstream error_stream;
            error_stream << "Failed to open file '" << path.c_str() << "' to read shader source!";
            throw std::runtime_error(error_stream.str());
        }
    }


    void
    Vulkan_Renderer::present(void)
    {
        vk::PresentInfoKHR presentation_info;
        presentation_info.pImageIndices = &this->swapchain_image_index_;
        presentation_info.pResults = nullptr;
        presentation_info.pSwapchains = &this->swapchain_.synced_object;
        presentation_info.pWaitSemaphores = &this->presentation_semaphore_;
        presentation_info.swapchainCount = 1;
        presentation_info.waitSemaphoreCount = 1;

        vk::Result presentation_result = this->graphics_queue_.presentKHR(presentation_info);
        if (presentation_result != vk::Result::eSuccess)
        {
            std::cerr << "ERROR! Call to present was not successful (" << vk::to_string(presentation_result) << ")." << '\n';
        }
    }


    vk::Extent2D
    Vulkan_Renderer::rect_to_extent(const RECT& rect) const
    {
        vk::Extent2D extent;
        extent.width = rect.right - rect.left;
        extent.height = rect.bottom - rect.top;
        return extent;
    }


    void
    Vulkan_Renderer::run(void)
    {
    }


    void
        Vulkan_Renderer::set_queue_family(void)
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
        auto queue_family_properties = this->physical_device_.getQueueFamilyProperties();

        for (uint32_t index = 0; index < queue_family_properties.size(); ++index)
        {
            if ((queue_family_properties[index].queueFlags & vk::QueueFlagBits::eGraphics) &&
                queue_family_properties[index].queueCount > 0)
            {
                if (VK_TRUE == this->physical_device_.getSurfaceSupportKHR(index, this->presentation_surface_))
                {
                    this->queue_family_index_ = index;
                    return;
                }
            }
        }

        throw std::runtime_error("Unable to find suitable queue family");
    }


    std::vector<const char*>
        Vulkan_Renderer::validate_device_extensions(const std::vector<const char*>& desired_extensions) const
    {
        auto available_device_extensions = this->physical_device_.enumerateDeviceExtensionProperties();
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
        Vulkan_Renderer::validate_device_layers(const std::vector<const char*>& desired_layers) const
    {
        auto available_device_layers = this->physical_device_.enumerateDeviceLayerProperties();
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
        Vulkan_Renderer::validate_instance_extensions(const std::vector<const char*>& desired_extensions) const
    {
        /*
        Extensions may define new Vulkan commands, structures, and enumerants.
        For compilation purposes, the interfaces defined by registered extensions,
        including new structures and enumerants as well as function pointer types
        for new commands, are defined in the Khronos-supplied core.h
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
        Vulkan_Renderer::validate_presentation_mode(const vk::PresentModeKHR& desired_mode) const
    {
        std::vector<vk::PresentModeKHR> presentation_modes = this->physical_device_.getSurfacePresentModesKHR(this->presentation_surface_);
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
        Vulkan_Renderer::validate_swapchain_format(const vk::SurfaceFormatKHR& desired_format) const
    {
        auto supported_formats = this->physical_device_.getSurfaceFormatsKHR(this->presentation_surface_);

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
        Vulkan_Renderer::validate_swapchain_image_size(const vk::Extent2D& desired_image_size) const
    {
        vk::SurfaceCapabilitiesKHR surface_capabilites = this->physical_device_.getSurfaceCapabilitiesKHR(this->presentation_surface_);
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
        Vulkan_Renderer::validate_swapchain_image_transform(const vk::SurfaceTransformFlagBitsKHR& desired_image_transform) const
    {
        vk::SurfaceCapabilitiesKHR surface_capabilites = this->physical_device_.getSurfaceCapabilitiesKHR(this->presentation_surface_);

        vk::SurfaceTransformFlagsKHR swapchain_image_transform = desired_image_transform & surface_capabilites.supportedTransforms;
        if (vk::SurfaceTransformFlagsKHR(desired_image_transform) != swapchain_image_transform)
        {
            std::cerr << "ERROR! Swapchain does not support image transform = " << vk::to_string(desired_image_transform) << '\n';
            return surface_capabilites.currentTransform;
        }
        return desired_image_transform;
    }


    vk::ImageUsageFlags
        Vulkan_Renderer::validate_swapchain_image_usages(const vk::ImageUsageFlags& desired_image_usages) const
    {
        vk::SurfaceCapabilitiesKHR surface_capabilites = this->physical_device_.getSurfaceCapabilitiesKHR(this->presentation_surface_);

        vk::ImageUsageFlags swapchain_image_usages = desired_image_usages & surface_capabilites.supportedUsageFlags;
        if (desired_image_usages != swapchain_image_usages)
        {
            std::cerr << "ERROR! Swapchain does not support image usage = " << vk::to_string(desired_image_usages ^ swapchain_image_usages) << '\n';
        }
        return swapchain_image_usages;
    }


    std::vector<const char*>
        Vulkan_Renderer::validate_validation_layers(const std::vector<const char*>& desired_validation_layers) const
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