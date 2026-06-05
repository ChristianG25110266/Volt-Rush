#include <SFML/Graphics.hpp>
#include "Jugador.hpp" // <-- Importamos nuestra nueva clase

int main() {
    sf::RenderWindow ventana(sf::VideoMode({800, 600}), "Juego Navarro SFML - Etapa 2: El Jugador");
    ventana.setFramerateLimit(60);

    // Creamos a nuestro personaje
    Jugador programador;

    while (ventana.isOpen()) {
        while (const std::optional evento = ventana.pollEvent()) {
            if (evento->is<sf::Event::Closed>()) {
                ventana.close();
            }
        }

        // 1. ACTUALIZAR LÓGICA
        programador.actualizar();

        // 2. DIBUJAR EN PANTALLA
        ventana.clear(sf::Color::Black);
        
        programador.dibujar(ventana); // Pintamos al personaje
        
        ventana.display();
    }
    return 0;
}