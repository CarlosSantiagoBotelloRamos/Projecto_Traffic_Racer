#include "GameOverState.hpp"
#include "GameEngine.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include <filesystem>
#include <cctype>
#include "Config.hpp"
#include <iostream>

GameOverState::GameOverState(GameEngine* engine, int score, float distance, int nearMisses)
        : State(engine),
            finalScore(score),
            finalDistance(distance),
            finalNearMisses(nearMisses),
            selectedOption(RESTART)
{
    ui.loadFont("assets/fonts/arial.ttf");
    // Reproducir audio de choque una sola vez
        // (Eliminado) audio de choque
}

void GameOverState::handleInput(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A)
        {
            selectedOption = std::max(0, selectedOption - 1);
        }
        else if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D)
        {
            selectedOption = std::min(2, selectedOption + 1);
        }
        else if (event.key.code == sf::Keyboard::Return)
        {
            if (selectedOption == RESTART)
            {
                // Reiniciar juego con misma configuración de vehículo y mapa
                int vehicle = engine->getSelectedVehicle();
                int map = engine->getSelectedMap();
                // Hacer exactamente lo mismo que Start Game del menú: push sin pop
                auto gameState = std::make_shared<GameState>(engine, vehicle, map);
                engine->pushState(gameState);
            }
            else if (selectedOption == RETURN_MENU)
            {
                // Volver al menú: quitar GameOver del tope y empujar Menu
                engine->popState();
                auto menuState = std::make_shared<MenuState>(engine);
                engine->pushState(menuState);
            }
            else if (selectedOption == EXIT)
            {
                // Salir del juego
                engine->quit();
            }
        }
    }
}

void GameOverState::update(float deltaTime)
{
    (void)deltaTime; // Evitar warning de parámetro no usado
    // Update logic if needed
}

void GameOverState::render(sf::RenderWindow& window)
{
    window.clear(sf::Color::Black);
    
    {
        sf::Text title("GAME OVER", ui.getFont(), 48);
        title.setFillColor(sf::Color::Red);
        auto winSize = window.getSize();
        auto tb = title.getLocalBounds();
        float tx = (static_cast<float>(winSize.x) - tb.width) * 0.5f - tb.left;
        float ty = 50.0f;
        title.setPosition(tx, ty);
        window.draw(title);
    }
    
    {
        sf::Text t1("Final Score: " + std::to_string(finalScore), ui.getFont(), 28);
        sf::Text t2("Distance: " + std::to_string(static_cast<int>(finalDistance)) + "m", ui.getFont(), 28);
        sf::Text t3("Near Misses: " + std::to_string(finalNearMisses), ui.getFont(), 28);
        auto win = window.getSize();
        auto b1 = t1.getLocalBounds();
        auto b2 = t2.getLocalBounds();
        auto b3 = t3.getLocalBounds();
        t1.setPosition((static_cast<float>(win.x) - b1.width) * 0.5f - b1.left, 150.f);
        t2.setPosition((static_cast<float>(win.x) - b2.width) * 0.5f - b2.left, 200.f);
        t3.setPosition((static_cast<float>(win.x) - b3.width) * 0.5f - b3.left, 250.f);
        window.draw(t1);
        window.draw(t2);
        window.draw(t3);
    }
    
    // Opciones: Restart, Return to Menu, Exit
    const float startY = 330.f;
    const float gapY = 40.f;
    sf::Text optRestart("Restart Game", ui.getFont(), 28);
    optRestart.setFillColor(selectedOption == RESTART ? sf::Color::Yellow : sf::Color::White);
    auto rB = optRestart.getLocalBounds();
    optRestart.setPosition((static_cast<float>(window.getSize().x) - rB.width) * 0.5f - rB.left, startY);
    window.draw(optRestart);

    sf::Text optMenu("Return to Menu", ui.getFont(), 28);
    optMenu.setFillColor(selectedOption == RETURN_MENU ? sf::Color::Yellow : sf::Color::White);
    auto mB = optMenu.getLocalBounds();
    optMenu.setPosition((static_cast<float>(window.getSize().x) - mB.width) * 0.5f - mB.left, startY + gapY);
    window.draw(optMenu);

    sf::Text optExit("Exit", ui.getFont(), 28);
    optExit.setFillColor(selectedOption == EXIT ? sf::Color::Yellow : sf::Color::White);
    auto eB = optExit.getLocalBounds();
    optExit.setPosition((static_cast<float>(window.getSize().x) - eB.width) * 0.5f - eB.left, startY + gapY * 2.f);
    window.draw(optExit);

    sf::Text instr("Use LEFT/RIGHT to navigate, ENTER to select", ui.getFont(), 16);
    instr.setFillColor(sf::Color::Cyan);
    auto ib = instr.getLocalBounds();
    instr.setPosition((static_cast<float>(window.getSize().x) - ib.width) * 0.5f - ib.left, 500.f);
    window.draw(instr);
}
