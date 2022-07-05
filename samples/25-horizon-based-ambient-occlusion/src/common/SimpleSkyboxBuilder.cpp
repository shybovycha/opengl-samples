#include "SimpleSkyboxBuilder.hpp"

SimpleSkyboxBuilder::SimpleSkyboxBuilder() :
    AbstractSkyboxBuilder()
{
}

SimpleSkyboxBuilder* SimpleSkyboxBuilder::top(std::string filename)
{
    m_top.loadFromFile(filename);
    return this;
}

SimpleSkyboxBuilder* SimpleSkyboxBuilder::bottom(std::string filename)
{
    m_bottom.loadFromFile(filename);
    return this;
}

SimpleSkyboxBuilder* SimpleSkyboxBuilder::left(std::string filename)
{
    m_left.loadFromFile(filename);
    return this;
}

SimpleSkyboxBuilder* SimpleSkyboxBuilder::right(std::string filename)
{
    m_right.loadFromFile(filename);
    return this;
}

SimpleSkyboxBuilder* SimpleSkyboxBuilder::front(std::string filename)
{
    m_front.loadFromFile(filename);
    return this;
}

SimpleSkyboxBuilder* SimpleSkyboxBuilder::back(std::string filename)
{
    m_back.loadFromFile(filename);
    return this;
}

globjects::Texture* SimpleSkyboxBuilder::getTexture()
{
    std::map<gl::GLenum, sf::Image> skyboxTextures {
        { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X), m_right },
        { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_X), m_left },
        { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_Y), m_top },
        { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y), m_bottom },
        { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_Z), m_back },
        { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z), m_front },
    };

    auto skyboxTexture = new globjects::Texture(static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP));

    skyboxTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
    skyboxTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

    skyboxTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_S), static_cast<GLint>(GL_CLAMP_TO_EDGE));
    skyboxTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_T), static_cast<GLint>(GL_CLAMP_TO_EDGE));
    skyboxTexture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_WRAP_R), static_cast<GLint>(GL_CLAMP_TO_EDGE));

    skyboxTexture->bind();

    for (auto& kv : skyboxTextures)
    {
        const auto target = kv.first;
        auto image = kv.second;

        if (target == gl::GL_TEXTURE_CUBE_MAP_POSITIVE_Y || target == gl::GL_TEXTURE_CUBE_MAP_NEGATIVE_Y)
        {
            image.flipVertically();
        }
        else
        {
            image.flipHorizontally();
        }

        ::glTexImage2D(
            static_cast<::GLenum>(target),
            0,
            static_cast<::GLenum>(GL_RGBA8),
            static_cast<::GLsizei>(image.getSize().x),
            static_cast<::GLsizei>(image.getSize().y),
            0,
            static_cast<::GLenum>(GL_RGBA),
            static_cast<::GLenum>(GL_UNSIGNED_BYTE),
            reinterpret_cast<const ::GLvoid*>(image.getPixelsPtr()));
    }

    skyboxTexture->unbind();

    return skyboxTexture;
}
