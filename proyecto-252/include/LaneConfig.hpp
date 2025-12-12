#ifndef LANE_CONFIG_HPP
#define LANE_CONFIG_HPP

#include <array>

// Configuración de carriles: simétricos respecto al centro de la ventana
// Centro X (ventana 800px): 400
// Separación entre carriles: 80px
// Posiciones: 400 - 1.5d, 400 - 0.5d, 400 + 0.5d, 400 + 1.5d
inline constexpr float BASE_ROAD_WIDTH = 250.0f;
inline constexpr float TARGET_WINDOW_WIDTH = 800.0f; // Debe coincidir con GameEngine
inline constexpr float SCALE_X = TARGET_WINDOW_WIDTH / BASE_ROAD_WIDTH; // 3.2 con 800/250

inline float getLaneX(int laneIndex)
{
    // laneIndex: 0..3
    // offsets: -1.5, -0.5, +0.5, +1.5
    static constexpr float BASE_LANES[4] = { 65.0f, 105.0f, 145.0f, 183.0f };
    if (laneIndex < 0) laneIndex = 0;
    if (laneIndex > 3) laneIndex = 3;
    return BASE_LANES[laneIndex] * SCALE_X;
}

#endif