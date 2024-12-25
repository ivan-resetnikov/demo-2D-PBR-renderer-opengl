#include <glm/glm.hpp>

namespace Engine
{
    struct PointLight {
        glm::vec3 color = glm::vec3(1.0f);
        glm::vec2 position = glm::vec2(0.0f);
        float energy = 1.0f;
        float radius = 256.0f;
        float height = 64.0f;

        struct {
            float linear = 0.0035f;
            float quadratic = 0.0001f;
        } attenuation;
    };
}