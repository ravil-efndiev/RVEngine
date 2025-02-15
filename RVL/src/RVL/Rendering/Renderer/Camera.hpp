#pragma once

#include <glm/glm.hpp>

namespace Rvl
{
    class Camera
    {
    public:
        virtual glm::mat4 GetProjectionMatrix(float, float) const = 0;
        virtual glm::mat4 GetViewMatrix() const = 0;
        virtual glm::vec3 GetPosition() const = 0;
    };
}

