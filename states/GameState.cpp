#include "GameState.hpp"
#include "GameEngine.hpp"
#include "GameOverState.hpp"
#include "Config.hpp"
#include "MenuState.hpp"
#include "CarSprite.hpp"
#include <filesystem>
#include "Collision.hpp"
#include <iostream>

// Helper: get car image list
#include <filesystem>
std::vector<std::string> getCarImageList() {
    std::vector<std::string> images;
    std::string dir = "assets/cars/";
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.path().extension() == ".png") {
            images.push_back(entry.path().string());
        }
    }
    return images;
}

GameState::GameState(GameEngine* engine, int selectedVehicle, int selectedMap)
    : State(engine),
      nearMissThreshold(150.0f)
{
    // Asegurar volumen global al máximo
    sf::Listener::setGlobalVolume(VOLUME_GLOBAL);
    // Selección de vehículo
    auto carImages = getCarImageList();
    std::string playerImage = carImages.empty() ? "assets/cars/Police car.png" : carImages[selectedVehicle % carImages.size()];
    player = std::make_unique<Player>(playerImage, 100.0f, 500.0f);
    spawner = std::make_unique<Spawner>(selectedMap);
    spawner->setCarImages(carImages);
    // Generación continua e inmediata (independiente de la aceleración del jugador)
    spawner->spawn();
    score = std::make_unique<Score>();

    ui.loadFont("assets/fonts/arial.ttf");

    // Música global ahora gestionada por GameEngine

    // Audio de aceleración: buscar cualquier archivo que empiece con "acelera"
    std::string accelPath;
    try {
        for (const auto& entry : std::filesystem::directory_iterator("assets/audio/")) {
            const auto name = entry.path().filename().string();
            if (name.rfind("acelera", 0) == 0) { // comienza con "acelera"
                accelPath = entry.path().string();
                break;
            }
        }
    } catch (...) {
        // ignorar errores de FS
    }
    if (!accelPath.empty()) {
        if (accelBuffer.loadFromFile(accelPath)) {
            accelSound.setBuffer(accelBuffer);
            accelSound.setLoop(true);
            {
                float vol = engine->getSfxVolume() * VOLUME_ACCEL_MULTIPLIER;
                if (vol > 100.f) vol = 100.f;
                accelSound.setVolume(vol);
            }
            std::cout << "[INFO] Audio de aceleracion cargado: " << accelPath << "\n";
        } else {
            std::cerr << "[WARNING] No se pudo cargar audio: " << accelPath << "\n";
        }
    } else {
        std::cerr << "[WARNING] No se encontro ningun archivo 'acelera*' en assets/audio/.\n";
    }

    // Audio andando (ralentí): buscar archivo que empiece con "andando"
    std::string idlePath;
    try {
        for (const auto& entry : std::filesystem::directory_iterator("assets/audio/")) {
            const auto name = entry.path().filename().string();
            if (name.rfind("andando", 0) == 0) { // comienza con "andando"
                idlePath = entry.path().string();
                break;
            }
        }
    } catch (...) {}
    if (!idlePath.empty()) {
        if (idleBuffer.loadFromFile(idlePath)) {
            idleSound.setBuffer(idleBuffer);
            idleSound.setLoop(true);
            idleSound.setVolume(engine->getSfxVolume());
            std::cout << "[INFO] Audio de andar cargado: " << idlePath << "\n";
        } else {
            std::cerr << "[WARNING] No se pudo cargar audio: " << idlePath << "\n";
        }
    } else {
        std::cerr << "[WARNING] No se encontro ningun archivo 'andando*' en assets/audio/.\n";
    }

    // Audio Downshift: buscar archivo que empiece con "Downshift" (case-insensitive)
    std::string downshiftPath;
    try {
        for (const auto& entry : std::filesystem::directory_iterator("assets/audio/")) {
            const auto name = entry.path().filename().string();
            std::string lower = name;
            for (auto& ch : lower) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            if (lower.rfind("downshift", 0) == 0) { // comienza con "downshift"
                downshiftPath = entry.path().string();
                break;
            }
        }
    } catch (...) {}
    if (downshiftPath.empty()) {
        // Intentos explícitos por nombre común
        const char* candidates[] = {
            "assets/audio/Downshift.ogg",
            "assets/audio/downshift.ogg",
            "assets/audio/Downshift.wav",
            "assets/audio/downshift.wav"
        };
        for (auto cand : candidates) {
            if (std::filesystem::exists(cand)) { downshiftPath = cand; break; }
        }
    }
    if (!downshiftPath.empty()) {
        if (downshiftBuffer.loadFromFile(downshiftPath)) {
            downshiftSound.setBuffer(downshiftBuffer);
            downshiftSound.setLoop(false);
            downshiftSound.setVolume(engine->getSfxVolume());
            std::cout << "[INFO] Audio Downshift cargado: " << downshiftPath << " (samples=" << downshiftBuffer.getSampleCount() << ")\n";
        } else {
            std::cerr << "[WARNING] No se pudo cargar audio: " << downshiftPath << "\n";
        }
    } else {
        std::cerr << "[WARNING] No se encontro ningun archivo 'Downshift*' en assets/audio/.\n";
    }

    // Audio Freno: buscar archivo que empiece con "freno" (case-insensitive)
    std::string frenoPath;
    try {
        for (const auto& entry : std::filesystem::directory_iterator("assets/audio/")) {
            const auto name = entry.path().filename().string();
            std::string lower = name;
            for (auto& ch : lower) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            if (lower.rfind("freno", 0) == 0) { // comienza con "freno"
                frenoPath = entry.path().string();
                break;
            }
        }
    } catch (...) {}
    if (frenoPath.empty()) {
        const char* candidates[] = {
            "assets/audio/freno.ogg",
            "assets/audio/Freno.ogg",
            "assets/audio/freno.wav",
            "assets/audio/Freno.wav"
        };
        for (auto cand : candidates) {
            if (std::filesystem::exists(cand)) { frenoPath = cand; break; }
        }
    }
    if (!frenoPath.empty()) {
        if (frenoBuffer.loadFromFile(frenoPath)) {
            frenoSound.setBuffer(frenoBuffer);
            frenoSound.setLoop(false);
            {
                float vol = engine->getSfxVolume() * VOLUME_FRENO_MULTIPLIER;
                if (vol > 100.f) vol = 100.f;
                frenoSound.setVolume(vol);
            }
            std::cout << "[INFO] Audio Freno cargado: " << frenoPath << " (samples=" << frenoBuffer.getSampleCount() << ")\n";
        } else {
            std::cerr << "[WARNING] No se pudo cargar audio: " << frenoPath << "\n";
        }
    } else {
        std::cerr << "[WARNING] No se encontro ningun archivo 'freno*' en assets/audio/.\n";
    }

    // Audio Horn: buscar archivo que empiece con "horn" (case-insensitive)
    std::string hornPath;
    try {
        for (const auto& entry : std::filesystem::directory_iterator("assets/audio/")) {
            const auto name = entry.path().filename().string();
            std::string lower = name;
            for (auto& ch : lower) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            if (lower.rfind("horn", 0) == 0) { // comienza con "horn"
                hornPath = entry.path().string();
                break;
            }
        }
    } catch (...) {}
    if (hornPath.empty()) {
        const char* candidates[] = {
            "assets/audio/horn.ogg",
            "assets/audio/Horn.ogg",
            "assets/audio/horn.wav",
            "assets/audio/Horn.wav"
        };
        for (auto cand : candidates) {
            if (std::filesystem::exists(cand)) { hornPath = cand; break; }
        }
    }
    if (!hornPath.empty()) {
        if (hornBuffer.loadFromFile(hornPath)) {
            hornSound.setBuffer(hornBuffer);
            hornSound.setLoop(false);
            hornSound.setVolume(engine->getSfxVolume());
            std::cout << "[INFO] Audio Horn cargado: " << hornPath << " (samples=" << hornBuffer.getSampleCount() << ")\n";
        } else {
            std::cerr << "[WARNING] No se pudo cargar audio: " << hornPath << "\n";
        }
    } else {
        std::cerr << "[WARNING] No se encontro ningun archivo 'horn*' en assets/audio/.\n";
    }

    // (Eliminado) Audio de choque

    // Fondo animado: seleccionar textura según el mapa elegido
    {
        std::string roadPath;
        // selectedMap: 0=City/Default, 1=Snow, 2=Jungle
        switch (selectedMap) {
            case 1: roadPath = "assets/road/Snow.png"; break;
            case 2: roadPath = "assets/road/Jungle.png"; break;
            default: roadPath = "assets/road/road_bg.png"; break;
        }
        if (!std::filesystem::exists(roadPath)) {
            // Fallbacks en caso de nombre faltante
            const char* fallbacks[] = {
                "assets/road/road_bg.png",
                "assets/road/Snow.png",
                "assets/road/Jungle.png"
            };
            for (auto p : fallbacks) {
                if (std::filesystem::exists(p)) { roadPath = p; break; }
            }
        }
        if (!roadTexture.loadFromFile(roadPath)) {
            std::cerr << "[ERROR] No se pudo cargar textura de fondo: " << roadPath << "\n";
        } else {
            std::cout << "[INFO] Fondo del mapa cargado: " << roadPath << "\n";
        }
    }
    roadSprite1.setTexture(roadTexture);
    roadSprite2.setTexture(roadTexture);
    // Escalar el fondo para cubrir toda el área de vista (soporta fullscreen zoom)
    auto viewSize = engine->getWindow().getView().getSize();
    float scaleX = viewSize.x / static_cast<float>(roadTexture.getSize().x);
    float scaleY = viewSize.y / static_cast<float>(roadTexture.getSize().y);
    roadSprite1.setScale(scaleX, scaleY);
    roadSprite2.setScale(scaleX, scaleY);
    // Posicionar para scroll continuo sin huecos
    roadSprite1.setPosition(0.f, 0.f);
    roadSprite2.setPosition(0.f, -viewSize.y);
    roadScrollY = 0.0f;
    roadScrollSpeed = 200.0f; // Puedes ajustar la velocidad
}

