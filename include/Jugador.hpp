#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class Jugador {
private:
    sf::Texture textura;
    sf::Sprite sprite;
    float velocidad;

    // --- Herramientas de Animación ---
    sf::Clock relojAnimacion; 
    int frameActual;
    
    // --- Variables de Ajuste Fino (Bisturí) ---
    int anchoFrame;
    int altoFrame;
    int yFilaCaminando;
    int xInicial;    // Margen izquierdo antes de empezar a recortar
    int separacion;  // Espacio en blanco entre un Pikachu y otro
    
    float escalaPikachu;

    // --- Variables para movimiento automático ---
    int direccion; 
    bool espacioPresionadoAnterior;

public:
    Jugador() : sprite(textura) {
        if (!textura.loadFromFile("assets/images/pikachu.png")) {
            std::cout << "Error al cargar la imagen.\n";
        }
        sprite.setTexture(textura, true);

        // 1. AJUSTES FINOS DEL RECORTADOR
        anchoFrame = 32;     // Lo hicimos un pelín más angosto
        altoFrame = 35;      
        yFilaCaminando = 82; 
        xInicial = 2;        // Nos saltamos los primeros 2 píxeles de la izquierda
        separacion = 0;      // Por si después necesitas sumarle píxeles entre frames
        
        frameActual = 0;
        
        // 2. TAMAÑO DEL PERSONAJE
        escalaPikachu = 2.0f; 

        // Aplicamos el recorte inicial tomando en cuenta el xInicial
        sprite.setTextureRect(sf::IntRect({xInicial, yFilaCaminando}, {anchoFrame, altoFrame}));
        sprite.setOrigin({anchoFrame / 2.0f, altoFrame / 2.0f});
        sprite.setPosition({400.f, 500.f}); 
        
        velocidad = 5.0f; 
        direccion = 1; 
        espacioPresionadoAnterior = false;
    }

    void actualizar() {
        // --- Cambio de Dirección (Barra Espaciadora) ---
        bool espacioPresionado = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
        if (espacioPresionado && !espacioPresionadoAnterior) {
            direccion *= -1; 
        }
        espacioPresionadoAnterior = espacioPresionado; 

        // --- Movimiento y Colisiones (Sin rebote) ---
        float posX = sprite.getPosition().x;
        float mitadAnchoEscalado = (anchoFrame * escalaPikachu) / 2.0f;
        
        bool seEstaMoviendo = false; 

        if (direccion == 1) { // Va a la DERECHA
            if (posX + mitadAnchoEscalado < 800) {
                sprite.move({velocidad, 0.f});
                seEstaMoviendo = true;
            }
            sprite.setScale({escalaPikachu, escalaPikachu}); 
            
        } else { // Va a la IZQUIERDA
            if (posX - mitadAnchoEscalado > 0) {
                sprite.move({-velocidad, 0.f});
                seEstaMoviendo = true;
            }
            sprite.setScale({-escalaPikachu, escalaPikachu}); 
        }

        // --- Lógica de la Animación ---
        if (seEstaMoviendo) {
            if (relojAnimacion.getElapsedTime().asSeconds() > 0.1f) {
                frameActual++; 
                if (frameActual > 3) { 
                    frameActual = 0;
                }
                
                // Calculamos la posición X exacta saltando el margen inicial y las separaciones
                int xRecorte = xInicial + (frameActual * (anchoFrame + separacion));
                sprite.setTextureRect(sf::IntRect({xRecorte, yFilaCaminando}, {anchoFrame, altoFrame}));
                
                relojAnimacion.restart(); 
            }
        } else {
            // Si chocó con la pared, pose inicial limpia
            frameActual = 0;
            sprite.setTextureRect(sf::IntRect({xInicial, yFilaCaminando}, {anchoFrame, altoFrame}));
        }
    }

    void dibujar(sf::RenderWindow& ventana) {
        ventana.draw(sprite);
    }
};