// This file is part of the course TPV2@UCM - Samir Genaim

#include <iostream>
#include "game/Game.h"

int main(int, char**) {
    try {
        // Inicializar SDL (SDLUtils + InputHandler)
        if (!Game::Init()) {
            std::cerr << "No se pudo inicializar el juego." << std::endl;
            return 1;
        }

        // Inicializar entidades del juego
        Game::Instance()->initGame();

        // Bucle principal
        Game::Instance()->start();

        // Liberar singleton
        Game::Release();

    }
    catch (const std::string& e) {
        std::cerr << e << std::endl;
    }
    catch (const char* e) {
        std::cerr << e << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Excepcion desconocida." << std::endl;
    }

    return 0;
}