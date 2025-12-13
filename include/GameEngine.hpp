#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <stack>
#include "Config.hpp"

class State;

class GameEngine
{
private:
    sf::RenderWindow window;
    sf::View baseView; // Vista con tamaño virtual fijo (800x600)
    std::stack<std::shared_ptr<State>> states;
    sf::Clock clock;
    bool running;
    bool isFullscreen = false;
    
    // Selecciones del jugador
    int selectedVehicle;
    int selectedMap;
    // Música global de fondo
    sf::Music bgMusic;
    // Volumen maestro de SFX (0-100)
    float sfxVolumeMaster = VOLUME_SFX * AUDIO_VOLUME_MULTIPLIER;
    
public:
    GameEngine();
    ~GameEngine();
    
    void run();
    void handleInput();
    void update(float deltaTime);
    void render();
    
    void pushState(std::shared_ptr<State> state);
    void popState();
    
    sf::RenderWindow& getWindow();
    sf::View& getBaseView();
    
    void setSelectedVehicle(int vehicle);
    void setSelectedMap(int map);
    int getSelectedVehicle() const;
    int getSelectedMap() const;
    
    // Música: acceder y ajustar volumen global
    float getMusicVolume() const;
    void setMusicVolume(float volume);
    float getSfxVolume() const;
    void setSfxVolume(float volume);
    void playMusic();
    void stopMusic();
    
    void quit();

    // Pantalla completa: alternar manteniendo la vista 800x600 con letterbox
    void toggleFullscreen();

private:
    void updateLetterboxView(unsigned int winWidth, unsigned int winHeight);
};

#endif
