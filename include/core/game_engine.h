#pragma once

#include <vector>
#include "player.h"
#include "../graphics/window.h"
#include "../../libs/imgui/imgui.h"
#include "../../libs/imgui/backends/imgui_impl_sdl3.h"
#include "../../libs/imgui/backends/imgui_impl_sdlrenderer3.h"

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
    int gGrilleTaile =5;
    std::vector<Case> grille;

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
    void loadGrille(float hauteur, float largeur, int taille);
    
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
    void IUshutdown();
};
bool SDL_PointInFRect(SDL_Point* p ,SDL_FRect* r );

} // namespace Core
} // namespace Morpion