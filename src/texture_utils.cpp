#include "texture_utils.h"
#include <stb/stb_image.h>

namespace Engine
{
    GLuint load_texture(const char* path, GLint wrap_mode, GLint min_filter_mode, GLint mag_filter_mode, GLenum texture_format, GLint internal_format, TextureInfo& texture_info)
    {
        stbi_set_flip_vertically_on_load(true);
        
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter_mode);

        int width, height, color_channel_count;
        unsigned char *data = stbi_load(path, &width, &height, &color_channel_count, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, texture_format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            log_error("[TEXTURE] Could not load texture from `" + (std::string)path + "`!");
        }

        texture_info.width = (uintmax_t)width;
        texture_info.height = (uintmax_t)height;

        stbi_image_free(data);

        return texture;
    }

    GLuint load_texture(const char* path, GLint wrap_mode, GLint min_filter_mode, GLint mag_filter_mode, GLenum texture_format, GLint internal_format)
    {
        stbi_set_flip_vertically_on_load(true);
        
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter_mode);

        int width, height, color_channel_count;
        unsigned char *data = stbi_load(path, &width, &height, &color_channel_count, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, texture_format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            log_error("[TEXTURE] Could not load texture from `" + (std::string)path + "`!");
        }

        stbi_image_free(data);

        return texture;
    }
}