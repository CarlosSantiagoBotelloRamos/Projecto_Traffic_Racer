#ifndef MAP_SELECT_STATE_HPP
#define MAP_SELECT_STATE_HPP

#include "State.hpp"
#include "UI.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class MapSelectState : public State
{
private:
    UI ui;
    int selectedMap;
    // Recursos para previsualización de mapas
    std::vector<sf::Texture> mapTextures;  // City, Snow, Jungle
    std::vector<sf::Sprite> mapSprites;
    std::vector<std::string> mapNames;     // "City", "Snow", "Jungle"
    // Fondo del menú Select Map
    sf::Texture bgTexture;
    sf::Sprite bgSprite;
    
public:
    MapSelectState(GameEngine* engine);
    
    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
};

#endif
