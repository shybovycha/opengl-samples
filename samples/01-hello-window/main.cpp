#include <SFML/Window.hpp>

int main() {
  sf::Window window(sf::VideoMode(800, 600), "Hello SFML Window!");

  while (window.isOpen()) {
    sf::Event event;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }
  }

  // auto program = new Program();

  // program->attach(
  //   Shader::fromString(GL_VERTEX_SHADER, vertexShaderSource),
  //   Shader::fromString(GL_FRAGMENT_SHADER, fragmentShaderSource)
  // );

  // program->setUniform("extent", glm::vec2(1.0f, 0.5f)));

  return 0;
}
