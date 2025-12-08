#ifndef VEHICLE_SELECT_STATE_HPP
#define VEHICLE_SELECT_STATE_HPP

#include "State.hpp"
#include "UI.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class VehicleSelectState : public State
{
private:
    UI ui;
    int selectedVehicle;
    float scrollOffset; // Para scroll horizontal
    // Fondo del menú Select Vehicle
    sf::Texture bgTexture;
    sf::Sprite bgSprite;
    // Audio 'rev' al confirmar selección
    sf::Sound revSound;
    sf::SoundBuffer revBuffer;
public:
    VehicleSelectState(GameEngine* engine);
    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
};

#endif
