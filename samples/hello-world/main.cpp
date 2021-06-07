#include <SFML/Window.hpp>

int main() {
  sf::Window window(sf::VideoMode(800, 600), "Hello OpenGL!");

  while (window.isOpen()) {
    sf::Event event;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }
  }

  return 0;
}
