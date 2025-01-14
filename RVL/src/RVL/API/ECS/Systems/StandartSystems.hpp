#pragma once

#include "System.hpp"

namespace Rvl
{
    void TransformSystem(const std::vector<Entity>& entities);

    void SpriteSystem(const std::vector<Entity>& entities);

    void Movement2DSystem(const std::vector<Entity>& entities);

    void Animation2DSystem(const std::vector<Entity>& entities);

    void MaterialSystem(const std::vector<Entity>& entities);

    void ModelLoaderSystem(const std::vector<Entity>& entities);

    void ParticleSystem(const std::vector<Entity>& entities);
}
