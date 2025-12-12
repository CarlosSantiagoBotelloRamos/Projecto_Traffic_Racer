#include "SettingsState.hpp"
#include "GameEngine.hpp"
#include "Config.hpp"
#include <iostream>

SettingsState::SettingsState(GameEngine* engine)
    : State(engine)
{
    ui.loadFont("assets/fonts/arial.ttf");
    volMusic = engine->getMusicVolume();
    volSfx = engine->getSfxVolume();
    setupLayout();
}

void SettingsState::handleInput(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            engine->popState();
            return;
        }
    }
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f pos = engine->getWindow().mapPixelToCoords(
            sf::Vector2i(event.mouseButton.x, event.mouseButton.y), engine->getBaseView());
        if (btnBack.contains(pos)) { engine->popState(); return; }
        if (btnMusicMinus.contains(pos)) { volMusic -= 10.f; if (volMusic < 0.f) volMusic = 0.f; engine->setMusicVolume(volMusic); }
        if (btnMusicPlus.contains(pos))  { volMusic += 10.f; if (volMusic > 100.f) volMusic = 100.f; engine->setMusicVolume(volMusic); }
        if (btnSfxMinus.contains(pos))   { volSfx -= 10.f; if (volSfx < 0.f) volSfx = 0.f; engine->setSfxVolume(volSfx); }
        if (btnSfxPlus.contains(pos))    { volSfx += 10.f; if (volSfx > 100.f) volSfx = 100.f; engine->setSfxVolume(volSfx); }
    }
}

void SettingsState::update(float deltaTime)
{
    (void)deltaTime;
}

void SettingsState::render(sf::RenderWindow& window)
{
    window.clear(sf::Color(230, 230, 230));
    ui.drawText(window, "Settings", 20, 20, 28, sf::Color::Black);

    ui.drawText(window, "Music", 60, 80, 22, sf::Color::Black);
    ui.drawButton(window, "-", btnMusicMinus.left, btnMusicMinus.top, btnMusicMinus.width, btnMusicMinus.height, sf::Color(50,50,50), sf::Color::White);
    ui.drawButton(window, "+", btnMusicPlus.left,  btnMusicPlus.top,  btnMusicPlus.width,  btnMusicPlus.height,  sf::Color(50,50,50), sf::Color::White);
    ui.drawText(window, std::to_string(static_cast<int>(volMusic)) + "%", 180, 86, 20, sf::Color::Black);

    ui.drawText(window, "SFX", 60, 130, 22, sf::Color::Black);
    ui.drawButton(window, "-", btnSfxMinus.left, btnSfxMinus.top, btnSfxMinus.width, btnSfxMinus.height, sf::Color(50,50,50), sf::Color::White);
    ui.drawButton(window, "+", btnSfxPlus.left,  btnSfxPlus.top,  btnSfxPlus.width,  btnSfxPlus.height,  sf::Color(50,50,50), sf::Color::White);
    ui.drawText(window, std::to_string(static_cast<int>(volSfx)) + "%", 180, 136, 20, sf::Color::Black);

    ui.drawButton(window, "Back", btnBack.left, btnBack.top, btnBack.width, btnBack.height, sf::Color(67,176,42), sf::Color::Black);
}

void SettingsState::setupLayout()
{
    // Simple layout at top-left using base view coordinates
    btnMusicMinus = sf::FloatRect(120, 80, 30, 30);
    btnMusicPlus  = sf::FloatRect(150, 80, 30, 30);
    btnSfxMinus   = sf::FloatRect(120, 130, 30, 30);
    btnSfxPlus    = sf::FloatRect(150, 130, 30, 30);
    btnBack       = sf::FloatRect(60, 190, 120, 36);
}
