#include "GameEngine.hpp"
#include "State.hpp"
#include <filesystem>
#include "Config.hpp"
#include <iostream>

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
        const std::string relPath = "assets/audio/music.ogg";
        const std::string absPath = "C:/Users/csbr1/OneDrive/Escritorio/EjemplosJuego/assets/audio/music.ogg";
        std::string usePath;
        if (std::filesystem::exists(relPath)) {
            usePath = relPath;
        } else if (std::filesystem::exists(absPath)) {
            usePath = absPath;
        }
        if (!usePath.empty()) {
            if (bgMusic.openFromFile(usePath)) {
                bgMusic.setLoop(true);
                bgMusic.setVolume(VOLUME_MUSIC * AUDIO_VOLUME_MULTIPLIER);
                // No reproducir aquí: se iniciará al entrar al menú
            } else {
                std::cerr << "[WARNING] No se pudo abrir musica: " << usePath << "\n";
            }
        } else {
            std::cerr << "[WARNING] No se encontro music.ogg en rutas esperadas.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "[WARNING] Excepcion cargando musica: " << e.what() << "\n";
    }
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

        // Toggle fullscreen with F11 or Alt+Enter
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::F11 ||
                (event.key.code == sf::Keyboard::Enter && (event.key.alt))) {
                toggleFullscreen();
                // After recreating window, continue processing
                continue;
            }
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

// Aspecto 800x600: letterbox en ventana; "cover zoom" en fullscreen
void GameEngine::updateLetterboxView(unsigned int winWidth, unsigned int winHeight)
{
    const float targetW = 800.f;
    const float targetH = 600.f;
    const float windowRatio = static_cast<float>(winWidth) / static_cast<float>(winHeight);
    const float targetRatio = targetW / targetH;

    if (isFullscreen) {
        // Cover zoom: llenar pantalla sin barras, manteniendo proporción (recorta área visible)
        // Ajustar tamaño de la vista al aspect ratio de la ventana
        if (windowRatio > targetRatio) {
            // Ventana más ancha: conservar ancho 800 y reducir alto para igualar ratio
            float newH = targetW / windowRatio; // e.g., 800 / 1.777... = 450
            baseView.setSize(targetW, newH);
        } else if (windowRatio < targetRatio) {
            // Ventana más alta: conservar alto 600 y reducir ancho para igualar ratio
            float newW = targetH * windowRatio; // e.g., 600 * 1.25 = 750
            baseView.setSize(newW, targetH);
        } else {
            baseView.setSize(targetW, targetH);
        }
        baseView.setCenter(targetW * 0.5f, targetH * 0.5f);
        baseView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
        window.setView(baseView);
        return;
    }

    // Letterbox para modo ventana (sin distorsión, con barras)
    sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);
    if (windowRatio > targetRatio) {
        float widthRatio = targetRatio / windowRatio;
        viewport.width = widthRatio;
        viewport.left = (1.f - widthRatio) * 0.5f;
    } else if (windowRatio < targetRatio) {
        float heightRatio = windowRatio / targetRatio;
        viewport.height = heightRatio;
        viewport.top = (1.f - heightRatio) * 0.5f;
    }
    baseView.setSize(targetW, targetH);
    baseView.setCenter(targetW * 0.5f, targetH * 0.5f);
    baseView.setViewport(viewport);
    window.setView(baseView);
}

void GameEngine::toggleFullscreen()
{
    // Preserve current window size for windowed mode
    sf::Vector2u currentSize = window.getSize();
    // Get current style toggle
    isFullscreen = !isFullscreen;
    // Need to recreate the window with new style
    window.close();
    if (isFullscreen) {
        auto mode = sf::VideoMode::getDesktopMode();
        window.create(mode, "Traffic Racer", sf::Style::Fullscreen);
    } else {
        window.create(sf::VideoMode(800, 600), "Traffic Racer", sf::Style::Titlebar | sf::Style::Close);
        window.setSize(currentSize); // keep last size if user resized
    }
    window.setFramerateLimit(60);
    // Reset the base view to 800x600 and apply letterbox
    baseView = sf::View(sf::FloatRect(0.f, 0.f, 800.f, 600.f));
    updateLetterboxView(window.getSize().x, window.getSize().y);
}

float GameEngine::getMusicVolume() const
{
    return bgMusic.getVolume();
}

void GameEngine::setMusicVolume(float volume)
{
    if (volume < 0.f) volume = 0.f;
    if (volume > 100.f) volume = 100.f;
    bgMusic.setVolume(volume);
}

float GameEngine::getSfxVolume() const
{
    return sfxVolumeMaster;
}

void GameEngine::setSfxVolume(float volume)
{
    if (volume < 0.f) volume = 0.f;
    if (volume > 100.f) volume = 100.f;
    sfxVolumeMaster = volume;
}

void GameEngine::playMusic()
{
    if (bgMusic.getStatus() != sf::Music::Status::Playing) {
        bgMusic.play();
    }
}

void GameEngine::stopMusic()
{
    if (bgMusic.getStatus() == sf::Music::Status::Playing) {
        bgMusic.stop();
    }
}