void GameState::handleInput(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Escape)
        {
            engine->popState();
            // Asegurar retorno al menú si no hay otro estado debajo
            auto menu = std::make_shared<MenuState>(engine);
            engine->pushState(menu);
            return;
        }
        // Horn: reproducir al presionar tecla 1 o Numpad1
        if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) {
            if (hornBuffer.getSampleCount() > 0) {
                hornSound.play();
                std::cout << "[INFO] Horn triggered (key 1)\n";
            } else {
                std::cout << "[INFO] Horn NOT triggered (samples=" << hornBuffer.getSampleCount() << ")\n";
            }
        }
    }
    
    // Movimiento por toque único de flecha (no repetido)
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
                player->moveLeft();
            }
            if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
                player->moveRight();
            }
        }
}

void GameState::update(float deltaTime)
{
    // Fondo animado: scroll infinito
    float bgHeight = engine->getWindow().getView().getSize().y; // Alto de vista
    roadScrollY += roadScrollSpeed * deltaTime;
    if (roadScrollY >= bgHeight)
        roadScrollY -= bgHeight;
    roadSprite1.setPosition(0, roadScrollY);
    roadSprite2.setPosition(0, roadScrollY - bgHeight);

    // Controles de aceleración/freno por frame (mantener presionado)
    bool up = sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    bool down = sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    if (down) { player->brakeStep(deltaTime); } else { /* liberar frenado si no se presiona */ }
    player->setAccelerating(up);

    // Audio interactivo sin solapamientos:
    // - Acelerar: reproducir 'acelera' (loop), parar 'idle'. No iniciar si 'freno' está sonando.
    // - Frenar: al hacer edge de frenado, reproducir 'freno' una sola vez si velocidad > umbral; parar 'acelera' e 'idle'.
    // - Andando: si no acelera ni frena y no suena 'freno', reproducir 'idle'.
    const float kmh = player->getSpeedKmh();
    bool frenoPlaying = (frenoSound.getStatus() == sf::Sound::Status::Playing);
    if (down) {
        // Edge detection para freno
        if (!prevBrakePressed) {
            // Al iniciar frenado: parar loops inmediatamente
            if (accelSound.getStatus() == sf::Sound::Status::Playing) accelSound.stop();
            if (idleSound.getStatus() == sf::Sound::Status::Playing) idleSound.stop();
            // Reproducir freno solo si supera umbral
            if (kmh >= DOWNSHIFT_MIN_KMH && frenoBuffer.getSampleCount() > 0) {
                float vol = engine->getSfxVolume() * VOLUME_FRENO_MULTIPLIER;
                if (vol > 100.f) vol = 100.f;
                frenoSound.setVolume(vol);
                frenoSound.play();
                std::cout << "[INFO] Freno triggered (>=" << DOWNSHIFT_MIN_KMH << " km/h)\n";
            }
        }
    } else if (up) {
        // Acelerar: si freno está sonando, no iniciar otro audio para evitar solape
        if (!frenoPlaying) {
            if (idleSound.getStatus() == sf::Sound::Status::Playing) idleSound.stop();
            if (accelBuffer.getSampleCount() > 0 && accelSound.getStatus() != sf::Sound::Status::Playing) {
                float vol = engine->getSfxVolume() * VOLUME_ACCEL_MULTIPLIER;
                if (vol > 100.f) vol = 100.f;
                accelSound.setVolume(vol);
                accelSound.play();
            }
        }
    } else {
        // Andando (ralentí): sólo si no suena freno
        if (!frenoPlaying) {
            if (accelSound.getStatus() == sf::Sound::Status::Playing) accelSound.stop();
            if (idleBuffer.getSampleCount() > 0 && idleSound.getStatus() != sf::Sound::Status::Playing) {
                idleSound.play();
            }
        }
    }
    // Downshift: sonar automáticamente una sola vez cuando se deja de acelerar
    if (accelPrevPressed && !up) {
        if (downshiftBuffer.getSampleCount() > 0) {
            // No reproducir si freno está sonando para evitar solape
            if (frenoSound.getStatus() != sf::Sound::Status::Playing) {
                downshiftSound.play();
                std::cout << "[INFO] Downshift auto on release of accelerate\n";
            }
        }
    }
    accelPrevPressed = up;
    prevBrakePressed = down;

    player->update(deltaTime);
    spawner->update(deltaTime);
    // Velocidad de scroll basada en km/h del jugador
    roadScrollSpeed = player->getSpeedKmh() * PIXELS_PER_KMH;
    // Aportar desplazamiento global a los enemigos
    CarSprite::setGlobalScroll(roadScrollSpeed);
    // Actualizar score: distancia con scroll y puntos si > 50 km/h
    score->update(deltaTime, roadScrollSpeed, player->getSpeedKmh());
    score->updateMaxVelocity(player->getSpeedKmh());

    // Dificultad ahora aumenta suavemente en Spawner::update (sin saltos)

    checkCollisions();
    checkNearMisses();

    // Si el jugador muere
    if (!player->isAlive())
    {
        engine->popState();
        auto gameOverState = std::make_shared<GameOverState>(engine,
            score->getPoints(),
            score->getDistance(),
            score->getNearMisses());
        engine->pushState(gameOverState);
    }
}

