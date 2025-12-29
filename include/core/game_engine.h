#pragma once

#include "../graphics/window.h"
#include "player.h"
#include <chrono>

namespace Morpion {
namespace Core {

class Game {
private:
    // 🔒 ÉTAT DU MOTEUR
    Graphics::Window gWindow;
    SDL_Renderer* grenderer;
    Morpion::Core::theme CurrentTheme;
    bool gRunningstatus;
    bool gIsPaused;


public:
    Morpion::Core::Case bord[9];
    int player = 1;
    
public:
    // 🏗 CONSTRUCTEUR
    Game(const std::string& title, float width, float height);
    
    // ⚙️ MÉTHODES PRINCIPALES
    bool Initialize();
    void Run();
    void Shutdown();
    
    // 🎮 GESTION D'ÉVÉNEMENTS
    void HandleEvents();
    void HandleInput(SDL_Event even);

private:
    // MÉTHODES INTERNES
    void Render();
    void RenderT();
    void loadvoid();
    void loadbord();
    void RenderUI();
};
bool SDL_PointInFRect(SDL_Point* p ,SDL_FRect* r );

} // namespace Core
} // namespace Morpion