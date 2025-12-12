#include "MenuState.hpp"
#include "GameEngine.hpp"
#include "GameState.hpp"
#include "SettingsState.hpp"
#include "VehicleSelectState.hpp"
#include "MapSelectState.hpp"
#include "Score.hpp"
#include <iostream>
#include <filesystem>

MenuState::MenuState(GameEngine* engine)
        : State(engine),
            selectedOption(START_GAME)
{
    // Iniciar música de fondo al entrar al menú principal
    if (engine) engine->playMusic();
    if (!ui.loadFont("assets/fonts/arial.ttf")) {
        std::cerr << "[ERROR] No se pudo cargar la fuente assets/fonts/arial.ttf.\n";
        std::cerr << "Asegúrate de que el archivo existe y es una fuente TTF válida.\n";
        std::cerr << "Puedes copiar una fuente desde C:/Windows/Fonts/.\n" << std::endl;
        if (engine) engine->quit();
    }

    // Cargar fondo del menú: buscar "fondo" (case-insensitive) en assets/images/
    std::string bgPath;
    try {
        for (const auto& entry : std::filesystem::directory_iterator("assets/images/")) {
            const auto name = entry.path().filename().string();
            std::string lower = name; for (auto& ch : lower) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            if (lower == std::string("fondo.png")) { bgPath = entry.path().string(); break; }
        }
    } catch (...) {}
    if (bgPath.empty()) {
        const char* candidates[] = { "assets/images/fondo.png", "assets/images/Fondo.png" };
        for (auto cand : candidates) { if (std::filesystem::exists(cand)) { bgPath = cand; break; } }
    }
    if (!bgPath.empty() && bgTexture.loadFromFile(bgPath)) {
        bgTexture.setSmooth(true);
        bgSprite.setTexture(bgTexture);
        auto win = engine->getWindow().getSize();
        float scaleX = static_cast<float>(win.x) / bgTexture.getSize().x;
        float scaleY = static_cast<float>(win.y) / bgTexture.getSize().y;
        bgSprite.setScale(scaleX, scaleY);
        bgSprite.setPosition(0.f, 0.f);
        std::cout << "[INFO] Fondo de menú cargado: " << bgPath << "\n";
    } else {
        std::cerr << "[WARNING] No se pudo cargar el fondo de menú (fondo.png). Verifica assets/images/.\n";
    }
}

void MenuState::handleInput(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Up)
        {
            selectedOption = (selectedOption - 1 + 5) % 5;
        }
        else if (event.key.code == sf::Keyboard::Down)
        {
            selectedOption = (selectedOption + 1) % 5;
        }
        else if (event.key.code == sf::Keyboard::Return)
        {
            switch (selectedOption)
            {
                case START_GAME:
                {
                    auto gameState = std::make_shared<GameState>(engine, 
                        engine->getSelectedVehicle(), 
                        engine->getSelectedMap());
                    engine->pushState(gameState);
                    break;
                }
                case VEHICLE_SELECT:
                {
                    auto vehicleState = std::make_shared<VehicleSelectState>(engine);
                    engine->pushState(vehicleState);
                    break;
                }
                case MAP_SELECT:
                {
                    auto mapState = std::make_shared<MapSelectState>(engine);
                    engine->pushState(mapState);
                    break;
                }
                case SETTINGS:
                {
                    auto settingsState = std::make_shared<SettingsState>(engine);
                    engine->pushState(settingsState);
                    break;
                }
                case QUIT:
                    engine->quit();
                    break;
            }
        }
    }
}

void MenuState::update(float deltaTime)
{
    (void)deltaTime; // Evitar warning de parámetro no usado
    // Update logic if needed
}

