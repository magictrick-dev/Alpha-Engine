#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <cassert>
#include <unordered_map>

enum TextureType : uint32_t
{
    TextureType_Diffuse,
    TextureType_Specular,
    TextureType_Normal,
    TextureType_Bump,
};

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

class TextureInterface
{

    public:
        inline virtual int32_t get_width() const = 0;
        inline virtual int32_t get_height() const = 0;
        inline virtual uint32_t get_handle() const = 0;
        inline virtual uint32_t get_format() const = 0;
        inline virtual uint32_t get_source_format() const = 0;
        inline virtual TextureType get_type() const = 0;
        inline virtual std::string get_name() const = 0;

        inline float aspect_ratio() const { return (float)get_width() / (float)get_height(); }

    public:
        static inline TextureInterface* CreateTextureFromFile(
                std::string path, 
                TextureType type,
                TextureSourceFormatType format);

        static inline TextureInterface* CreateTextureFromData(
                std::string name, 
                TextureType type,
                uint32_t source_format,
                uint32_t internal_format,
                uint8_t *data, 
                int32_t width, 
                int32_t height);

        static inline void DestroyTexture(TextureInterface *texture);
        static inline void DestroyTexture(std::string name);

        static inline bool TextureExistsByPath(std::string path);
        static inline bool TextureExistsByName(std::string name);

        static inline TextureInterface* GetWhiteFallbackTexture();
        static inline TextureInterface* GetBlackFallbackTexture();

        static inline std::vector<TextureInterface*> AvailableTextures;
        static inline std::unordered_map<std::string, TextureInterface*> TextureAssociations;


};

