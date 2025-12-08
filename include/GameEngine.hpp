#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <stack>

class State;

class GameEngine
{
private:
    sf::RenderWindow window;
    sf::View baseView; // Vista con tamaño virtual fijo (800x600)
    std::stack<std::shared_ptr<State>> states;
    sf::Clock clock;
    bool running;
    
    // Selecciones del jugador
    int selectedVehicle;
    int selectedMap;
    // Música global de fondo
    sf::Music bgMusic;
    
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
    
    void quit();

private:
    void updateLetterboxView(unsigned int winWidth, unsigned int winHeight);
};

#endif
