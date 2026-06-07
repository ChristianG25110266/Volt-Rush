#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class Jugador {
private:
    sf::Texture textura;
    sf::Sprite sprite;
    float velocidad;

    sf::Clock relojAnimacion; 
    int frameActual;
    int anchoFrame;
    int altoFrame;
    int yFilaCaminando;
    int xInicial;
    int separacion;
    float escalaPikachu;

    int direccion; 
    bool espacioPresionadoAnterior;

    // --- SISTEMA DE VIDAS ---
    int vidas;
    int vidasMaximas;
    sf::Texture texturaCorazon;
    sf::Sprite spriteCorazon;

    // --- SISTEMA DE PUNTOS ---
    int puntuacion;
    int multiplicador;

    // --- NUEVO: INVENCIBILIDAD ---
    bool invencible;
    sf::Clock relojInvencibilidad;

public:
    Jugador() : sprite(textura), spriteCorazon(texturaCorazon) { 
        if (!textura.loadFromFile("assets/images/pikachu.png")) {
            std::cout << "Error al cargar la imagen de Pikachu.\n";
        }
        sprite.setTexture(textura, true);

        anchoFrame = 32;     
        altoFrame = 35;      
        yFilaCaminando = 82; 
        xInicial = 2;        
        separacion = 0;      
        frameActual = 0;
        escalaPikachu = 2.0f; 

        sprite.setTextureRect(sf::IntRect({xInicial, yFilaCaminando}, {anchoFrame, altoFrame}));
        sprite.setOrigin({anchoFrame / 2.0f, altoFrame / 2.0f});
        sprite.setPosition({400.f, 500.f}); 
        
        velocidad = 5.0f; 
        direccion = 1; 
        espacioPresionadoAnterior = false;

        vidas = 3;         
        vidasMaximas = 3;  
        
        if (!texturaCorazon.loadFromFile("assets/images/corazon.png")) {
            std::cout << "Error al cargar la imagen del corazon.\n";
        }
        spriteCorazon.setTexture(texturaCorazon, true);
        spriteCorazon.setScale({0.08f, 0.08f}); 

        puntuacion = 0;
        multiplicador = 1; 
        
        invencible = false; 
    }

    void actualizar() {
        bool espacioPresionado = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
        if (espacioPresionado && !espacioPresionadoAnterior) {
            direccion *= -1; 
        }
        espacioPresionadoAnterior = espacioPresionado; 

        float posX = sprite.getPosition().x;
        float mitadAnchoEscalado = (anchoFrame * escalaPikachu) / 2.0f;
        bool seEstaMoviendo = false; 

        if (direccion == 1) { 
            if (posX + mitadAnchoEscalado < 800) {
                sprite.move({velocidad, 0.f});
                seEstaMoviendo = true;
            }
            sprite.setScale({escalaPikachu, escalaPikachu}); 
        } else { 
            if (posX - mitadAnchoEscalado > 0) {
                sprite.move({-velocidad, 0.f});
                seEstaMoviendo = true;
            }
            sprite.setScale({-escalaPikachu, escalaPikachu}); 
        }

        if (seEstaMoviendo) {
            if (relojAnimacion.getElapsedTime().asSeconds() > 0.1f) {
                frameActual++; 
                if (frameActual > 3) { frameActual = 0; }
                int xRecorte = xInicial + (frameActual * (anchoFrame + separacion));
                sprite.setTextureRect(sf::IntRect({xRecorte, yFilaCaminando}, {anchoFrame, altoFrame}));
                relojAnimacion.restart(); 
            }
        } else {
            frameActual = 0;
            sprite.setTextureRect(sf::IntRect({xInicial, yFilaCaminando}, {anchoFrame, altoFrame}));
        }

        // --- CONTROL DEL TIEMPO DE INVENCIBILIDAD (COLOR AZUL) ---
        if (invencible) {
            if (relojInvencibilidad.getElapsedTime().asSeconds() > 8.0f) {
                invencible = false;
                sprite.setColor(sf::Color::White); // Regresa a su color normal
            } else {
                sprite.setColor(sf::Color::Cyan); // Efecto visual: Color Azul brillante
            }
        }
    }

    void perderVida() {
        if (vidas > 0) {
            vidas--; 
        }
        multiplicador = 1; 
    }

    void ganarVida() {
        if (vidas < vidasMaximas) {
            vidas++; 
        }
    }

    bool estaVivo() { return vidas > 0; }

    void sumarPuntos(int puntosBase) {
        puntuacion += (puntosBase * multiplicador);
    }
    
    // Función que se llama cuando agarras el Power-Up
    void activarPowerUp() { 
        multiplicador = 2;       
        invencible = true;       
        relojInvencibilidad.restart(); // Inicia la cuenta de 8 segundos
    }
    
    int getPuntuacion() { return puntuacion; }
    int getMultiplicador() { return multiplicador; }
    bool esInvencible() { return invencible; } 

    void dibujar(sf::RenderWindow& ventana) {
        ventana.draw(sprite); 

        for (int i = 0; i < vidas; i++) {
            spriteCorazon.setPosition({15.f + (i * 40.f), 15.f});
            ventana.draw(spriteCorazon);
        }
    }

    sf::FloatRect getLimites() {
        return sprite.getGlobalBounds();
    }
};