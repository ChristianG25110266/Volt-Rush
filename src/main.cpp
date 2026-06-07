#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib> 
#include <ctime>   
#include <iostream>
#include <string> 
#include "Jugador.hpp"
#include "Entidad.hpp"

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::RenderWindow ventana(sf::VideoMode({800, 600}), "Juego Navarro - Etapa 6: Dificultad Dinamica");
    ventana.setFramerateLimit(60);

    Jugador programador;

    sf::Texture texPokebola, texVayaFrambu, texVaya;
    if (!texPokebola.loadFromFile("assets/images/pokebola.png") ||
        !texVayaFrambu.loadFromFile("assets/images/vayaframbu.png") ||
        !texVaya.loadFromFile("assets/images/vaya.png")) {
        std::cout << "Advertencia: Alguna imagen no cargo correctamente.\n";
    }

    sf::Font fuente;
    if (!fuente.openFromFile("assets/fonts/fuente.ttf")) {
        std::cout << "Error: No se encontro assets/fonts/fuente.ttf\n";
    }
    
    sf::Text textoMarcador(fuente);
    textoMarcador.setCharacterSize(24);
    textoMarcador.setFillColor(sf::Color::White);
    textoMarcador.setPosition({600.f, 15.f}); 

    std::vector<Entidad> listaEntidades;
    sf::Clock relojSpawneo;
    
    // --- NUEVO: RELOJ DE DIFICULTAD ---
    sf::Clock relojDificultad; 

    while (ventana.isOpen()) {
        while (const std::optional evento = ventana.pollEvent()) {
            if (evento->is<sf::Event::Closed>()) {
                ventana.close();
            }
        }

        if (programador.estaVivo()) {
            
            // --- NUEVO: CALCULAR LA DIFICULTAD ACTUAL ---
            float tiempoJugado = relojDificultad.getElapsedTime().asSeconds();
            
            // La velocidad aumenta 0.05 por cada segundo que sobrevives
            float incrementoVelocidad = tiempoJugado * 0.05f; 
            
            // El tiempo entre caídas se reduce gradualmente para que caigan más objetos
            float tiempoEntreSpawns = 1.2f - (tiempoJugado * 0.01f);
            if (tiempoEntreSpawns < 0.5f) { 
                tiempoEntreSpawns = 0.5f; // Límite máximo de locura (medio segundo)
            }

            // --- GENERADOR CON NUEVAS PROBABILIDADES ---
            if (relojSpawneo.getElapsedTime().asSeconds() > tiempoEntreSpawns) { 
                float posXAleatoria = std::rand() % 700 + 50; 
                int probabilidad = std::rand() % 100; 

                if (probabilidad < 90) { 
                    // 90% Pokébolas (se les suma el incrementoVelocidad para que caigan más rápido)
                    listaEntidades.push_back(Entidad(texPokebola, OBSTACULO, posXAleatoria, 3.0f + incrementoVelocidad));
                } 
                else if (probabilidad < 96) { 
                    // 6% Bayas Curativas
                    listaEntidades.push_back(Entidad(texVayaFrambu, CURA, posXAleatoria, 3.5f + incrementoVelocidad));
                } 
                else { 
                    // 4% Power-Ups
                    listaEntidades.push_back(Entidad(texVaya, POWERUP, posXAleatoria, 4.0f + incrementoVelocidad));
                }
                relojSpawneo.restart(); 
            }

            programador.actualizar();

            for (size_t i = 0; i < listaEntidades.size(); ) {
                listaEntidades[i].actualizar(); 
                
                if (programador.getLimites().findIntersection(listaEntidades[i].getLimites())) {
                    TipoEntidad tipoTocado = listaEntidades[i].getTipo();
                    
                    if (tipoTocado == OBSTACULO) {
                        if (programador.esInvencible()) {
                            programador.sumarPuntos(10); 
                        } else {
                            programador.perderVida(); 
                        }
                    }
                    else if (tipoTocado == CURA) {
                        programador.ganarVida();
                    }
                    else if (tipoTocado == POWERUP) {
                        programador.activarPowerUp(); 
                    }

                    listaEntidades.erase(listaEntidades.begin() + i);
                }
                else if (listaEntidades[i].getY() > 600.f) {
                    if (listaEntidades[i].getTipo() == OBSTACULO) {
                        programador.sumarPuntos(10); 
                    }
                    listaEntidades.erase(listaEntidades.begin() + i);
                } 
                else {
                    i++; 
                }
            }
        } 

        std::string textoPuntos = "Puntos: " + std::to_string(programador.getPuntuacion());
        if (programador.getMultiplicador() == 2) {
            textoPuntos += " (x2)";
        }
        
        if (!programador.estaVivo()) {
            textoPuntos = "GAME OVER\nPuntos Finales: " + std::to_string(programador.getPuntuacion());
            textoMarcador.setPosition({300.f, 250.f}); 
            textoMarcador.setCharacterSize(30);
            textoMarcador.setFillColor(sf::Color::Red);
        }

        textoMarcador.setString(textoPuntos);

        ventana.clear(sf::Color::Black);
        for (auto& entidad : listaEntidades) {
            entidad.dibujar(ventana);
        }
        programador.dibujar(ventana); 
        ventana.draw(textoMarcador); 
        
        ventana.display();
    }
    return 0;
}