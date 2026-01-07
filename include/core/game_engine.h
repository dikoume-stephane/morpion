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
    int gGrilleTaile =6;
    std::vector<Case> grille;

public:
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
    void RenderUI();
    void IUshutdown();
    void changegrillsize(int nouvTaille, float h, float w);
};
bool SDL_PointInFRect(SDL_Point* p ,SDL_FRect* r );

} // namespace Core
} // namespace Morpion