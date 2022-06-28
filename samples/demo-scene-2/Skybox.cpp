#include "Skybox.hpp"

AbstractSkyboxBuilder* AbstractSkyboxBuilder::size(float size)
{
    m_size = size;
    return this;
}

std::unique_ptr<Skybox> AbstractSkyboxBuilder::build()
{
    std::vector<glm::vec3> vertices{
        { -1.0f, 1.0f, 1.0f },
        { -1.0f, -1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f },
        { -1.0f, -1.0f, 1.0f },
        { -1.0f, -1.0f, -1.0f },
        { -1.0f, 1.0f, -1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f },
        { 1.0f, -1.0f, -1.0f },
        { 1.0f, 1.0f, -1.0f },
        { -1.0f, 1.0f, -1.0f },
        { -1.0f, -1.0f, -1.0f },
        { 1.0f, -1.0f, -1.0f },
        { 1.0f, 1.0f, -1.0f },
        { -1.0f, 1.0f, -1.0f },
        { -1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, -1.0f },
        { -1.0f, -1.0f, -1.0f },
        { -1.0f, -1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f },
        { 1.0f, -1.0f, -1.0f },
    };

    std::vector<glm::vec3> normals{
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },

        { 0.0f, -1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },

        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },

        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },

        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },

        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },

        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },

        { 0.0f, -1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },

        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },

        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },

        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },

        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
    };

    std::for_each(vertices.begin(), vertices.end(), [this](glm::vec3 p) { return p * m_size; });

    std::vector<unsigned int> indices{
        2, 1, 0,
        2, 0, 3,
        4, 5, 6,
        7, 4, 6,
        10, 9, 8,
        10, 8, 11,
        12, 13, 14,
        15, 12, 14,
        18, 17, 16,
        18, 16, 19,
        20, 21, 22,
        23, 20, 22,
    };

    std::vector<glm::vec2> uvs{
        { 0.333333f, 0.500000f },
        { 0.333333f, 0.000000f },
        { 0.000000f, 0.000000f },
        { 0.000000f, 0.500000f },
        { 0.000000f, 1.000000f },
        { 0.000000f, 0.500000f },
        { 0.333333f, 0.500000f },
        { 0.333333f, 1.000000f },
        { 1.000000f, 1.000000f },
        { 1.000000f, 0.500000f },
        { 0.666666f, 0.500000f },
        { 0.666666f, 1.000000f },
        { 0.333333f, 1.000000f },
        { 0.333333f, 0.500000f },
        { 0.666666f, 0.500000f },
        { 0.666666f, 1.000000f },
        { 0.340000f, 0.500000f },
        { 0.666666f, 0.500000f },
        { 0.666666f, 0.000000f },
        { 0.340000f, 0.000000f },
        { 0.666666f, 0.500000f },
        { 0.666666f, 0.000000f },
        { 1.000000f, 0.000000f },
        { 1.000000f, 0.500000f },
    };

    auto mesh = AbstractMesh::builder()
        ->addVertices(vertices)
        ->addIndices(indices)
        ->addNormals(normals)
        ->addUVs(uvs)
        ->addTexture(getTexture())
        ->build();

    return std::make_unique<Skybox>(std::move(mesh));
}

CubemapSkyboxBuilder::CubemapSkyboxBuilder(std::unique_ptr<globjects::Texture> texture) : m_texture(texture.get()), AbstractSkyboxBuilder() {}

CubemapSkyboxBuilder::CubemapSkyboxBuilder(globjects::Texture* texture) : m_texture(texture), AbstractSkyboxBuilder() {}

globjects::Texture* CubemapSkyboxBuilder::getTexture()
{
    return m_texture;
}

SimpleSkyboxBuilder::SimpleSkyboxBuilder() : AbstractSkyboxBuilder() {}

SimpleSkyboxBuilder* SimpleSkyboxBuilder::top(std::string filename)
{
    m_top = std::make_unique<sf::Image>();
    m_top->loadFromFile(filename);
    return this;
}

SimpleSkyboxBuilder* SimpleSkyboxBuilder::bottom(std::string filename)
{
    m_bottom = std::make_unique<sf::Image>();
    m_bottom->loadFromFile(filename);
    return this;
}

SimpleSkyboxBuilder* SimpleSkyboxBuilder::left(std::string filename)
{
    m_left = std::make_unique<sf::Image>();
    m_left->loadFromFile(filename);
    return this;
}

SimpleSkyboxBuilder* SimpleSkyboxBuilder::right(std::string filename)
{
    m_right = std::make_unique<sf::Image>();
    m_right->loadFromFile(filename);
    return this;
}

SimpleSkyboxBuilder* SimpleSkyboxBuilder::front(std::string filename)
{
    m_front = std::make_unique<sf::Image>();
    m_front->loadFromFile(filename);
    return this;
}

SimpleSkyboxBuilder* SimpleSkyboxBuilder::back(std::string filename)
{
    m_back = std::make_unique<sf::Image>();
    m_back->loadFromFile(filename);
    return this;
}

globjects::Texture* SimpleSkyboxBuilder::getTexture()
{
    std::map<gl::GLenum, sf::Image*> skyboxTextures{
        { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X), m_right.get() },
        { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_X), m_left.get() },
        { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_Y), m_top.get() },
        { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y), m_bottom.get() },
        { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_Z), m_back.get() },
        { static_cast<gl::GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z), m_front.get() },
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
            image->flipVertically();
        }
        else
        {
            image->flipHorizontally();
        }

        ::glTexImage2D(
            static_cast<::GLenum>(target),
            0,
            static_cast<::GLenum>(GL_RGBA8),
            static_cast<::GLsizei>(image->getSize().x),
            static_cast<::GLsizei>(image->getSize().y),
            0,
            static_cast<::GLenum>(GL_RGBA),
            static_cast<::GLenum>(GL_UNSIGNED_BYTE),
            reinterpret_cast<const ::GLvoid*>(image->getPixelsPtr()));
    }

    skyboxTexture->unbind();

    return skyboxTexture;
}

SimpleSkyboxBuilder* Skybox::builder()
{
    return new SimpleSkyboxBuilder();
}

CubemapSkyboxBuilder* Skybox::fromCubemap(globjects::Texture* cubemapTexture)
{
    return new CubemapSkyboxBuilder(cubemapTexture);
}

Skybox::Skybox(std::unique_ptr<AbstractMesh> mesh) : SingleMeshModel(std::move(mesh))
{
}
