#include "Player.hpp"
#include "LaneConfig.hpp"
#include "Config.hpp"
#include <memory>
#include <cmath>

const float MAX_VELOCITY = 500.0f; // ya no se usa como límite duro
const float ACCELERATION = 300.0f; // no usado en km/h
const float DECELERATION = 150.0f; // no usado en km/h
const float LANE_CHANGE_SPEED = 200.0f;

Player::Player(const std::string& imagePath, float startX, float startY)
    : velocity(0.0f),
      acceleration(ACCELERATION),
      maxVelocity(MAX_VELOCITY),
      deceleration(DECELERATION),
      lane(1),
      health(100.0f),
      imagePath(imagePath),
            laneAnimElapsed(0.0f),
            isLaneChanging(false)
{
        (void)startX; // unused parameter
    // Las posiciones de los carriles ya están definidas en el header
    car = std::make_unique<CarSprite>(imagePath, startY, lane, velocity, true);
    float x = getLaneX(lane);
    car->setPosition(x, startY);
    this->startX = x;
    targetX = x;
    // Velocidad base en km/h
    speedKmh = BASE_SPEED_KMH;
}

void Player::update(float deltaTime)
{
    // Aceleración continua mientras se mantiene la tecla
    if (accelerating)
    {
        speedKmh += ACCEL_RATE_KMH * deltaTime;
    }
    else if (speedKmh > BASE_SPEED_KMH)
    {
        // Volver suavemente hacia la velocidad base si no se acelera
        speedKmh -= DECEL_RATE_KMH * deltaTime;
        if (speedKmh < BASE_SPEED_KMH) speedKmh = BASE_SPEED_KMH;
    }
    // Aplicar frenado progresivo si está activo
    if (braking)
    {
        speedKmh -= BRAKE_RATE_KMH * deltaTime;
        if (speedKmh < BASE_SPEED_KMH) speedKmh = BASE_SPEED_KMH;
    }
    // Convertir a píxeles/segundo (solo para lógica de puntuación/colisión si se necesita)
    velocity = speedKmh * PIXELS_PER_KMH;
    // Mantener el jugador anclado verticalmente: el fondo simula el movimiento
    car->setVelocity(0.0f);
    // Transición suave entre carriles
    if (isLaneChanging && laneAnimTime > 0.0f) {
        laneAnimElapsed += deltaTime;
        float t = laneAnimElapsed / laneAnimTime;
        if (t > 1.0f) t = 1.0f;
        // Suavizado (smoothstep)
        float s = t * t * (3.0f - 2.0f * t);
        float newX = startX + (targetX - startX) * s;
        car->setPosition(newX, car->getY());
        if (t >= 1.0f) {
            isLaneChanging = false;
            startX = targetX;
        }
    }
    car->update(deltaTime);
    // Mantener dentro de pantalla verticalmente
    float y = car->getY();
    if (y < 0)
        car->setPosition(car->getX(), 0);
    if (y > WINDOW_HEIGHT)
        car->setPosition(car->getX(), WINDOW_HEIGHT - 60.0f);
    // Consumimos la bandera para el siguiente frame
    accelerating = false;
}

void Player::accelerate()
{
    accelerating = true;
}

void Player::brake()
{
    // Activar frenado progresivo; la reducción se aplica en update()
    braking = true;
}

void Player::moveLeft()
{
    if (lane > 0 && !isLaneChanging) {
        --lane;
        startX = car->getX();
        targetX = getLaneX(lane);
        laneAnimElapsed = 0.0f;
        isLaneChanging = true;
    }
}

void Player::moveRight()
{
    if (lane < NUM_LANES - 1 && !isLaneChanging) {
        ++lane;
        startX = car->getX();
        targetX = getLaneX(lane);
        laneAnimElapsed = 0.0f;
        isLaneChanging = true;
    }
}

void Player::takeDamage(float damage)
{
    health -= damage;
    if (health < 0)
        health = 0;
}

bool Player::isAlive() const
{
    return health > 0;
}

sf::FloatRect Player::getBounds() const
{
    return car->getBounds();
}

float Player::getVelocity() const
{
    return velocity;
}

float Player::getSpeedKmh() const
{
    return speedKmh;
}

void Player::setAccelerating(bool on)
{
    accelerating = on;
    if (on) braking = false;
}

void Player::brakeStep(float)
{
    braking = true;
}

int Player::getLane() const
{
    return lane;
}

float Player::getX() const
{
    return car->getX();
}

float Player::getY() const
{
    return car->getY();
}

void Player::setHealth(float h)
{
    health = h;
}

float Player::getHealth() const
{
    return health;
}

void Player::render(sf::RenderWindow& window)
{
    car->render(window);
}
