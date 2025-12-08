#include "GameEngine.hpp"
#include "State.hpp"
#include <filesystem>
#include "Config.hpp"

GameEngine::GameEngine() 
    : window(sf::VideoMode(800, 600), "Traffic Racer"),
      running(true),
      selectedVehicle(0),
      selectedMap(0)
{
    window.setFramerateLimit(60);
    // Configurar vista base con tamaño virtual fijo 800x600
    baseView = sf::View(sf::FloatRect(0.f, 0.f, 800.f, 600.f));
    window.setView(baseView);

    // Música global de fondo (menus + juego)
    try {
        std::string musicPath = "assets/audio/music.ogg";
        if (std::filesystem::exists(musicPath)) {
            if (bgMusic.openFromFile(musicPath)) {
                bgMusic.setLoop(true);
                bgMusic.setVolume(VOLUME_MUSIC);
                bgMusic.play();
            }
        }
    } catch (...) {}
}

GameEngine::~GameEngine() = default;

void GameEngine::run()
{
    while (running && window.isOpen())
    {
        handleInput();
        
        float deltaTime = clock.restart().asSeconds();
        update(deltaTime);
        render();
    }
}

void GameEngine::handleInput()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            running = false;

        if (event.type == sf::Event::Resized) {
            updateLetterboxView(event.size.width, event.size.height);
        }
        
        if (!states.empty())
            states.top()->handleInput(event);
    }
}

void GameEngine::update(float deltaTime)
{
    if (!states.empty())
        states.top()->update(deltaTime);
}

void GameEngine::render()
{
    window.clear(sf::Color::Black);
    
    if (!states.empty())
        states.top()->render(window);
    
    window.display();
}

void GameEngine::pushState(std::shared_ptr<State> state)
{
    states.push(state);
}

void GameEngine::popState()
{
    if (!states.empty())
        states.pop();
}

sf::RenderWindow& GameEngine::getWindow()
{
    return window;
}

sf::View& GameEngine::getBaseView()
{
    return baseView;
}

void GameEngine::setSelectedVehicle(int vehicle)
{
    selectedVehicle = vehicle;
}

void GameEngine::setSelectedMap(int map)
{
    selectedMap = map;
}

int GameEngine::getSelectedVehicle() const
{
    return selectedVehicle;
}

int GameEngine::getSelectedMap() const
{
    return selectedMap;
}

void GameEngine::quit()
{
    running = false;
}

// Mantener aspecto 800x600 con letterboxing
void GameEngine::updateLetterboxView(unsigned int winWidth, unsigned int winHeight)
{
    const float targetW = 800.f;
    const float targetH = 600.f;
    float windowRatio = static_cast<float>(winWidth) / static_cast<float>(winHeight);
    float targetRatio = targetW / targetH;

    sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);
    if (windowRatio > targetRatio) {
        // Ventana más ancha: barras verticales
        float widthRatio = targetRatio / windowRatio;
        viewport.width = widthRatio;
        viewport.left = (1.f - widthRatio) * 0.5f;
        viewport.top = 0.f;
        viewport.height = 1.f;
    } else if (windowRatio < targetRatio) {
        // Ventana más alta: barras horizontales
        float heightRatio = windowRatio / targetRatio;
        viewport.height = heightRatio;
        viewport.top = (1.f - heightRatio) * 0.5f;
        viewport.left = 0.f;
        viewport.width = 1.f;
    }
    baseView.setViewport(viewport);
    window.setView(baseView);
}
