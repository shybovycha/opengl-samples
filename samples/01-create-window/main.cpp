#include <SFML/Window.hpp>

int main() {
#ifdef SYSTEM_DARWIN
  auto videoMode = sf::VideoMode(2048, 1536);
#else
  auto videoMode = sf::VideoMode(1024, 768);
#endif

  sf::Window window(videoMode, "Hello SFML Window!");

  while (window.isOpen()) {
    sf::Event event {};

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }
  }

  return 0;
}
