#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include <glm/vec2.hpp>

#include <glbinding/gl/gl.h>
#include <glbinding/Version.h>
#include <glbinding-aux/ContextInfo.h>
#include <glbinding-aux/types_to_string.h>

#include <globjects/globjects.h>
#include <globjects/base/File.h>
#include <globjects/logging.h>

#include <globjects/Error.h>
#include <globjects/Buffer.h>
#include <globjects/Program.h>
#include <globjects/ProgramPipeline.h>
#include <globjects/Shader.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/base/StaticStringSource.h>

#include <GLFW/glfw3.h>

std::string readFile(const std::string &fileName) {
  std::ifstream file(fileName);

  std::string content = "";
  std::string line;

  while (std::getline(file, line)) {
    content += line + "\n";
  }

  file.close();

  return content;
}

int main() {
  if (!glfwInit()) {
    std::cerr << "Could not initialize GLFW" << std::endl;
    return 1;
  }

  glfwSetErrorCallback([](int errnum, const char *msg) {
    std::cerr << "[ERROR] " << msg << std::endl;
  });

  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_VISIBLE, true);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

#ifdef SYSTEM_DARWIN
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

  GLFWwindow* window = glfwCreateWindow(640, 480, "Hello OpenGL", nullptr, nullptr);

  if (window == nullptr) {
    std::cerr << "Failed to create context" << std::endl;

    glfwTerminate();

    return 1;
  }

  glfwMakeContextCurrent(window);

  globjects::init([&](const char * name) {
    return glfwGetProcAddress(name);
  });

  globjects::DebugMessage::enable(); // enable automatic messages if KHR_debug is available

  globjects::DebugMessage::setCallback([](const globjects::DebugMessage & message) {
    std::cout << "[DEBUG] " << message.message() << std::endl;
  });

  std::cout << std::endl
        << "OpenGL Version:  " << glbinding::aux::ContextInfo::version() << std::endl
        << "OpenGL Vendor:   " << glbinding::aux::ContextInfo::vendor() << std::endl
        << "OpenGL Renderer: " << glbinding::aux::ContextInfo::renderer() << std::endl << std::endl;

  std::cout << "[INFO] Initializing..." << std::endl;

  std::cout << "[INFO] Loading vertex shader code...";

  const auto vertexShaderCode = readFile("media/vertex.glsl");

  std::cout << "done" << std::endl << vertexShaderCode << std::endl;

  std::cout << "[INFO] Loading fragment shader code...";

  const auto fragmentShaderCode = readFile("media/fragment.glsl");

  std::cout << "done" << std::endl << fragmentShaderCode << std::endl;

  auto g_cornerBuffer = globjects::Buffer::create();

  std::cout << "[INFO] Creating shaders..." << std::endl;

  std::cout << "[INFO] Creating vertex program...";

  auto g_vertexProgram = globjects::Program::create();

  auto error = globjects::Error::get();

  if (error) {
    std::cerr << "Error " << std::hex << error.code() << ": " << error.name() << std::endl;
    return 1;
  }

  std::cout << "done" << std::endl;

  auto g_fragmentProgram = globjects::Program::create();

  std::cout << "[INFO] Creating fragment program...";

  auto g_programPipeline = globjects::ProgramPipeline::create();

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Creating VAO...";

  auto g_vao = globjects::VertexArray::create();

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Compiling vertex shader...";

  auto g_vertexShaderSource = globjects::Shader::sourceFromString(vertexShaderCode);
  auto g_vertexShaderTemplate = globjects::Shader::applyGlobalReplacements(g_vertexShaderSource.get());
  auto g_vertexShader = globjects::Shader::create(static_cast<gl::GLenum>(GL_VERTEX_SHADER), g_vertexShaderTemplate.get());

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Compiling fragment shader...";

  auto g_fragmentShaderSource = globjects::Shader::sourceFromString(fragmentShaderCode);
  auto g_fragmentShaderTemplate = globjects::Shader::applyGlobalReplacements(g_fragmentShaderSource.get());
  auto g_fragmentShader = globjects::Shader::create(static_cast<gl::GLenum>(GL_FRAGMENT_SHADER), g_fragmentShaderTemplate.get());

  std::cout << "done" << std::endl;

  // std::cout << "done" << std::endl;

  // std::cout << "Creating pipeline...";

  // auto g_vertexShader = globjects::Shader::fromFile(GL_VERTEX_SHADER, "media/vertex.glsl");
  // auto g_fragmentShader = globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "media/fragment.glsl");

  auto g_size = glm::ivec2{ 600, 800 };

  std::cout << "[INFO] Linking shader program...";

  g_vertexProgram->attach(g_vertexShader.get());
  g_fragmentProgram->attach(g_fragmentShader.get());

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Setting pipeline stages...";

  g_programPipeline->useStages(g_vertexProgram.get(), gl::GL_VERTEX_SHADER_BIT);
  g_programPipeline->useStages(g_fragmentProgram.get(), gl::GL_FRAGMENT_SHADER_BIT);

  std::cout << "done" << std::endl;

  std::cout << "[INFO] Filling VAO buffers with data...";

  g_cornerBuffer->setData(
    std::array<glm::vec2, 4>{
      { glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, 1), glm::vec2(1, 1) }
    },
    static_cast<gl::GLenum>(GL_STATIC_DRAW)
  );

  g_vao->binding(0)->setAttribute(0);
  g_vao->binding(0)->setBuffer(g_cornerBuffer.get(), 0, sizeof(glm::vec2));
  g_vao->binding(0)->setFormat(2, static_cast<gl::GLenum>(GL_FLOAT));
  g_vao->enable(0);

  std::cout << "done" << std::endl;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, g_size.x, g_size.y);

    g_programPipeline->use();
    g_vao->drawArrays(static_cast<gl::GLenum>(GL_TRIANGLE_STRIP), 0, 4);

    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}
