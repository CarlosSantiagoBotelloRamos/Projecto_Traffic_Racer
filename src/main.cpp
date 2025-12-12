#include "GameEngine.hpp"
#include "MenuState.hpp"
#include "LoadingState.hpp"
#include <iostream>

int main()
{
    try
    {
        GameEngine engine;
        
        // Mostrar pantalla de Loading por 5 segundos antes del menú
        auto loadingState = std::make_shared<LoadingState>(&engine);
        engine.pushState(loadingState);
        
        // Iniciar el juego
        engine.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
