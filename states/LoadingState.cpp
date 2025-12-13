#include "LoadingState.hpp"
#include "GameEngine.hpp"
#include "MenuState.hpp"
#include <filesystem>
#include <iostream>

LoadingState::LoadingState(GameEngine* engine)
    : State(engine)
{
    // No texto: no es necesario cargar fuente
    // Try to load loading image
    std::string imgPath;
    try {
        // Look for Loading.png case-insensitive
        for (const auto& entry : std::filesystem::directory_iterator("assets/images/")) {
            auto name = entry.path().filename().string();
            std::string lower = name; for (auto& ch : lower) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            if (lower == std::string("loading.png")) { imgPath = entry.path().string(); break; }
        }
    } catch (...) {}
    if (imgPath.empty()) {
        const char* candidates[] = { "assets/images/Loading.png", "assets/images/loading.png" };
        for (auto cand : candidates) { if (std::filesystem::exists(cand)) { imgPath = cand; break; } }
    }
    if (!imgPath.empty() && loadingTexture.loadFromFile(imgPath)) {
        loadingTexture.setSmooth(true);
        loadingSprite.setTexture(loadingTexture);
        auto view = engine->getWindow().getView().getSize();
        float scaleX = view.x / static_cast<float>(loadingTexture.getSize().x);
        float scaleY = view.y / static_cast<float>(loadingTexture.getSize().y);
        loadingSprite.setScale(scaleX, scaleY);
        loadingSprite.setPosition(0.f, 0.f);
        std::cout << "[INFO] Splash Loading cargado: " << imgPath << "\n";
    } else {
        std::cout << "[WARNING] No se encontro imagen de Loading (assets/images/Loading.png). Se muestra texto.\n";
    }
}

void LoadingState::handleInput(const sf::Event& event)
{
    if (event.type == sf::Event::Closed) {
        engine->quit();
    }
}

void LoadingState::update(float deltaTime)
{
    elapsed += deltaTime;
    // Animación deshabilitada
    if (elapsed >= duration) {
        // transition to menu
        engine->popState();
        auto menu = std::make_shared<MenuState>(engine);
        engine->pushState(menu);
    }
}

void LoadingState::render(sf::RenderWindow& window)
{
    window.clear(sf::Color::Black);
    if (loadingTexture.getSize().x > 0) {
        auto view = window.getView().getSize();
        // Cover zoom: ajustar escala manteniendo proporción
        float sx = view.x / static_cast<float>(loadingTexture.getSize().x);
        float sy = view.y / static_cast<float>(loadingTexture.getSize().y);
        float scale = std::max(sx, sy);
        loadingSprite.setScale(scale, scale);
        // Centrar
        sf::FloatRect b = loadingSprite.getGlobalBounds();
        float x = (view.x - b.width) * 0.5f;
        float y = (view.y - b.height) * 0.5f;
        loadingSprite.setPosition(x, y);
        window.draw(loadingSprite);
    }
    // Sin texto: solo imagen de fondo
}
