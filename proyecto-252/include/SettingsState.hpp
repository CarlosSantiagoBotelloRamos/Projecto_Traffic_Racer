#ifndef SETTINGS_STATE_HPP
#define SETTINGS_STATE_HPP

#include "State.hpp"
#include "UI.hpp"
#include <SFML/Graphics.hpp>

class SettingsState : public State {
private:
    UI ui;
    float volMusic;
    float volSfx;
    sf::FloatRect btnMusicMinus, btnMusicPlus;
    sf::FloatRect btnSfxMinus, btnSfxPlus;
    sf::FloatRect btnBack;

public:
    explicit SettingsState(GameEngine* engine);
    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    void setupLayout();
};

#endif