void MenuState::render(sf::RenderWindow& window)
{
    window.clear(sf::Color::Black);
    // Dibujar fondo si está cargado
    if (bgTexture.getSize().x > 0 && bgTexture.getSize().y > 0) {
        // Reajustar escala si la ventana cambió de tamaño
        auto win = window.getSize();
        float scaleX = static_cast<float>(win.x) / bgTexture.getSize().x;
        float scaleY = static_cast<float>(win.y) / bgTexture.getSize().y;
        bgSprite.setScale(scaleX, scaleY);
        window.draw(bgSprite);
    }
    
    // Se elimina el título "TRAFFIC RACER" del menú principal
    
    // Menú
    // Botones verdes (verde limón) con texto blanco, el seleccionado se amplia 1px
    struct Item { const char* label; int id; float y; };
    Item items[] = {
        { "Start Game", START_GAME, 140.f },
        { "Select Vehicle", VEHICLE_SELECT, 195.f },
        { "Select Map", MAP_SELECT, 250.f },
        { "Settings", SETTINGS, 305.f },
        { "Quit", QUIT, 350.f }
    };
    // Usar tamaño de la vista (coordenadas virtuales) para posicionar
    const auto viewSize = window.getView().getSize();
    const float vw = viewSize.x;
    const float vh = viewSize.y;
    const float baseW = 150.f; // mitad de ancho (más delgado horizontalmente)
    const float baseH = 44.f;
    const sf::Color lime(67, 176, 42); // Pantone Lime Green (approx PANTONE 368 C #43B02A)
    const float margin = 20.f;
    const float gap = 24.f;          // mayor separación entre botones
    const float marginSide = margin * 3.0f;      // triple separación lateral
    const float marginBottom = margin * 2.5f;    // 2.5x separación inferior
    const float lift = 0.f;          // bajar el conjunto (alinear con Quit izq.)
    for (size_t idx = 0; idx < sizeof(items)/sizeof(items[0]); ++idx) {
        const auto& it = items[idx];
        bool sel = (selectedOption == it.id);
        float w = baseW + (sel ? 2.f : 0.f);
        float h = baseH + (sel ? 2.f : 0.f);
        float x = 0.f;
        float y = 0.f;
        if (it.id == QUIT || it.id == SETTINGS) {
            // Quit en esquina inferior izquierda con márgenes ampliados
            x = marginSide;
            // Si es SETTINGS, colocarlo justo encima de Quit
            if (it.id == QUIT) {
                y = vh - marginBottom - h;
            } else {
                y = vh - marginBottom - (2.f * baseH + gap);
            }
        } else {
            // Los otros tres en esquina inferior derecha, apilados hacia arriba
            int orderIndex = 0; // 0=Start, 1=Select Vehicle, 2=Select Map
            if (it.id == START_GAME) orderIndex = 0;
            else if (it.id == VEHICLE_SELECT) orderIndex = 1;
            else if (it.id == MAP_SELECT) orderIndex = 2;
            float stackTopY = vh - marginBottom - (3.f * baseH + 2.f * gap) - lift;
            y = stackTopY + orderIndex * (baseH + gap) - (sel ? 1.f : 0.f);
            x = vw - marginSide - w;
        }
        sf::Color textColor = sel ? sf::Color::Black : sf::Color::White;
        ui.drawButton(window, it.label, x, y, w, h, lime, textColor);
    }
    
    {
        // Posicionar las instrucciones centradas horizontalmente y
        // a la mitad de la separación inferior respecto al botón Quit
        sf::Text instr("Use UP/DOWN arrows to navigate, ENTER to select", ui.getFont(), 16);
        instr.setFillColor(sf::Color::Black);
        auto b = instr.getLocalBounds();
        float ix = (vw - b.width) * 0.5f - b.left;
        // Separación inferior (marginBottom) ya definida arriba
        float iy = vh - (marginBottom * 0.5f) - (b.height * 0.5f) - b.top;
        instr.setPosition(ix, iy);
        window.draw(instr);
    }

    // Mostrar récord guardado
    int high = Score().getHighScore();
    ui.drawText(window, "High Score: " + std::to_string(high), 320, 400, 20, sf::Color::Black);
}
