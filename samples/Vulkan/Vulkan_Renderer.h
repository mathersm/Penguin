/*
* Copyright (c) 2019 Michael Mathers
*/
#ifndef PENGUIN_SAMPLE_VULKAN_RENDERER_H
#define PENGUIN_SAMPLE_VULKAN_RENDERER_H


#include "Host_Synchronized.h"
#include "Vulkan_Config.h"
#include "Window.h"
#include <mutex>
#include <string>
#include <vector>


namespace Penguin::Sample
{
    struct Shader_Source
    {
        size_t  size;
        char*   code;

        Shader_Source(void);
        ~Shader_Source(void);

        Shader_Source(Shader_Source&& source);
        Shader_Source& operator = (Shader_Source&& source);

        Shader_Source(const Shader_Source&) = delete;
        Shader_Source& operator = (const Shader_Source&) = delete;

    };


    struct Synchronized_Command_Pool
    {
        std::mutex mutex;
        vk::CommandPool command_pool;
        std::vector<vk::CommandBuffer> command_buffers;
    };


    struct Vertex
    {
        float position[3];
        float color[3];
    };


    class Vulkan_Renderer
    {
    public:
        Vulkan_Renderer(void);
        ~Vulkan_Renderer(void);

    public:
        void close(void);
        void initialise(std::shared_ptr<Window> window);
        void run(void);

    private:
        Vulkan_Renderer(const Vulkan_Renderer&) = delete;
        Vulkan_Renderer& operator = (const Vulkan_Renderer&) = delete;

    private:
        void begin_command_buffer_recording(size_t buffer_index);

        
        void create_command_buffers(void);
        void create_command_pool(void);
        void create_framebuffer(void);
        void create_graphics_queue(void);
        void create_instance(void);
        void create_logical_device(void);
        void create_physical_device(void);
        void create_pipeline(void);
        void create_pipeline_cache(void);
        void create_presentation_surface(void);
        void create_render_pass(void);
        void create_shader_modules(void);
        void create_swapchain(void);

        vk::PipelineColorBlendStateCreateInfo               define_color_blend_state(const std::vector<vk::PipelineColorBlendAttachmentState>& attachment_states) const;
        vk::PipelineDepthStencilStateCreateInfo             define_depth_stencil_state(void) const;
        vk::PipelineDynamicStateCreateInfo                  define_dynamic_state(const std::vector<vk::DynamicState>& dynamic_states) const;
        vk::PipelineInputAssemblyStateCreateInfo            define_input_assembly_state(void) const;
        vk::PipelineMultisampleStateCreateInfo              define_multisample_state(void) const;
        vk::PipelineRasterizationStateCreateInfo            define_rasterization_state(void) const;
        std::vector<vk::VertexInputAttributeDescription>    define_vertex_input_attributes(void) const;
        std::vector<vk::VertexInputBindingDescription>      define_vertex_input_binding(void) const;
        vk::PipelineVertexInputStateCreateInfo              define_vertex_input_state(const std::vector<vk::VertexInputBindingDescription>& vertex_input_binding, const std::vector<vk::VertexInputAttributeDescription>& vertex_input_attributes) const;
        vk::PipelineViewportStateCreateInfo                 define_viewport_state(void) const;

        void end_command_buffer_recording(size_t buffer_index);

        uint32_t get_swapchain_image_index(void);
        uint32_t get_swapchain_image_count(void) const;

        Penguin::Sample::Shader_Source load_shader_code(const std::string& path);

        void present(void);

        void set_queue_family(void);

        std::vector<const char*>        validate_device_extensions(const std::vector<const char*>& desired_extensions) const;
        std::vector<const char*>        validate_device_layers(const std::vector<const char*>& desired_layers) const;
        std::vector<const char*>        validate_instance_extensions(const std::vector<const char*>& desired_extensions) const;
        vk::PresentModeKHR              validate_presentation_mode(const vk::PresentModeKHR& desired_mode) const;
        vk::SurfaceFormatKHR            validate_swapchain_format(const vk::SurfaceFormatKHR& desired_format) const;
        vk::Extent2D                    validate_swapchain_image_size(const vk::Extent2D& desired_image_size) const;
        vk::SurfaceTransformFlagBitsKHR validate_swapchain_image_transform(const vk::SurfaceTransformFlagBitsKHR& desired_image_transform) const;
        vk::ImageUsageFlags             validate_swapchain_image_usages(const vk::ImageUsageFlags& desired_image_usages) const;
        std::vector<const char*>        validate_validation_layers(const std::vector<const char*>& desired_validation_layers) const;

    private:
        vk::Extent2D    rect_to_extent(const RECT& rect) const;

    private:
        std::shared_ptr<Penguin::Sample::Window> window_;

        Penguin::Sample::Host_Synchronized<vk::Instance> instance_;
        vk::PhysicalDevice physical_device_;
        vk::SurfaceKHR presentation_surface_;
        uint32_t queue_family_index_;
        Penguin::Sample::Host_Synchronized<vk::Device> logical_device_;
        Penguin::Sample::Host_Synchronized<vk::SwapchainKHR> swapchain_;
        std::vector<vk::Image> swapchain_images_;
        uint32_t swapchain_image_index_;
        vk::Semaphore presentation_semaphore_;
        vk::Queue graphics_queue_;
        Penguin::Sample::Synchronized_Command_Pool command_pool_;
        vk::RenderPass  render_pass_;
        vk::ShaderModule vertex_shader_module_;
        vk::ShaderModule fragment_shader_module_;
        vk::PipelineCache pipeline_cache_;
        vk::Pipeline    pipeline_;
    };
}


#endif // PENGUIN_SAMPLE_VULKAN_RENDERER_H
