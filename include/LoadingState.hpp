#ifndef LOADING_STATE_HPP
#define LOADING_STATE_HPP

#include "State.hpp"
#include "UI.hpp"
#include <SFML/Graphics.hpp>

class LoadingState : public State {
private:
    UI ui;
    sf::Texture loadingTexture;
    sf::Sprite loadingSprite;
    float elapsed = 0.f;
    float duration = 5.f; // show for 5 seconds
    // Animación deshabilitada: texto estático

public:
    explicit LoadingState(GameEngine* engine);
    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
};

#endif
