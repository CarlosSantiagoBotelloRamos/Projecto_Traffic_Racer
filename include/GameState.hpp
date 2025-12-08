#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "State.hpp"
#include "Player.hpp"
#include "Spawner.hpp"
#include "Score.hpp"
#include "UI.hpp"
#include <SFML/Audio.hpp>
#include <memory>


class GameState : public State
{
private:
    std::unique_ptr<Player> player;
    std::unique_ptr<Spawner> spawner;
    std::unique_ptr<Score> score;
    UI ui;
    float nearMissThreshold;

    // Fondo animado
    sf::Texture roadTexture;
    sf::Sprite roadSprite1;
    sf::Sprite roadSprite2;
    float roadScrollY;
    float roadScrollSpeed;
        // Audio de aceleración
        sf::Sound accelSound;
        sf::SoundBuffer accelBuffer;
        bool accelPrevPressed = false;
        // Audio andando (ralentí) cuando no se acelera
        sf::Sound idleSound;
        sf::SoundBuffer idleBuffer;
        // Audio de downshift (una sola vez al frenar por encima de 50 km/h)
        sf::Sound downshiftSound;
        sf::SoundBuffer downshiftBuffer;
        bool prevBrakePressed = false;

        // Audio de freno (una sola vez al iniciar frenado)
        sf::Sound frenoSound;
        sf::SoundBuffer frenoBuffer;

        // Audio de claxon (horn) al presionar tecla 1
        sf::Sound hornSound;
        sf::SoundBuffer hornBuffer;


public:
    GameState(GameEngine* engine, int selectedVehicle, int selectedMap);
    ~GameState();

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    void checkCollisions();
    void checkNearMisses();
    //void drawRoad(sf::RenderWindow& window); // Ya no se usa
};

#endif
