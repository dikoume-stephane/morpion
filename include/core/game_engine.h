#pragma once

#include <vector>
#include "../graphics/window.h"
#include "../../libs/imgui/imgui.h"
#include "../../libs/imgui/backends/imgui_impl_sdl3.h"
#include "../../libs/imgui/backends/imgui_impl_sdlrenderer3.h"

namespace Morpion {namespace Core { class Player;}}
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
    std::vector<int> indiceGagants;

public:
    int Idplayer = 1;
    
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

    //LOGIQUE DU JEU 
    static bool checkwin(const std::vector<Case> grille, int gGrilleTaile, int Idplayer, std::vector<int>* indiceGagants = nullptr);
    static bool analyseSegment(const std::vector<Case> grille, int depart, int pas, int gGrilleTaile, int Idplayer, std::vector<int>* indiceGagants);

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