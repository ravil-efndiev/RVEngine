#include "GLTexture.hpp"

#include <Rvlglpch.hpp>
#include <stb_image.h>

namespace Rvl
{
    GLuint GLTexture::TextureFromFile(const std::string& path, bool gamma)
    {
        stbi_set_flip_vertically_on_load(false);

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        i32 width, height, channels;
        byte* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (!data) 
            throw Error("GLTexture: Failed to load texture", RVL_INTERNAL_ERROR);

        GLenum dataFormat = channels == 3 ? GL_RGB : GL_RGBA;
        GLenum internalFormat = channels == 3 ? GL_RGB8 : GL_RGBA8;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);

        return textureID;
    }

    void GLTexture::BindTextureUnit(GLuint texture, GLuint sampler, i32 unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindSampler(GL_TEXTURE0 + unit, sampler);
    }

    void GLTexture::BindTextureUnit(GLuint texture, i32 unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void GLTexture::ActivateTexture(i32 unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
    }

    GLTexture::GLTexture() {}

    GLTexture::GLTexture(u32 width, u32 height)
    {
        _width = width;
        _height = height;

        glGenTextures(1, &_textureId);
        glBindTexture(GL_TEXTURE_2D, _textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        _dataFormat = GL_RGB8;
        _internalFormat = GL_RGB8;

        glTexImage2D(GL_TEXTURE_2D, 0, _dataFormat, _width, _height, 0, _dataFormat, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLTexture::GLTexture(const std::string &path)
    {       
        LoadTexture(path);
        _path = path;
    }

    GLTexture::~GLTexture() 
    {
        glDeleteTextures(1, &_textureId);
    }

    void GLTexture::LoadTexture(const std::string& path)
    {
        stbi_set_flip_vertically_on_load(true);

        byte* textureData = stbi_load(path.c_str(), &_width, &_height, &_channels, 0);
        if (!textureData)
        {
            throw Error("GLTexture: Failed to load texture", RVL_RUNTIME_ERROR);
        }

        glGenTextures(1, &_textureId);
        glBindTexture(GL_TEXTURE_2D, _textureId);

        if (_channels < 3 || _channels > 4)
            throw Error("GLTexture: Unsupported texture format", RVL_RUNTIME_ERROR);

        _dataFormat = _channels == 3 ? GL_RGB : GL_RGBA;
        _internalFormat = _channels == 3 ? GL_RGB8 : GL_RGBA8;

        RVL_LOG(_channels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _width, _height, 0, _dataFormat, GL_UNSIGNED_BYTE, textureData);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(textureData);
    }

    void GLTexture::SetData(byte* data, i32 channels)
    {
        RVL_ASSERT((channels == 4 || channels == 3), "GLTexture: number of channels in texture can be only 4 or 3");

        _channels = channels;
        _dataFormat = _channels == 4 ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, _textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, _dataFormat, _width, _height, 0, _dataFormat, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GLTexture::Bind(i32 unit) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, _textureId);
    }

    void GLTexture::Unbind(i32 unit) const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindSampler(GL_TEXTURE0 + unit, 0);
    }

    i32 GLTexture::GetWidth() const { return _width; }

    i32 GLTexture::GetHeight() const { return _height; }

    GLuint GLTexture::GetId() const { return _textureId; }


    std::string GLTexture::GetPath() const { return _path; }

    bool GLTexture::operator==(const GLTexture& tex) const
    {
        return _path == "" ? _textureId == tex._textureId : _path == tex._path;
    }
}
