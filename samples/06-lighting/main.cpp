#include <iostream>
#include <sstream>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <glbinding/gl/gl.h>

#include <globjects/globjects.h>
#include <globjects/base/File.h>

#include <globjects/Error.h>
#include <globjects/Buffer.h>
#include <globjects/Program.h>
#include <globjects/ProgramPipeline.h>
#include <globjects/Shader.h>
#include <globjects/Texture.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/base/StaticStringSource.h>

#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

#ifdef WIN32
using namespace gl;
#endif

int main() {
  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antialiasingLevel = 4;
  settings.majorVersion = 3;
  settings.minorVersion = 2;
  settings.attributeFlags = sf::ContextSettings::Attribute::Core;

  sf::Window window(sf::VideoMode(800, 600), "Hello Lights!", sf::Style::Default, settings);

  globjects::init([](const char* name) {
    return sf::Context::getFunction(name);
  });

  globjects::DebugMessage::enable();// enable automatic messages if KHR_debug is available

  globjects::DebugMessage::setCallback([](const globjects::DebugMessage& message) {
    std::cout << "[DEBUG] " << message.message() << std::endl;
  });

  std::cout << "[INFO] Initializing..." << std::endl;

  std::cout << "[INFO] Creating shaders..." << std::endl;

  std::cout << "[INFO] Compiling vertex shader...";

  auto vertexProgram = globjects::Program::create();
  auto vertexShaderSource = globjects::Shader::sourceFromFile("media/vertex.glsl");
  auto vertexShaderTemplate = globjects::Shader::applyGlobalReplacements(vertexShaderSource.get());
  auto vertexShader = globjects::Shader::create(static_cast<gl::GLenum>(GL_VERTEX_SHADER), vertexShaderTemplate.get());

  if (!vertexShader->compile()) {
    std::cerr << "[ERROR] Can not compile vertex shader" << std::endl;
    return 1;
  }

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Compiling fragment shader...";

  auto fragmentProgram = globjects::Program::create();
  auto fragmentShaderSource = globjects::Shader::sourceFromFile("media/fragment.glsl");
  auto fragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(fragmentShaderSource.get());
  auto fragmentShader = globjects::Shader::create(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), fragmentShaderTemplate.get());

  if (!fragmentShader->compile()) {
    std::cerr << "[ERROR] Can not compile fragment shader" << std::endl;
    return 1;
  }

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Linking shader programs...";

  vertexProgram->attach(vertexShader.get());
  fragmentProgram->attach(fragmentShader.get());

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Creating rendering pipeline...";

  auto programPipeline = globjects::ProgramPipeline::create();

  programPipeline->useStages(vertexProgram.get(), gl::GL_VERTEX_SHADER_BIT);
  programPipeline->useStages(fragmentProgram.get(), gl::GL_FRAGMENT_SHADER_BIT);

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Creating buffer objects...";

  auto meshVertexBuffer = globjects::Buffer::create();

  meshVertexBuffer->setData(
      std::array<glm::vec3, 8> {
          {
              glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 1, 0), glm::vec3(1, 0, 0),
              glm::vec3(0, 0, 1), glm::vec3(0, 1, 1), glm::vec3(1, 1, 1), glm::vec3(1, 0, 1),
          }
      },
      static_cast<gl::GLenum>(GL_STATIC_DRAW)
  );

  auto meshVertexUVsBuffer = globjects::Buffer::create();

  meshVertexUVsBuffer->setData(
      std::array<glm::vec2, 8> {
          {
              glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0),
              glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0),
          }
      },
      static_cast<gl::GLenum>(GL_STATIC_DRAW)
  );

  auto meshVertexNormalsBuffer = globjects::Buffer::create();

  meshVertexNormalsBuffer->setData(
      std::array<glm::vec3, 8> {
          {
              glm::vec3(-0.5, -0.5, -0.5), glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0.5, 0.5, -0.5), glm::vec3(0.5, -0.5, -0.5),
              glm::vec3(-0.5, -0.5, 0.5), glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.5, -0.5, 0.5),
          }
      },
      static_cast<gl::GLenum>(GL_STATIC_DRAW)
  );

  auto meshIndexBuffer = globjects::Buffer::create();

  const auto numFaces = 12;

  meshIndexBuffer->setData(
      std::array<std::array<GLuint, 3>, numFaces> {
          {
              { 0, 1, 2 }, { 2, 3, 0 }, // back
              { 4, 5, 6 }, { 6, 7, 4 }, // front
              { 0, 1, 5 }, { 5, 4, 0 }, // left
              { 3, 2, 6 }, { 6, 7, 3 }, // right
              { 1, 5, 6 }, { 6, 2, 1 }, // top
              { 0, 3, 7 }, { 7, 4, 0 }, // bottom
          }
      },
      static_cast<gl::GLenum>(GL_STATIC_DRAW)
  );

  auto vao = globjects::VertexArray::create();

  vao->bindElementBuffer(meshIndexBuffer.get());

  vao->binding(0)->setAttribute(0);
  vao->binding(0)->setBuffer(meshVertexBuffer.get(), 0, sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
  vao->binding(0)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
  vao->enable(0);

  vao->binding(1)->setAttribute(1);
  vao->binding(1)->setBuffer(meshVertexUVsBuffer.get(), 0, sizeof(glm::vec2)); // number of elements in buffer, stride, size of buffer element
  vao->binding(1)->setFormat(2, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
  vao->enable(1);

  vao->binding(2)->setAttribute(2);
  vao->binding(2)->setBuffer(meshVertexNormalsBuffer.get(), 0, sizeof(glm::vec3)); // number of elements in buffer, stride, size of buffer element
  vao->binding(2)->setFormat(3, static_cast<gl::GLenum>(GL_FLOAT)); // number of data elements per buffer element (vertex), type of data
  vao->enable(2);

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Loading texture...";

  sf::Image textureImage;

  if (!textureImage.loadFromFile("media/texture.jpg")) {
    std::cerr << "[ERROR] Can not load texture" << std::endl;
    return 1;
  }

  textureImage.flipVertically();

  auto texture = globjects::Texture::create(static_cast<gl::GLenum>(GL_TEXTURE_2D));

  texture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MIN_FILTER), static_cast<GLint>(GL_LINEAR));
  texture->setParameter(static_cast<gl::GLenum>(GL_TEXTURE_MAG_FILTER), static_cast<GLint>(GL_LINEAR));

  texture->image2D(
      0,
      static_cast<gl::GLenum>(GL_RGBA8),
      glm::vec2(textureImage.getSize().x, textureImage.getSize().y),
      0,
      static_cast<gl::GLenum>(GL_RGBA),
      static_cast<gl::GLenum>(GL_UNSIGNED_BYTE),
      reinterpret_cast<const gl::GLvoid*>(textureImage.getPixelsPtr())
  );

  fragmentProgram->setUniform("textureSampler", 0);

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Done initializing" << std::endl;

  const float fov = 45.0f;

  const float cameraMoveSpeed = 1.0f;
  const float cameraRotateSpeed = 10.0f;

  glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 cameraForward = glm::normalize(glm::cross(cameraUp, cameraRight));

  sf::Clock clock;

  glEnable(static_cast<gl::GLenum>(GL_DEPTH_TEST));

  while (window.isOpen()) {
#ifdef WIN32
    if (!window.hasFocus()) {
      continue;
    }
#endif

    sf::Event event {};

    // measure time since last frame, in seconds
    float deltaTime = static_cast<float>(clock.restart().asSeconds());

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
        break;
      }
    }

    glm::vec2 currentMousePos = glm::vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
    glm::vec2 mouseDelta = currentMousePos - glm::vec2((window.getSize().x / 2), (window.getSize().y / 2));

    float horizontalAngle = (mouseDelta.x / static_cast<float>(window.getSize().x)) * -1 * deltaTime * cameraRotateSpeed * fov;
    float verticalAngle = (mouseDelta.y / static_cast<float>(window.getSize().y)) * -1 * deltaTime * cameraRotateSpeed * fov;

    cameraForward = glm::rotate(cameraForward, horizontalAngle, cameraUp);
    cameraForward = glm::rotate(cameraForward, verticalAngle, cameraRight);

    cameraRight = glm::normalize(glm::rotate(cameraRight, horizontalAngle, cameraUp));

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
      cameraPos += cameraForward * cameraMoveSpeed * deltaTime;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
      cameraPos -= cameraForward * cameraMoveSpeed * deltaTime;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
      cameraPos -= glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
      cameraPos += glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraMoveSpeed * deltaTime;
    }

    glm::mat4 projection = glm::perspective(glm::radians(fov), (float) window.getSize().x / (float) window.getSize().y, 0.1f, 100.0f);

    glm::mat4 view = glm::lookAt(
        cameraPos,
        cameraPos + cameraForward,
        cameraUp
    );

    glm::mat4 model = glm::mat4(1.0f); // identity

    vertexProgram->setUniform("model", model);
    vertexProgram->setUniform("view", view);
    vertexProgram->setUniform("projection", projection);

    fragmentProgram->setUniform("lightPosition", glm::vec3(-2, 2, 2));
    fragmentProgram->setUniform("lightColor", glm::vec3(1, 0.5, 0.5));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));

    vao->bind();

    glActiveTexture(GL_TEXTURE0);

    texture->bind();

    programPipeline->use();

    // number of values passed = number of elements * number of vertices per element
    // in this case: 2 triangles, 3 vertex indexes per triangle
    vao->drawElements(static_cast<gl::GLenum>(GL_TRIANGLES), numFaces * 3, static_cast<gl::GLenum>(GL_UNSIGNED_INT), nullptr);

    programPipeline->release();

    vao->unbind();

    texture->unbind();

    window.display();

    sf::Mouse::setPosition(sf::Vector2<int>(window.getSize().x / 2, window.getSize().y / 2), window);
  }

  return 0;
}
