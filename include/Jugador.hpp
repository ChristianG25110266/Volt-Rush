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
    float escalaPikachu;

    sf::IntRect framesCaminando[4]; 

    int direccion; 
    bool espacioPresionadoAnterior;

    int vidas;
    int vidasMaximas;
    sf::Texture texturaCorazon;
    sf::Sprite spriteCorazon;

    int puntuacion;
    int multiplicador;
    bool invencible;
    sf::Clock relojInvencibilidad;

public:
    Jugador() : sprite(textura), spriteCorazon(texturaCorazon) { 
        if (!textura.loadFromFile("assets/images/pikachu.png")) {
            std::cout << "Error al cargar la imagen de Pikachu.\n";
        }
        sprite.setTexture(textura, true);

        // --- DEFINICIÓN EXACTA DE LOS CUADROS ---
        framesCaminando[0] = sf::IntRect({26, 78}, {37, 33}); 
        framesCaminando[1] = sf::IntRect({65, 78}, {39, 35}); 
        
        // Cuadros vacíos temporales (no se usarán hasta que los midas)
        framesCaminando[2] = sf::IntRect({0, 0}, {0, 0});     
        framesCaminando[3] = sf::IntRect({0, 0}, {0, 0});     

        frameActual = 0;
        escalaPikachu = 2.0f; 

        sprite.setTextureRect(framesCaminando[0]);
        
        // Origen dinámico con sintaxis de SFML 3 (.size.x / .size.y)
        sprite.setOrigin(sf::Vector2f(framesCaminando[0].size.x / 2.0f, framesCaminando[0].size.y / 2.0f));
        sprite.setPosition(sf::Vector2f(400.f, 500.f)); 
        
        // --- CORRECCIÓN: Aplicar la escala desde el inicio ---
        sprite.setScale(sf::Vector2f(escalaPikachu, escalaPikachu));

        velocidad = 5.0f; 
        direccion = 1; 
        espacioPresionadoAnterior = false;

        vidas = 3;         
        vidasMaximas = 3;  
        
        if (!texturaCorazon.loadFromFile("assets/images/corazon.png")) {
            std::cout << "Error al cargar la imagen del corazon.\n";
        }
        spriteCorazon.setTexture(texturaCorazon, true);
        spriteCorazon.setScale(sf::Vector2f(0.08f, 0.08f)); 

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
        
        float mitadAnchoEscalado = (framesCaminando[frameActual].size.x * escalaPikachu) / 2.0f;
        bool seEstaMoviendo = false; 

        if (direccion == 1) { 
            if (posX + mitadAnchoEscalado < 800) {
                sprite.move(sf::Vector2f(velocidad, 0.f));
                seEstaMoviendo = true;
            }
            sprite.setScale(sf::Vector2f(escalaPikachu, escalaPikachu)); 
        } else { 
            if (posX - mitadAnchoEscalado > 0) {
                sprite.move(sf::Vector2f(-velocidad, 0.f));
                seEstaMoviendo = true;
            }
            sprite.setScale(sf::Vector2f(-escalaPikachu, escalaPikachu)); 
        }

        if (seEstaMoviendo) {
            if (relojAnimacion.getElapsedTime().asSeconds() > 0.15f) {
                frameActual++; 
                
                // --- CORRECCIÓN: Solo iterar entre los 2 cuadros medidos (0 y 1) para evitar parpadeo ---
                if (frameActual > 1) { frameActual = 0; } 
                
                sprite.setTextureRect(framesCaminando[frameActual]);
                
                // Actualizamos el centro dinámicamente para evitar temblores
                sprite.setOrigin(sf::Vector2f(framesCaminando[frameActual].size.x / 2.0f, framesCaminando[frameActual].size.y / 2.0f));
                
                relojAnimacion.restart(); 
            }
        } else {
            if (frameActual != 0) {
                frameActual = 0;
                sprite.setTextureRect(framesCaminando[0]);
                sprite.setOrigin(sf::Vector2f(framesCaminando[0].size.x / 2.0f, framesCaminando[0].size.y / 2.0f));
            }
        }

        if (invencible) {
            if (relojInvencibilidad.getElapsedTime().asSeconds() > 4.0f) {
                invencible = false;
                sprite.setColor(sf::Color::White); 
            } else {
                sprite.setColor(sf::Color::Cyan); 
            }
        }
    }

    void perderVida() {
        if (vidas > 0) vidas--; 
        multiplicador = 1; 
    }

    void ganarVida() {
        if (vidas < vidasMaximas) vidas++; 
    }

    bool estaVivo() { return vidas > 0; }

    void sumarPuntos(int puntosBase) {
        puntuacion += (puntosBase * multiplicador);
    }
    
    void activarPowerUp() { 
        multiplicador = 2;       
        invencible = true;       
        relojInvencibilidad.restart(); 
    }
    
    int getPuntuacion() { return puntuacion; }
    int getMultiplicador() { return multiplicador; }
    bool esInvencible() { return invencible; } 

    void dibujar(sf::RenderWindow& ventana) {
        ventana.draw(sprite); 
        for (int i = 0; i < vidas; i++) {
            spriteCorazon.setPosition(sf::Vector2f(15.f + (i * 40.f), 15.f));
            ventana.draw(spriteCorazon);
        }
    }

    sf::FloatRect getLimites() {
        return sprite.getGlobalBounds();
    }
};