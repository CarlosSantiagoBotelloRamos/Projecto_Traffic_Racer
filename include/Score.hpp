#ifndef SCORE_HPP
#define SCORE_HPP

class Score
{
private:
    int points;
    float distanceTraveled;
    int nearMisses;
    float maxVelocity;
    int collisions;
    float timeAlive;
    int highScore;
    float pointsAccumulator; // para sumar 1 punto/seg cuando aplique
    
    static int readHighScoreFromFile();
    static void writeHighScoreToFile(int value);
    
public:
    Score();
    
    // scrollPxPerSec: velocidad del fondo en px/s; playerKmh: velocidad del jugador en km/h
    void update(float deltaTime, float scrollPxPerSec, float playerKmh);
    void addPoints(int pts);
    void recordNearMiss();
    void recordCollision();
    void updateDistance(float distance);
    void updateMaxVelocity(float velocity);
    
    int getPoints() const;
    float getDistance() const;
    int getNearMisses() const;
    float getMaxVelocity() const;
    int getCollisions() const;
    float getTimeAlive() const;
    int getHighScore() const;
    void resetHighScore();
    
    void reset();
};

#endif
