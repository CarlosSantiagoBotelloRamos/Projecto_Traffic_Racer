#include "MapSelectState.hpp"
#include "GameEngine.hpp"
#include "MenuState.hpp"

MapSelectState::MapSelectState(GameEngine* engine)
    : State(engine),
      selectedMap(0)
{
    ui.loadFont("assets/fonts/arial.ttf");

    // Cargar fondo específico del menú Select Map
    std::string bgPath = "assets/images/fondo select map.png";
    if (bgTexture.loadFromFile(bgPath)) {
        bgTexture.setSmooth(true);
        bgSprite.setTexture(bgTexture);
        auto win = engine->getWindow().getSize();
        float scaleX = static_cast<float>(win.x) / bgTexture.getSize().x;
        float scaleY = static_cast<float>(win.y) / bgTexture.getSize().y;
        bgSprite.setScale(scaleX, scaleY);
        bgSprite.setPosition(0.f, 0.f);
    }

    // Usar la selección previa si existe
    if (engine) selectedMap = engine->getSelectedMap();

    // Nombres y rutas de mapas
    mapNames = {"City", "Snow", "Jungle"};
    const std::vector<std::string> mapPaths = {
        "assets/road/road_bg.png", // City/default
        "assets/road/Snow.png",
        "assets/road/Jungle.png"
    };

    mapTextures.resize(mapPaths.size());
    mapSprites.resize(mapPaths.size());

    // Cargar y escalar miniaturas
    const float targetW = 220.f;
    const float targetH = 140.f;
    for (size_t i = 0; i < mapPaths.size(); ++i) {
        if (mapTextures[i].loadFromFile(mapPaths[i])) {
            mapTextures[i].setSmooth(true);
            mapSprites[i].setTexture(mapTextures[i]);
            const auto tw = static_cast<float>(mapTextures[i].getSize().x);
            const auto th = static_cast<float>(mapTextures[i].getSize().y);
            if (tw > 0 && th > 0) {
                float scale = std::min(targetW / tw, targetH / th);
                mapSprites[i].setScale(scale, scale);
            }
        } else {
            // Si falla, sprite quedará vacío; se omitirá al dibujar
        }
    }
}

void MapSelectState::handleInput(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A)
        {
            selectedMap = (selectedMap - 1 + 3) % 3;
        }
        else if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D)
        {
            selectedMap = (selectedMap + 1) % 3;
        }
        else if (event.key.code == sf::Keyboard::Return)
        {
            engine->setSelectedMap(selectedMap);
            engine->popState();
        }
        else if (event.key.code == sf::Keyboard::Escape)
        {
            engine->popState();
        }
    }
}

void MapSelectState::update(float deltaTime)
{
    (void)deltaTime; // Evitar warning de parámetro no usado
    // Update logic if needed
}

void MapSelectState::render(sf::RenderWindow& window)
{
    window.clear(sf::Color::Black);

    // Dibujar fondo si está cargado y reescalar a tamaño de ventana
    if (bgTexture.getSize().x > 0 && bgTexture.getSize().y > 0) {
        auto win = window.getSize();
        float scaleX = static_cast<float>(win.x) / bgTexture.getSize().x;
        float scaleY = static_cast<float>(win.y) / bgTexture.getSize().y;
        bgSprite.setScale(scaleX, scaleY);
        bgSprite.setPosition(0.f, 0.f);
        window.draw(bgSprite);
    }

    // Título removido para dejar la imagen de fondo limpia

    const auto winSize = window.getSize();
    const float centers[3] = { winSize.x * 0.2f, winSize.x * 0.5f, winSize.x * 0.8f };
    const float topY = 120.f;
    const float pad = 6.f;

    for (size_t i = 0; i < mapSprites.size() && i < 3; ++i) {
        auto& spr = mapSprites[i];
        auto bounds = spr.getGlobalBounds();
        float w = bounds.width;
        float h = bounds.height;
        float x = centers[i] - w * 0.5f;
        float y = topY;

        // Marco de selección
        if (static_cast<int>(i) == selectedMap) {
            sf::RectangleShape highlight;
            highlight.setPosition(x - pad, y - pad);
            highlight.setSize(sf::Vector2f(w + pad * 2.f, h + pad * 2.f));
            highlight.setFillColor(sf::Color(0, 0, 0, 0));
            highlight.setOutlineColor(sf::Color::Yellow);
            highlight.setOutlineThickness(3.f);
            window.draw(highlight);
        }

        spr.setPosition(x, y);
        window.draw(spr);

        // Nombre debajo en negro y centrado
        {
            sf::Text label(mapNames[i], ui.getFont(), 22);
            label.setFillColor(sf::Color::Black);
            auto lb = label.getLocalBounds();
            float lx = centers[i] - lb.width * 0.5f - lb.left;
            float ly = y + h + 12.f - lb.top;
            label.setPosition(lx, ly);
            window.draw(label);
        }
    }
    // Instrucciones en negro
    {
        sf::Text instr1("Use UP/DOWN to select, ENTER to confirm", ui.getFont(), 16);
        instr1.setFillColor(sf::Color::Black);
        auto b = instr1.getLocalBounds();
        float x = (winSize.x - b.width) * 0.5f - b.left;
        float y = 500.f - b.top;
        instr1.setPosition(x, y);
        window.draw(instr1);
    }
    {
        sf::Text instr2("Press ESC to go back", ui.getFont(), 16);
        instr2.setFillColor(sf::Color::Black);
        auto b = instr2.getLocalBounds();
        float x = (winSize.x - b.width) * 0.5f - b.left;
        float y = 530.f - b.top;
        instr2.setPosition(x, y);
        window.draw(instr2);
    }
}