void GameState::render(sf::RenderWindow& window)
{
    window.clear();

    // Fondo animado infinito
    window.draw(roadSprite2);
    window.draw(roadSprite1);

    // Garantizar cobertura total con una tercera copia si fuera necesario
    if (roadScrollY < 0) {
        sf::Sprite extraSprite(roadTexture);
        extraSprite.setScale(roadSprite1.getScale());
        float bgHeight = engine->getWindow().getView().getSize().y;
        extraSprite.setPosition(0.f, roadScrollY + bgHeight);
        window.draw(extraSprite);
    }

    // Dibujar vehículos
    spawner->render(window);
    player->render(window);

    // Dibujar HUD
    ui.drawHUD(window, score->getPoints(), score->getDistance(),
               player->getSpeedKmh());
    // Mostrar récord durante la partida
    ui.drawText(window, "High Score: " + std::to_string(score->getHighScore()), 10, 85, 20, sf::Color::Black);

    ui.drawText(window, "Health: " + std::to_string(static_cast<int>(player->getHealth())) + "%",
                650, 10, 16, sf::Color::Black);
    ui.drawText(window, "Press ESC to quit", 650, 35, 12, sf::Color::Black);
}

GameState::~GameState() {
    // Música global se detiene desde GameEngine al cerrar si es necesario
}

void GameState::checkCollisions()
{
    auto& cars = spawner->getCars();
    for (size_t i = 0; i < cars.size(); ++i)
    {
        if (player->getBounds().intersects(cars[i]->getBounds()))
        {
            // Game Over inmediato al primer contacto
            player->setHealth(0.0f);
            score->recordCollision();
            // (Eliminado) reproducción de audio de choque
            // No removemos el coche para que se vea el contacto en la transición al Game Over
            return; // salir temprano para que el update procese el Game Over
        }
    }
}

void GameState::checkNearMisses()
{
    auto& cars = spawner->getCars();
    for (size_t i = 0; i < cars.size(); ++i)
    {
        float dist = std::abs(player->getY() - cars[i]->getY());
        if (dist < nearMissThreshold && !player->getBounds().intersects(cars[i]->getBounds()))
        {
            score->recordNearMiss();
        }
    }
}



