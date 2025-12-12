#include "Spawner.hpp"
#include "Config.hpp"
#include <random>
#include <ctime>

static std::mt19937 rng(std::time(nullptr));

const float INITIAL_SPEED = 50.0f; // velocidad inicial visible (px/s)
static constexpr float SAFE_SPAWN_BUFFER = 120.0f; // separación vertical mínima entre coches en mismo carril

Spawner::Spawner(int selectedMap)
        : spawnTimer(0.0f),
            spawnInterval(BASE_SPAWN_INTERVAL),
            difficulty(1.0f),
            elapsedSeconds(0.0f),
            growthPerMinute(0.25f),
            spawnTightenPerMinute(1.00f),
            speedGrowthPerSecond(10.0f),
            map(selectedMap)
{
}

void Spawner::update(float deltaTime)
{
    // Incrementar suavemente la dificultad con el tiempo (por minuto)
    elapsedSeconds += deltaTime;
    float minutes = elapsedSeconds / 60.0f;
    difficulty = 1.0f + growthPerMinute * minutes; // lineal y suave
    if (difficulty > 3.0f) difficulty = 3.0f; // limitar a x3 por seguridad

    // Reducir el intervalo de spawn con el tiempo (de 2.0s hacia 0.5s aprox. en 5 minutos)
    float minutes2 = elapsedSeconds / 60.0f;
    spawnInterval = std::max(MIN_SPAWN_INTERVAL, BASE_SPAWN_INTERVAL - spawnTightenPerMinute * minutes2);

    spawnTimer += deltaTime;
    
    if (spawnTimer >= spawnInterval)
    {
        spawn();
        spawnTimer = 0.0f;
    }
    
    // Actualizar autos enemigos
    for (auto& car : cars)
    {
        car->update(deltaTime);
    }
    
    // Remover autos que salieron de pantalla
    for (size_t i = 0; i < cars.size(); )
    {
        if (cars[i]->isOffScreen())
        {
            cars.erase(cars.begin() + i);
        }
        else
        {
            i++;
        }
    }
}

void Spawner::spawn()
{
    // Intentar encontrar un carril seguro (sin encimarse con el coche más cercano en ese carril)
    int lanes[4] = {0, 1, 2, 3};
    std::shuffle(std::begin(lanes), std::end(lanes), rng);

    // Velocidad basada solo en tiempo jugado
    float baseSpeed = INITIAL_SPEED + speedGrowthPerSecond * elapsedSeconds;
    std::uniform_real_distribution<> speedVar(0.95f, 1.05f);
    float candidateSpeed = baseSpeed * speedVar(rng);

    auto getBelowCarInfo = [&](int lane, float spawnY, float& belowY, float& belowSpeed)->bool {
        bool found = false;
        float minY = 1e9f;
        float spd = 0.0f;
        for (const auto& c : cars) {
            if (c->getLane() == lane) {
                float y = c->getY();
                if (y > spawnY && y < minY) { // el más cercano por debajo
                    minY = y;
                    spd = c->getVelocity();
                    found = true;
                }
            }
        }
        if (found) { belowY = minY; belowSpeed = spd; }
        return found;
    };

    auto isLaneSafe = [&](int lane, float newSpeed) {
        // Buscar el coche con mayor Y en este carril (el más cercano por delante)
        bool found = false;
        float frontY = -1e9f;
        float frontSpeed = 0.0f;
        for (const auto& c : cars) {
            if (c->getLane() == lane) {
                if (!found || c->getY() > frontY) {
                    found = true;
                    frontY = c->getY();
                    frontSpeed = c->getVelocity();
                }
            }
        }
        if (!found) return true; // libre
        // Distancia al frente desde la posición de spawn
        float dy = frontY - (-60.0f) - SAFE_SPAWN_BUFFER;
        if (dy <= 0) return false; // demasiado cerca
        // Si el nuevo es más lento o igual, no alcanzará
        if (newSpeed <= frontSpeed) return true;
        // Si es más rápido, estimar si alcanzará antes de que el de delante salga de pantalla
        float relative = newSpeed - frontSpeed;
        float timeToCatch = dy / relative;
        if (frontSpeed <= 0.0f) return false; // el de delante detenido haría inevitable el alcance
        float timeToOff = (WINDOW_HEIGHT - frontY) / frontSpeed;
        return timeToCatch >= timeToOff; // seguro si alcanza después de que salga
    };

    int chosenLane = -1;
    for (int i = 0; i < 4; ++i) {
        float belowY = 0.0f, belowSpd = 0.0f;
        // Enforce orden: el de arriba siempre más lento que el de abajo
        if (getBelowCarInfo(lanes[i], -60.0f, belowY, belowSpd)) {
            const float margin = 5.0f;
            if (belowSpd - margin > 5.0f) {
                candidateSpeed = std::min(candidateSpeed, belowSpd - margin);
            } else {
                // Muy poco margen, probar otro carril
                continue;
            }
        }
        if (isLaneSafe(lanes[i], candidateSpeed)) { chosenLane = lanes[i]; break; }
    }
    if (chosenLane == -1) {
        // No hay carril seguro ahora, posponer spawn este tick
        return;
    }

    // Imagen aleatoria
    std::string imagePath = "assets/cars/Police car.png";
    if (!carImages.empty()) {
        std::uniform_int_distribution<> imgDist(0, static_cast<int>(carImages.size()) - 1);
        imagePath = carImages[imgDist(rng)];
    }
    cars.push_back(std::make_shared<EnemyCar>(imagePath, -60.0f, chosenLane, candidateSpeed));
}
void Spawner::setCarImages(const std::vector<std::string>& images)
{
    carImages = images;
}

std::vector<std::shared_ptr<EnemyCar>>& Spawner::getCars()
{
    return cars;
}

void Spawner::removeCar(size_t index)
{
    if (index < cars.size())
        cars.erase(cars.begin() + index);
}

void Spawner::increaseDifficulty()
{
    difficulty += 0.1f;
    spawnInterval = std::max(0.5f, spawnInterval - 0.1f);
}

void Spawner::setMap(int mapId)
{
    map = mapId;
}

void Spawner::render(sf::RenderWindow& window)
{
    for (auto& car : cars)
    {
        car->render(window);
    }
}
