#include "Math.hpp"

#include <API/Time.hpp>

namespace Rvl
{
    float Math::Lerp(float current, float target, float t)
    {
        return current * (1.f - t) + target * t;
    }

    float Math::Lerp(float current, float target, float t, float deadZone)
    {
        return (glm::abs(target - current) > deadZone) ? Lerp(current, target, t) : target;
    }

    glm::vec3 Math::Lerp(const glm::vec3& current, const glm::vec3& target, float t)
    {
        return current * (1.f - t) + target * t;
    }

    glm::vec4 Math::Lerp(const glm::vec4& current, const glm::vec4& target, float t)
    {
        return current * (1.f - t) + target * t;
    }
}
