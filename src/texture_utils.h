#include "typedefs.h"

#include <glad/glad.h>

#include "logging.h"

namespace Engine
{
    struct TextureInfo {
        uintmax_t width;
        uintmax_t height;
    };

    GLuint load_texture(const char* path, GLint wrap_mode, GLint min_filter_mode, GLint mag_filter_mode, GLenum texture_format, GLint internal_format, TextureInfo& texture_info);
}