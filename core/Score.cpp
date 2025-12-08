#include "Score.hpp"
#include <fstream>
#include <algorithm>

static const char* HIGHSCORE_FILE = "highscore.dat";

Score::Score()
    : points(0),
      distanceTraveled(0.0f),
      nearMisses(0),
      maxVelocity(0.0f),
      collisions(0),
      timeAlive(0.0f),
      highScore(0),
      pointsAccumulator(0.0f)
{
    highScore = readHighScoreFromFile();
}

void Score::update(float deltaTime, float scrollPxPerSec, float playerKmh)
{
    timeAlive += deltaTime;

    // Puntos solo cuando se circula a más de 50 km/h
    if (playerKmh > 50.0f) {
        pointsAccumulator += deltaTime;
        while (pointsAccumulator >= 1.0f) {
            addPoints(1);
            pointsAccumulator -= 1.0f;
        }
    }

    // Actualizar distancia con el scroll del fondo (px -> metros aprox.)
    distanceTraveled += (scrollPxPerSec * deltaTime) / 100.0f;

    // Actualizar récord
    if (points > highScore) {
        highScore = points;
        writeHighScoreToFile(highScore);
    }
}

void Score::addPoints(int pts)
{
    points += pts;
}

void Score::recordNearMiss()
{
    nearMisses++;
    addPoints(50); // Bonus por esquivar
}

void Score::recordCollision()
{
    collisions++;
    points -= 100; // Penalización por colisión
    if (points < 0)
        points = 0;
}

void Score::updateDistance(float distance)
{
    distanceTraveled = distance;
}

void Score::updateMaxVelocity(float velocity)
{
    if (velocity > maxVelocity)
        maxVelocity = velocity;
}

int Score::getPoints() const
{
    return points;
}

float Score::getDistance() const
{
    return distanceTraveled;
}

int Score::getNearMisses() const
{
    return nearMisses;
}

float Score::getMaxVelocity() const
{
    return maxVelocity;
}

int Score::getCollisions() const
{
    return collisions;
}

float Score::getTimeAlive() const
{
    return timeAlive;
}

int Score::getHighScore() const
{
    return highScore;
}

void Score::resetHighScore()
{
    highScore = 0;
    writeHighScoreToFile(highScore);
}

void Score::reset()
{
    points = 0;
    distanceTraveled = 0.0f;
    nearMisses = 0;
    maxVelocity = 0.0f;
    collisions = 0;
    timeAlive = 0.0f;
    pointsAccumulator = 0.0f;
}

int Score::readHighScoreFromFile()
{
    std::ifstream in(HIGHSCORE_FILE, std::ios::in | std::ios::binary);
    if (!in) return 0;
    int v = 0;
    in.read(reinterpret_cast<char*>(&v), sizeof(v));
    if (!in) return 0;
    return std::max(0, v);
}

void Score::writeHighScoreToFile(int value)
{
    std::ofstream out(HIGHSCORE_FILE, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!out) return;
    out.write(reinterpret_cast<const char*>(&value), sizeof(value));
}
