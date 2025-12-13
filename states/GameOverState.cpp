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
                int vehicle = engine->getSelectedVehicle();
                int map = engine->getSelectedMap();
                auto gameState = std::make_shared<GameState>(engine, vehicle, map);
                engine->pushState(gameState);
            }
            else if (selectedOption == RETURN_MENU)
            {
                engine->popState();
                auto menuState = std::make_shared<MenuState>(engine);
                engine->pushState(menuState);
            }
            else if (selectedOption == EXIT)
            {
                engine->quit();
            }
        }
    }

    // Soporte de ratón: clic para activar opciones y hover para selección
    if (event.type == sf::Event::MouseMoved || event.type == sf::Event::MouseButtonPressed)
    {
        const auto view = engine->getWindow().getView().getSize();
        const float vw = view.x;
        const float vh = view.y;
        const float startY = vh * 0.52f;
        const float gapY = vh * 0.06f;

        // Construir las cajas de impacto usando los bounds de texto con padding
        auto makeRect = [&](const char* label, float y){
            sf::Text t(label, ui.getFont(), 28);
            auto b = t.getLocalBounds();
            float x = (vw - b.width) * 0.5f - b.left;
            const float padX = 20.f;
            const float padY = 10.f;
            return sf::FloatRect(x - padX, y - padY, b.width + 2*padX, b.height + 2*padY);
        };
        sf::FloatRect rRestart = makeRect("Restart Game", startY);
        sf::FloatRect rMenu    = makeRect("Return to Menu", startY + gapY);
        sf::FloatRect rExit    = makeRect("Exit", startY + gapY * 2.f);

        sf::Vector2f pos;
        if (event.type == sf::Event::MouseMoved) {
            pos = sf::Vector2f(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
            if (rRestart.contains(pos)) selectedOption = RESTART;
            else if (rMenu.contains(pos)) selectedOption = RETURN_MENU;
            else if (rExit.contains(pos)) selectedOption = EXIT;
        } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            pos = sf::Vector2f(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
            if (rRestart.contains(pos)) {
                int vehicle = engine->getSelectedVehicle();
                int map = engine->getSelectedMap();
                auto gameState = std::make_shared<GameState>(engine, vehicle, map);
                engine->pushState(gameState);
            } else if (rMenu.contains(pos)) {
                engine->popState();
                auto menuState = std::make_shared<MenuState>(engine);
                engine->pushState(menuState);
            } else if (rExit.contains(pos)) {
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

    const auto view = window.getView().getSize();
    const float vw = view.x;
    const float vh = view.y;

    {
        sf::Text title("GAME OVER", ui.getFont(), 48);
        title.setFillColor(sf::Color::Red);
        auto tb = title.getLocalBounds();
        float tx = (vw - tb.width) * 0.5f - tb.left;
        float ty = vh * 0.10f;
        title.setPosition(tx, ty);
        window.draw(title);
    }

    {
        sf::Text t1("Final Score: " + std::to_string(finalScore), ui.getFont(), 28);
        sf::Text t2("Distance: " + std::to_string(static_cast<int>(finalDistance)) + "m", ui.getFont(), 28);
        sf::Text t3("Near Misses: " + std::to_string(finalNearMisses), ui.getFont(), 28);
        auto b1 = t1.getLocalBounds();
        auto b2 = t2.getLocalBounds();
        auto b3 = t3.getLocalBounds();
        t1.setPosition((vw - b1.width) * 0.5f - b1.left, vh * 0.22f);
        t2.setPosition((vw - b2.width) * 0.5f - b2.left, vh * 0.30f);
        t3.setPosition((vw - b3.width) * 0.5f - b3.left, vh * 0.38f);
        window.draw(t1);
        window.draw(t2);
        window.draw(t3);
    }

    // Opciones: Restart, Return to Menu, Exit
    const float startY = vh * 0.52f;
    const float gapY = vh * 0.06f;
    sf::Text optRestart("Restart Game", ui.getFont(), 28);
    optRestart.setFillColor(selectedOption == RESTART ? sf::Color::Yellow : sf::Color::White);
    auto rB = optRestart.getLocalBounds();
    optRestart.setPosition((vw - rB.width) * 0.5f - rB.left, startY);
    window.draw(optRestart);

    sf::Text optMenu("Return to Menu", ui.getFont(), 28);
    optMenu.setFillColor(selectedOption == RETURN_MENU ? sf::Color::Yellow : sf::Color::White);
    auto mB = optMenu.getLocalBounds();
    optMenu.setPosition((vw - mB.width) * 0.5f - mB.left, startY + gapY);
    window.draw(optMenu);

    sf::Text optExit("Exit", ui.getFont(), 28);
    optExit.setFillColor(selectedOption == EXIT ? sf::Color::Yellow : sf::Color::White);
    auto eB = optExit.getLocalBounds();
    optExit.setPosition((vw - eB.width) * 0.5f - eB.left, startY + gapY * 2.f);
    window.draw(optExit);

    sf::Text instr("Use LEFT/RIGHT to navigate, ENTER to select", ui.getFont(), 16);
    instr.setFillColor(sf::Color::Cyan);
    auto ib = instr.getLocalBounds();
    instr.setPosition((vw - ib.width) * 0.5f - ib.left, vh * 0.90f);
    window.draw(instr);
}
