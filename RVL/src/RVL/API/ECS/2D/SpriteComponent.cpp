#include "SpriteComponent.hpp"

namespace Rvl
{
    SpriteComponent::SpriteComponent(Entity* self, const std::string& path, float scale) : Component(self)
    {
        RVL_ASSERT((_self->HasComponent<TransformComponent>()), "target entity doesn't have transform component");

        _scale = scale;
        LoadTexture(path);
    }
    
    SpriteComponent::SpriteComponent(Entity* self, const Ref<GLTexture>& texture, float scale) : Component(self)
    {
        RVL_ASSERT((_self->HasComponent<TransformComponent>()), "target entity doesn't have transform component");

        _scale = scale;
        LoadTexture(texture);
    }

    SpriteComponent::SpriteComponent(Entity* self, const glm::vec4& color) : Component(self)
    {
        RVL_ASSERT((_self->HasComponent<TransformComponent>()), "target entity doesn't have transform component");

        _scale = 1.f;
        _color = color;
        _drawType = DrawType::Color;
    }

    void SpriteComponent::LoadTexture(const std::string& path)
    {
        _drawType = DrawType::Texture;

        _texture = NewRef<GLTexture>(path);
        ResetScale();

        _subTexture = SubTexture::New(_texture, 0.f, 0.f, _texture->GetWidth(), _texture->GetHeight());
    }

    void SpriteComponent::LoadTexture(const Ref<GLTexture>& texture)
    {
        _drawType = DrawType::Texture;

        _texture = texture;
        ResetScale();

        _subTexture = SubTexture::New(_texture, 0.f, 0.f, _texture->GetWidth(), _texture->GetHeight());
    }

    void SpriteComponent::SetSubTexture(float x, float y, float spriteWidth, float spriteHeight)
    {
        RVL_ASSERT((_drawType == DrawType::Texture), "Cannot load subtexture into sprite without base texture");
        _subTexture = SubTexture::New(_texture, x, y, spriteWidth, spriteHeight);

        float ratio = spriteWidth / spriteHeight;
        _self->GetComponent<TransformComponent>().Scale = glm::vec2(ratio * _scale, _scale);
    }

    void SpriteComponent::SetSubTexture(const Ref<SubTexture>& subTexture)
    {
        RVL_ASSERT((_drawType == DrawType::Texture), "Cannot load subtexture into sprite without base texture");
        _subTexture = SubTexture::New(_texture, subTexture->GetX(), subTexture->GetY(), subTexture->GetWidth(), subTexture->GetHeight());

        float ratio = subTexture->GetWidth() / subTexture->GetHeight();
        _self->GetComponent<TransformComponent>().Scale = glm::vec2(ratio * _scale, _scale);
    }

    void SpriteComponent::ResetScale()
    {
        RVL_ASSERT((_drawType == DrawType::Texture), "Cannot reset scale based on texture in sprite without texture");
        float ratio = (float)_texture->GetWidth() / (float)_texture->GetHeight();

        _self->GetComponent<TransformComponent>().Scale = glm::vec2(ratio * _scale, _scale);
    }

    void SpriteComponent::ResetSubTexture()
    {
        RVL_ASSERT((_drawType == DrawType::Texture), "Cannot reset subtexture in sprite without texture");
        ResetScale();
        _subTexture = SubTexture::New(_texture, 0.f, 0.f, _texture->GetWidth(), _texture->GetHeight());
    }  

    Ref<GLTexture> SpriteComponent::GetTexture() const
    {
        return _texture;
    }

    Ref<SubTexture> SpriteComponent::GetSubTexture() const
    {
        return _subTexture;
    }
    
    void SpriteComponent::SetColor(const glm::vec4& color)
    {
        _color = color;
    }

    glm::vec4 SpriteComponent::GetColor() const
    {
        return _color;
    }
    
    SpriteComponent::DrawType SpriteComponent::GetDrawType() const
    {
        return _drawType;
    }
    
    void SpriteComponent::UseColorAsTint(bool flag)
    {
        _useColorAsTint = flag;
    }

    bool SpriteComponent::ColorIsTint() const
    {
        return _useColorAsTint;
    }
}