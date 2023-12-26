#pragma once

#include <glm/glm.hpp>

namespace Rvl
{
    class Transform
    {
    public:
        Transform();
        Transform(const glm::vec3& position, const glm::vec3& rotationDeg, const glm::vec3& scale);
        ~Transform();

        bool operator== (const Transform& other);

        glm::mat4 GetMatrix() const;

        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::vec3 Scale;

        glm::vec3 Pivot;
    };
}
