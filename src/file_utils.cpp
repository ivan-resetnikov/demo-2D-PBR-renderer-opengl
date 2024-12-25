#include "file_utils.h"

namespace Engine
{
    std::string read_text_file(const std::string& path)
    {
        std::ifstream file_stream(path);
        std::string content;
        content.assign(std::istreambuf_iterator<char>(file_stream), std::istreambuf_iterator<char>());
        
        return content;
    }
}