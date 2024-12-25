#include <fstream>
#include <string>

#include <glad/glad.h>

#include "logging.h"

namespace Engine
{
    std::string read_text_file(const std::string& path);
}