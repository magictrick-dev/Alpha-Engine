#include <platform/renderer/framebuffer.hpp>

#include <glad/glad.h>

#include <cassert>

struct Framebuffer
{
    FramebufferSpecification specification;
    GLuint frame_buffer;
    GLuint color_buffer;
    GLuint attachment_buffer;
    bool color_buffer_is_texture = false;
};

static inline GLuint
get_color_format(FramebufferColorFormatType type)
{

    switch (type)
    {
        case FramebufferColorFormatType_RGBA: return GL_RGBA;   
        case FramebufferColorFormatType_sRGBA: return GL_SRGB_ALPHA;
        case FramebufferColorFormatType_Float16: return GL_RGBA16F;
        case FramebufferColorFormatType_Float32: return GL_RGBA32F;
        default: assert(!"unimplemented type");
    }

    return 0;

}

static inline framebuffer_t
pde_renderer_create_trivial_framebuffer(FramebufferSpecification *specification)
{

    GLuint framebuffer;
    GLuint color_buffer;
    GLuint attachment_buffer;

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    if (specification->color_format != FramebufferColorFormatType_None)
    {

        glGenTextures(1, &color_buffer);
        glBindTexture(GL_TEXTURE_2D, color_buffer);
          
        GLuint color_format = get_color_format(specification->color_format);
        glTexImage2D(GL_TEXTURE_2D, 0, color_format, specification->width, 
                specification->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
                color_buffer, 0);

    }

    if (specification->attachment_format != FramebufferAttachmentFormatType_None)
    {

        glGenRenderbuffers(1, &attachment_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, attachment_buffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                specification->width, specification->height);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 
                GL_RENDERBUFFER, attachment_buffer);

    }

    GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status == GL_FRAMEBUFFER_COMPLETE)
    {

        Framebuffer *result = new Framebuffer;
        result->frame_buffer = framebuffer;
        result->color_buffer = color_buffer;
        result->attachment_buffer = attachment_buffer;
        result->specification = *specification;

        return result;

    }

    glDeleteFramebuffers(1, &framebuffer);
    if (color_buffer) glDeleteTextures(1, &color_buffer);
    if (attachment_buffer) glDeleteRenderbuffers(1, &attachment_buffer);
    return PDE_INVALID_FRAMEBUFFER;

}

framebuffer_t 
pde_renderer_create_framebuffer(FramebufferSpecification *specification)
{

    if (specification == NULL) return PDE_INVALID_FRAMEBUFFER;

    if (specification->multisample_type == FramebufferMultisampleType_Disabled)
        return pde_renderer_create_trivial_framebuffer(specification);

    uint32_t samples = 0;
    switch (specification->multisample_type)
    {
        case FramebufferMultisampleType_MSAA2: samples = 2; break;
        case FramebufferMultisampleType_MSAA4: samples = 4; break;
        case FramebufferMultisampleType_MSAA8: samples = 8; break;
        case FramebufferMultisampleType_MSAA16: samples = 16; break;
        default: assert(!"Unimplemented case.");
    }

    GLuint framebuffer;
    GLuint color_buffer;
    GLuint attachment_buffer;

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    if (specification->color_format != FramebufferColorFormatType_None)
    {
        glGenRenderbuffers(1, &color_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, color_buffer);

        GLuint color_format = get_color_format(specification->color_format);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, color_format, 
                specification->width, specification->height);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                GL_RENDERBUFFER, color_buffer);
    }

    if (specification->attachment_format != FramebufferAttachmentFormatType_None)
    {

        glGenRenderbuffers(1, &attachment_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, attachment_buffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8,
                specification->width, specification->height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 
                GL_RENDERBUFFER, attachment_buffer);

    }

    GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status == GL_FRAMEBUFFER_COMPLETE)
    {

        Framebuffer *result = new Framebuffer;
        result->frame_buffer = framebuffer;
        result->color_buffer = color_buffer;
        result->attachment_buffer = attachment_buffer;
        result->specification = *specification;

        return result;

    }

    glDeleteFramebuffers(1, &framebuffer);
    if (color_buffer) glDeleteRenderbuffers(1, &color_buffer);
    if (attachment_buffer) glDeleteRenderbuffers(1, &attachment_buffer);
    return PDE_INVALID_FRAMEBUFFER;

}

void 
pde_renderer_destroy_framebuffer(framebuffer_t handle)
{

    Framebuffer *framebuffer = (Framebuffer*)handle;
    assert(framebuffer != NULL);

    if (framebuffer->color_buffer != 0)
    {
        if (framebuffer->color_buffer_is_texture) glDeleteTextures(1, &framebuffer->color_buffer);
        else glDeleteRenderbuffers(1, &framebuffer->color_buffer);
    }

    if (framebuffer->attachment_buffer != 0)
    {
        glDeleteRenderbuffers(1, &framebuffer->attachment_buffer);
    }

    glDeleteFramebuffers(1, &framebuffer->frame_buffer);

}

int32_t 
pde_renderer_get_framebuffer_width(framebuffer_t handle)
{

    Framebuffer *framebuffer = (Framebuffer*)handle;
    return framebuffer->specification.width;

}

int32_t 
pde_renderer_get_framebuffer_height(framebuffer_t handle)
{

    Framebuffer *framebuffer = (Framebuffer*)handle;
    return framebuffer->specification.height;

}

void 
pde_renderer_blit_framebuffer_to_target_framebuffer(framebuffer_t handle)
{

    Framebuffer *framebuffer = (Framebuffer*)handle;

    if (framebuffer->specification.target == NULL)
    {
        // The target is the default framebuffer.
        int32_t width = framebuffer->specification.width;
        int32_t height = framebuffer->specification.height;
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer->frame_buffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    else
    {
        // Probably a textured surface.
        Framebuffer *other = (Framebuffer*)framebuffer->specification.target;
        int32_t width = framebuffer->specification.width;
        int32_t height = framebuffer->specification.height;
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer->frame_buffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, other->frame_buffer);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, NULL);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, NULL);

}

bool 
pde_renderer_framebuffer_is_trivial(framebuffer_t handle)
{

    Framebuffer *framebuffer = (Framebuffer*)handle;
    return framebuffer->color_buffer_is_texture;

}

void 
pde_renderer_bind_framebuffer(framebuffer_t handle)
{

    if (handle == NULL)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, NULL);
        return;
    }

    Framebuffer *framebuffer = (Framebuffer*)handle;
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->frame_buffer);

}

