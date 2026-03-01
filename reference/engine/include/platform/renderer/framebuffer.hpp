#pragma once
#include <cstdint>

typedef void* framebuffer_t;
#define PDE_INVALID_FRAMEBUFFER NULL

enum FramebufferColorFormatType : uint32_t
{
    FramebufferColorFormatType_None,
    FramebufferColorFormatType_RGBA,
    FramebufferColorFormatType_sRGBA,
    FramebufferColorFormatType_Float16,
    FramebufferColorFormatType_Float32,
};

enum FramebufferAttachmentFormatType : uint32_t
{
    FramebufferAttachmentFormatType_None,
    FramebufferAttachmentFormatType_Depth24_Stencil8,
};

enum FramebufferMultisampleType : uint32_t
{
    FramebufferMultisampleType_Disabled,
    FramebufferMultisampleType_MSAA2,
    FramebufferMultisampleType_MSAA4,
    FramebufferMultisampleType_MSAA8,
    FramebufferMultisampleType_MSAA16,
};

struct FramebufferSpecification
{

    FramebufferColorFormatType color_format;
    FramebufferAttachmentFormatType attachment_format;
    FramebufferMultisampleType multisample_type;
    framebuffer_t target;
    uint32_t width;
    uint32_t height;

};

framebuffer_t pde_renderer_create_framebuffer(FramebufferSpecification *specification);
void pde_renderer_destroy_framebuffer(framebuffer_t handle);
int32_t pde_renderer_get_framebuffer_width(framebuffer_t handle);
int32_t pde_renderer_get_framebuffer_height(framebuffer_t handle);
void pde_renderer_blit_framebuffer_to_target_framebuffer(framebuffer_t handle);
bool pde_renderer_framebuffer_is_trivial(framebuffer_t handle);
void pde_renderer_bind_framebuffer(framebuffer_t handle);

