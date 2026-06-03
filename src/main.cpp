#include <SFML/Graphics.hpp>

int main() {
    // SFML 3.0 usa sf::Vector2u para el tamaño de la ventana
    sf::RenderWindow ventana(sf::VideoMode({800, 600}), "Juego Navarro SFML - Etapa 1: Modo Programador");
    ventana.setFramerateLimit(60);

    while (ventana.isOpen()) {
        // En SFML 3.0, pollEvent() devuelve un std::optional
        while (const std::optional evento = ventana.pollEvent()) {
            // Se revisa si el evento es de tipo Cerrar
            if (evento->is<sf::Event::Closed>()) {
                ventana.close();
            }
        }

        ventana.clear(sf::Color::Black);
        ventana.display();
    }
    return 0;
}