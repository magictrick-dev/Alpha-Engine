#include <platform/renderer/texture.hpp>
#include <glad/glad.h>

#include <cassert>
#include <unordered_map>
#include <string>

struct Texture
{
    GLuint identifier;
    TextureSpecification specification;
};

static std::unordered_map<std::string, Texture*> texture_lookup;

static inline GLuint
convert_format(TextureFormatType type)
{
    switch (type)
    {
        case TextureFormatType_RGB: return GL_RGB;
        case TextureFormatType_RGBA: return GL_RGBA;
        case TextureFormatType_sRGB: return GL_SRGB;
        case TextureFormatType_sRGBA: return GL_SRGB_ALPHA;
        default: assert(!"Unimplemented format.");
    }
}

static inline GLuint
convert_source_format(TextureSourceFormatType format)
{

    switch (format)
    {
        case TextureSourceFormatType_RGBA: return GL_RGBA;
        default: assert(!"Unimplemented format.");
    }

}

texture_t 
pde_renderer_create_texture(TextureSpecification *specification)
{

    if (specification == NULL) return PDE_INVALID_TEXTURE;
    if (texture_lookup.find(specification->name) != texture_lookup.end())
        return texture_lookup[specification->name];

    GLuint internal_format = convert_format(specification->texture_format);
    GLuint source_format = convert_source_format(specification->source_format);
    
    GLuint texture_identifier;
    glGenTextures(1, &texture_identifier);

    glBindTexture(GL_TEXTURE_2D, texture_identifier);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, specification->width, specification->height,
            0, source_format, GL_UNSIGNED_BYTE, specification->data);

    glGenerateMipmap(GL_TEXTURE_2D);

    Texture *texture = new Texture();
    texture->identifier = texture_identifier;
    texture->specification = *specification;

    texture_lookup[specification->name] = texture;
    return texture;

}

void
pde_renderer_destroy_texture(texture_t handle)
{

    Texture *texture = (Texture*)handle;
    texture_lookup.erase(texture->specification.name);
    glDeleteTextures(1, &texture->identifier);
    delete texture;

}

texture_t 
pde_renderer_search_for_texture(std::string name)
{

    if (texture_lookup.find(name) == texture_lookup.end()) return PDE_INVALID_TEXTURE;
    return texture_lookup[name];
    
}

void 
pde_renderer_get_texture_specification(texture_t handle, TextureSpecification *specification)
{

    Texture *texture = (Texture*)handle;
    *specification = texture->specification;

}

bool 
pde_renderer_get_texture_specification(std::string name, TextureSpecification *specification)
{

    if (texture_lookup.find(name) == texture_lookup.end()) return false;

    *specification = texture_lookup[name]->specification;
    return true;

}

texture_t
pde_renderer_get_default_texture()
{

    static texture_t cached_texture = PDE_INVALID_TEXTURE;
    if (cached_texture != PDE_INVALID_TEXTURE) return cached_texture;

    uint32_t pixel = 0xFF'FF'FF'FF;

    TextureSpecification specification = {};
    specification.name = "default";
    specification.source_format = TextureSourceFormatType_RGBA;
    specification.texture_format = TextureFormatType_sRGBA;
    specification.data = (uint8_t*)&pixel;
    specification.width = 1;
    specification.height = 1;
    specification.mips = 1;

    cached_texture = pde_renderer_create_texture(&specification);
    return cached_texture;

}

texture_t
pde_renderer_get_default_void_texture()
{

    static texture_t cached_texture = PDE_INVALID_TEXTURE;
    if (cached_texture != PDE_INVALID_TEXTURE) return cached_texture;

    uint32_t pixel = 0x00'00'00'FF;

    TextureSpecification specification = {};
    specification.name = "default_void";
    specification.source_format = TextureSourceFormatType_RGBA;
    specification.texture_format = TextureFormatType_sRGBA;
    specification.data = (uint8_t*)&pixel;
    specification.width = 1;
    specification.height = 1;
    specification.mips = 1;

    cached_texture = pde_renderer_create_texture(&specification);
    return cached_texture;

}

void* 
pde_renderer_get_raw_texture_handle(texture_t handle)
{

    Texture *texture = (Texture*)handle;
    return (void*)((uint64_t)texture->identifier);

}

bool 
pde_renderer_texture_is_defaulted(texture_t handle)
{

    if (handle == pde_renderer_get_default_texture()) return true;
    if (handle == pde_renderer_get_default_void_texture()) return true;
    return false;

}
