#include <engine/textures.hpp>
#include <glad/glad.h>

class Texture : public TextureInterface
{

    public:
        inline virtual int32_t get_width() const { return m_width; }
        inline virtual int32_t get_height() const { return m_height; }
        inline virtual uint32_t get_handle() const { return m_handle; }
        inline virtual uint32_t get_format() const { return m_internal_format; }
        inline virtual uint32_t get_source_format() const { return m_source_format; }
        inline virtual TextureType get_type() const { return m_type; }
        inline virtual std::string get_name() const { return m_name; }

        inline float aspect_ratio() const { return (float)get_width() / (float)get_height(); }

    private:
        uint32_t m_handle;

        std::string m_name;
        TextureType m_type;
        int32_t m_width;
        int32_t m_height;
        uint32_t m_internal_format;
        uint32_t m_source_format;

        friend TextureInterface;

};

TextureInterface* TextureInterface::
CreateTextureFromData(std::string name, 
        TextureType type, 
        uint32_t source_format,
        uint32_t internal_format,
        uint8_t *data,
        int32_t width,
        int32_t height)
{

    if (TextureAssociations.contains(name)) return TextureAssociations[name];

    GLuint texture_identifier;
    glGenTextures(1, &texture_identifier);

    glBindTexture(GL_TEXTURE_2D, texture_identifier);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, source_format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    Texture *binary_texture = new Texture();
    binary_texture->m_handle = texture_identifier;
    binary_texture->m_name = name;
    binary_texture->m_type = type;
    binary_texture->m_width = width;
    binary_texture->m_height = height;
    binary_texture->m_source_format = source_format;
    binary_texture->m_internal_format = internal_format;

    TextureAssociations[name] = binary_texture;
    return binary_texture;

}
