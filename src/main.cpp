#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> 
#include <vector>
#include <cstdlib> 
#include <ctime>   
#include <iostream>
#include <string> 
#include "Jugador.hpp"
#include "Entidad.hpp"

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::RenderWindow ventana(sf::VideoMode({800, 600}), "Juego Navarro - Etapa 7: Audio y Ajustes Finales");
    ventana.setFramerateLimit(60);

    Jugador programador;

    // --- CARGAR FONDO ---
    sf::Texture texFondo;
    if (!texFondo.loadFromFile("assets/images/fondo.png")) {
        std::cout << "Advertencia: No se encontro assets/images/fondo.png\n";
    }
    sf::Sprite spriteFondo(texFondo);
    
    // AJUSTE: Subimos la imagen en el eje Y para que se vea el camino de tierra abajo
    spriteFondo.setPosition({0.f, -150.f}); 

    // --- CARGAR TEXTURAS DE ENTIDADES ---
    sf::Texture texPokebola, texVayaFrambu, texVaya;
    if (!texPokebola.loadFromFile("assets/images/pokebola.png") ||
        !texVayaFrambu.loadFromFile("assets/images/vayaframbu.png") ||
        !texVaya.loadFromFile("assets/images/vaya.png")) {
        std::cout << "Advertencia: Alguna imagen de entidad no cargo.\n";
    }

    // --- CARGAR FUENTE ---
    sf::Font fuente;
    if (!fuente.openFromFile("assets/fonts/fuente.ttf")) {
        std::cout << "Error: No se encontro assets/fonts/fuente.ttf\n";
    }
    
    sf::Text textoMarcador(fuente);
    textoMarcador.setCharacterSize(24);
    textoMarcador.setFillColor(sf::Color::White);
    textoMarcador.setPosition({600.f, 15.f}); 

    // --- CARGAR AUDIOS ---
    sf::SoundBuffer bufVaya, bufPowerup, bufGameover, bufInicial;
    if (!bufVaya.loadFromFile("assets/audio/musica_vaya.ogg") ||
        !bufPowerup.loadFromFile("assets/audio/musica_powerup.ogg") ||
        !bufGameover.loadFromFile("assets/audio/musica_gameover.ogg") ||
        !bufInicial.loadFromFile("assets/audio/sonido_inicial.ogg")) {
        std::cout << "Advertencia: Algun archivo de sonido (.ogg) no cargo.\n";
    }
    
    sf::Sound sndPokebola(bufVaya);          
    sf::Sound sndBayaRoja(bufPowerup);       
    sf::Sound sndGameover(bufGameover);      
    sf::Sound sndInicial(bufInicial);        

    sf::Music loopPowerUp;
    if (!loopPowerUp.openFromFile("assets/audio/sonido_powerup.ogg")) {
        std::cout << "Advertencia: No se encontro sonido_powerup.ogg\n";
    }
    loopPowerUp.setLooping(true); 

    std::vector<Entidad> listaEntidades;
    sf::Clock relojSpawneo;
    sf::Clock relojDificultad; 
    sf::Clock relojIntro; // Controla los 4 segundos de tiempo muerto inicial
    
    bool gameOverSonado = false; 
    bool juegoIniciado = false; 

    // --- REPRODUCIR SONIDO INICIAL ---
    sndInicial.play();

    while (ventana.isOpen()) {
        while (const std::optional evento = ventana.pollEvent()) {
            if (evento->is<sf::Event::Closed>()) {
                ventana.close();
            }
        }

        // Control del tiempo muerto inicial de 4 segundos
        if (!juegoIniciado) {
            if (relojIntro.getElapsedTime().asSeconds() >= 4.0f) {
                juegoIniciado = true;
                relojSpawneo.restart();   
                relojDificultad.restart();
            }
        }

        // El juego solo corre si Pikachu está vivo Y ya pasaron los 4 segundos de intro
        if (programador.estaVivo() && juegoIniciado) {
            
            float tiempoJugado = relojDificultad.getElapsedTime().asSeconds();
            float incrementoVelocidad = tiempoJugado * 0.1f; 
            float tiempoEntreSpawns = 1.2f - (tiempoJugado * 0.01f);
            if (tiempoEntreSpawns < 0.5f) tiempoEntreSpawns = 0.5f; 

            // GENERADOR CON NUEVAS PROBABILIDADES (Más adictivo)
            if (relojSpawneo.getElapsedTime().asSeconds() > tiempoEntreSpawns) { 
                float posXAleatoria = std::rand() % 700 + 50; 
                int probabilidad = std::rand() % 100; 

                if (probabilidad < 75) { // 75% Pokebolas
                    listaEntidades.push_back(Entidad(texPokebola, OBSTACULO, posXAleatoria, 3.0f + incrementoVelocidad));
                } 
                else if (probabilidad < 88) { // 13% Baya Roja
                    listaEntidades.push_back(Entidad(texVayaFrambu, CURA, posXAleatoria, 3.5f + incrementoVelocidad));
                } 
                else { // 12% Power-Up / Baya Dorada
                    listaEntidades.push_back(Entidad(texVaya, POWERUP, posXAleatoria, 4.0f + incrementoVelocidad));
                }
                relojSpawneo.restart(); 
            }

            programador.actualizar();

            // DETENER EL LOOP CUANDO SE ACABA EL ESCUDO
            if (!programador.esInvencible() && loopPowerUp.getStatus() == sf::SoundSource::Status::Playing) {
                loopPowerUp.stop(); 
            }

            // COLISIONES
            for (size_t i = 0; i < listaEntidades.size(); ) {
                listaEntidades[i].actualizar(); 
                
                if (programador.getLimites().findIntersection(listaEntidades[i].getLimites())) {
                    TipoEntidad tipoTocado = listaEntidades[i].getTipo();
                    
                    if (tipoTocado == OBSTACULO) {
                        if (programador.esInvencible()) {
                            programador.sumarPuntos(10); 
                        } else {
                            programador.perderVida(); 
                            sndPokebola.play(); 
                        }
                    }
                    else if (tipoTocado == CURA) {
                        programador.ganarVida();
                        sndBayaRoja.play(); 
                    }
                    else if (tipoTocado == POWERUP) {
                        programador.activarPowerUp(); 
                        
                        if (loopPowerUp.getStatus() != sf::SoundSource::Status::Playing) {
                            loopPowerUp.play(); 
                        }
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

        // ACTUALIZAR MARCADOR Y GAME OVER
        std::string textoPuntos = "Puntos: " + std::to_string(programador.getPuntuacion());
        if (programador.getMultiplicador() == 2) {
            textoPuntos += " (x2)";
        }
        
        // Mensaje en pantalla durante el tiempo muerto inicial
        if (!juegoIniciado && programador.estaVivo()) {
            textoPuntos = "PREPARATE...";
        }

        if (!programador.estaVivo()) {
            textoPuntos = "GAME OVER\nPuntos Finales: " + std::to_string(programador.getPuntuacion());
            textoMarcador.setPosition({300.f, 250.f}); 
            textoMarcador.setCharacterSize(30);
            textoMarcador.setFillColor(sf::Color::Red);
            
            if (!gameOverSonado) {
                loopPowerUp.stop(); 
                sndGameover.play(); 
                gameOverSonado = true;
            }
        }

        textoMarcador.setString(textoPuntos);

        // DIBUJAR TODO
        ventana.clear(sf::Color::Black);
        ventana.draw(spriteFondo); 
        
        // Solo dibujamos y actualizamos entidades si la partida ya inició
        if (juegoIniciado) {
            for (auto& entidad : listaEntidades) {
                entidad.dibujar(ventana);
            }
        }

        programador.dibujar(ventana); // Pikachu se dibuja desde el inicio en su camino de tierra
        ventana.draw(textoMarcador); 
        
        ventana.display();
    }
    return 0;
}
