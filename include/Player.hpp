#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>

// Player now uses CarSprite for sprite-based rendering
#include "CarSprite.hpp"
#include <memory>

class Player {
private:
    std::unique_ptr<CarSprite> car;
    float velocity;      // píxeles/segundo (derivado de km/h)
    float acceleration;
    float maxVelocity;
    float deceleration;
    int lane; // índice de carril (0-3)
    float health;
    std::string imagePath;
    // Movimiento por carriles
    static constexpr int NUM_LANES = 4;
    float visualWidth = 150.0f;
    float laneAnimTime = 0.35f; // transición suave de carril
    float laneAnimElapsed = 0.0f;
    bool isLaneChanging = false;
    float startX = 0.0f;

    float targetX = 0.0f;

    // Nueva lógica de velocidad en km/h
    float speedKmh = 0.0f;
    bool accelerating = false;
    bool braking = false;

public:
    Player(const std::string& imagePath, float startX, float startY);
    void update(float deltaTime);
    void accelerate();
    void brake();
    void brakeStep(float);
    void setAccelerating(bool on);
    void moveLeft();
    void moveRight();
    void takeDamage(float damage);
    bool isAlive() const;
    sf::FloatRect getBounds() const;
    float getVelocity() const;
    float getSpeedKmh() const;
    int getLane() const;
    float getX() const;
    float getY() const;
    void setHealth(float h);
    float getHealth() const;
    void render(sf::RenderWindow& window);
};

#endif
