#pragma once
#include <platform/filesystem.hpp>

#include <string>
#include <vector>

#include <stb/stb_image.h>

typedef void* texture_t;
#define PDE_INVALID_TEXTURE NULL

enum TextureFormatType : uint32_t
{
    TextureFormatType_RGB,
    TextureFormatType_RGBA,
    TextureFormatType_sRGB,
    TextureFormatType_sRGBA,
};

enum TextureSourceFormatType : uint32_t
{
    TextureSourceFormatType_RGBA,
};

struct TextureSpecification
{
    std::string name;
    uint8_t *data;
    uint32_t width;
    uint32_t height;
    uint32_t mips;
    TextureFormatType texture_format;
    TextureSourceFormatType source_format;
};

texture_t pde_renderer_create_texture(TextureSpecification *specification);
texture_t pde_renderer_search_for_texture(std::string name);
void pde_renderer_destroy_texture(texture_t texture);
void pde_renderer_get_texture_specification(texture_t handle, TextureSpecification *specification);
bool pde_renderer_get_texture_specification(std::string name, TextureSpecification *specification);
void* pde_renderer_get_raw_texture_handle(texture_t handle);
texture_t pde_renderer_get_default_texture();
texture_t pde_renderer_get_default_void_texture();
bool pde_renderer_texture_is_defaulted(texture_t handle);

inline texture_t 
pde_renderer_create_texture(std::string path, uint32_t mips = 1)
{

    std::string resolved_path = pde_filesystem_resolve(path);
    if (resolved_path == "") return PDE_INVALID_TEXTURE;
    if (!pde_filesystem_path_exists_as_file(resolved_path)) return PDE_INVALID_TEXTURE;
    texture_t search = pde_renderer_search_for_texture(resolved_path);
    if (search != PDE_INVALID_TEXTURE) return search;

    int32_t width, height, channels;
    uint8_t *data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (data == NULL) return PDE_INVALID_TEXTURE;

    TextureSpecification specification = {};
    specification.name = resolved_path;
    specification.source_format = TextureSourceFormatType_RGBA;
    specification.texture_format = TextureFormatType_sRGBA;
    specification.data = data;
    specification.width = width;
    specification.height = height;
    specification.mips = mips;

    // NOTE(Chris): This potentially could fail, but generally shouldn't by this point.
    texture_t result = pde_renderer_create_texture(&specification);

    stbi_image_free(data);

    return result;

}

