#pragma once
#include <SFML/Graphics.hpp>

enum TipoEntidad { 
    OBSTACULO, 
    CURA,      
    POWERUP    
};

class Entidad {
private:
    sf::Sprite sprite;
    TipoEntidad tipo;
    float velocidadCaida;

public:
    Entidad(const sf::Texture& textura, TipoEntidad tipoEntidad, float posX, float vel) : sprite(textura) {
        sprite.setTexture(textura, true); 
        tipo = tipoEntidad;
        velocidadCaida = vel;
        sprite.setPosition({posX, -50.f}); 
        
        // --- AJUSTE DE TAMAÑOS ---
        if (tipo == OBSTACULO) {
            sprite.setScale({0.07f, 0.07f}); // Aumentado un poco - Power up
        } else if (tipo == CURA) {
            sprite.setScale({0.20f, 0.20f}); // Más visible - vaya frambu
        } else {
            sprite.setScale({0.25f, 0.25f}); // Más visible - vaya dorada
        }
    }

    void actualizar() {
        sprite.move({0.f, velocidadCaida}); 
    }

    void dibujar(sf::RenderWindow& ventana) {
        ventana.draw(sprite);
    }

    // Funciones para detectar las colisiones
    TipoEntidad getTipo() { return tipo; }
    sf::FloatRect getLimites() { return sprite.getGlobalBounds(); }
    float getY() { return sprite.getPosition().y; }
};