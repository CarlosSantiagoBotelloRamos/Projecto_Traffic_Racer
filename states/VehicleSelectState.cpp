#include "VehicleSelectState.hpp"
#include "GameEngine.hpp"
#include "Config.hpp"
#include "MenuState.hpp"

// Helper: get car image list (case-insensitive .png)
#include <filesystem>
#include <vector>
#include <cctype>
std::vector<std::string> getCarImageListVS() {
    std::vector<std::string> images;
    std::string dir = "assets/cars/";
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            auto ext = entry.path().extension().string();
            for (auto& c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            if (ext == ".png") {
                images.push_back(entry.path().string());
            }
        }
    } catch (...) {
        // If directory can't be read, return empty list
    }
    return images;
}

VehicleSelectState::VehicleSelectState(GameEngine* engine)
    : State(engine),
      selectedVehicle(0),
      scrollOffset(0.0f)
{
    ui.loadFont("assets/fonts/arial.ttf");
    // Cargar fondo específico del menú Select Vehicle
    std::string bgPath = "assets/images/fondo select car.png";
    if (bgTexture.loadFromFile(bgPath)) {
        bgTexture.setSmooth(true);
        bgSprite.setTexture(bgTexture);
        auto win = engine->getWindow().getSize();
        float scaleX = static_cast<float>(win.x) / bgTexture.getSize().x;
        float scaleY = static_cast<float>(win.y) / bgTexture.getSize().y;
        bgSprite.setScale(scaleX, scaleY);
        bgSprite.setPosition(0.f, 0.f);
    }

    // Cargar audio 'rev' (case-insensitive, prefijo 'rev')
    try {
        for (const auto& entry : std::filesystem::directory_iterator("assets/audio/")) {
            auto name = entry.path().filename().string();
            std::string lower = name;
            for (auto& ch : lower) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            if (lower.rfind("rev", 0) == 0) {
                if (revBuffer.loadFromFile(entry.path().string())) {
                    revSound.setBuffer(revBuffer);
                    revSound.setLoop(false);
                    revSound.setVolume(VOLUME_SFX);
                }
                break;
            }
        }
    } catch (...) {}
}

void VehicleSelectState::handleInput(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        auto carImages = getCarImageListVS();
        int total = static_cast<int>(carImages.size());
        if (total == 0) total = 1;
        if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A)
        {
            selectedVehicle = (selectedVehicle - 1 + total) % total;
        }
        else if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D)
        {
            selectedVehicle = (selectedVehicle + 1) % total;
        }
        else if (event.key.code == sf::Keyboard::Return)
        {
            if (revBuffer.getSampleCount() > 0) {
                revSound.play();
            }
            engine->setSelectedVehicle(selectedVehicle);
            engine->popState();
        }
        else if (event.key.code == sf::Keyboard::Escape)
        {
            engine->popState();
        }
    }
}

void VehicleSelectState::update(float deltaTime)
{
    (void)deltaTime; // Evitar warning de parámetro no usado
    // Update logic if needed
}

void VehicleSelectState::render(sf::RenderWindow& window)
{
    window.clear(sf::Color::Black);
    // Draw background once, scaled to window size
    if (bgTexture.getSize().x > 0 && bgTexture.getSize().y > 0) {
        auto win = window.getSize();
        float sX = static_cast<float>(win.x) / bgTexture.getSize().x;
        float sY = static_cast<float>(win.y) / bgTexture.getSize().y;
        bgSprite.setScale(sX, sY);
        bgSprite.setPosition(0.f, 0.f);
        window.draw(bgSprite);
    }
    auto carImages = getCarImageListVS();
    int total = static_cast<int>(carImages.size());
    if (total == 0) {
        ui.drawText(window, "No car images found!", 300, 300, 28, sf::Color::Red);
    } else {
        // Tamaño ampliado para los sprites
        float bigScaleX = 220.0f * 0.75f; // 25% más delgado
        float bigScaleY = 330.0f;
        float smallScaleX = 120.0f * 0.75f;
        float smallScaleY = 180.0f;
        float centerX = 400.0f;
        float centerY = 200.0f;
        for (int i = 0; i < total; ++i) {
            sf::Texture tex;
            if (tex.loadFromFile(carImages[i])) {
                sf::Sprite spr(tex);
                float scaleX, scaleY, x, y;
                if (i == selectedVehicle) {
                    scaleX = bigScaleX / tex.getSize().x;
                    scaleY = bigScaleY / tex.getSize().y;
                    x = centerX - bigScaleX / 2.0f;
                    y = centerY - bigScaleY / 2.0f;
                    spr.setColor(sf::Color(255, 255, 100));
                } else {
                    scaleX = smallScaleX / tex.getSize().x;
                    scaleY = smallScaleY / tex.getSize().y;
                    float offset = (i - selectedVehicle) * 250.0f;
                    x = centerX - smallScaleX / 2.0f + offset;
                    y = centerY - smallScaleY / 2.0f + 80.0f;
                    spr.setColor(sf::Color(180, 180, 180));
                }
                spr.setScale(scaleX, scaleY);
                spr.setPosition(x, y);
                window.draw(spr);
                // Draw centered black label below the sprite
                const std::string name = std::filesystem::path(carImages[i]).stem().string();
                unsigned int fontSize = (i == selectedVehicle ? 28 : 18);
                sf::Text label(name, ui.getFont(), fontSize);
                label.setFillColor(sf::Color::Black);
                auto lb = label.getLocalBounds();
                float spriteW = tex.getSize().x * scaleX;
                float spriteH = tex.getSize().y * scaleY;
                float labelX = x + (spriteW - lb.width) * 0.5f - lb.left;
                float labelY = y + spriteH + 10.f - lb.top;
                label.setPosition(labelX, labelY);
                window.draw(label);
            }
        }
    }
    // Instrucciones centradas abajo en color negro
    const auto viewSize = window.getView().getSize();
    float vw = viewSize.x; float vh = viewSize.y;
    {
        sf::Text instr1("Use LEFT/RIGHT arrows to select, ENTER to confirm", ui.getFont(), 16);
        instr1.setFillColor(sf::Color::Black);
        auto b = instr1.getLocalBounds();
        float x = (vw - b.width) * 0.5f - b.left;
        float y = vh - 60.f - b.top;
        instr1.setPosition(x, y);
        window.draw(instr1);
    }
    {
        sf::Text instr2("Press ESC to go back", ui.getFont(), 16);
        instr2.setFillColor(sf::Color::Black);
        auto b = instr2.getLocalBounds();
        float x = (vw - b.width) * 0.5f - b.left;
        float y = vh - 30.f - b.top;
        instr2.setPosition(x, y);
        window.draw(instr2);
    }
}
