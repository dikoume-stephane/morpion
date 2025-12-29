#pragma once 

#include <SDL3/SDL.h> 
#include <string> 
#include "../core/structs.h" 
//#include "libs/SDL3/SDL3_image/SDL_image.h"
#include <SDL3_image/SDL_image.h>

namespace Morpion { 
namespace Graphics { 
class Window { 
private: 
    // outils SDL 
    SDL_Window* gWindow; 
    SDL_Renderer* gRenderer; 
    float gWidth; 
    float gHeight; 
    bool gInitializedstatus; 
    std::string gTitle;
    int gThemeIs;

public: 
    Morpion::Core::theme themes[3];
    // 🏗 CONSTRUCTEUR/DESTRUCTEUR 
    Window(const std::string& title, float width, float height); 
    ~Window(); 
    // ⚙INITIALISATION 
    bool Initialize(); 
    void Shutdown(); 
    void loadthemes();
    void destroythemes(Morpion::Core::theme* themes);
     
    // RENDU 
    void Clear(const Core::Color& color ); 
    void Present(); 
     
    // GETTERS 
    SDL_Renderer* GetRenderer() const { return gRenderer; } 
    bool IsInitialized() const { return gInitializedstatus; } 
    float GetWidth() const { return gWidth; } 
    float GetHeight() const { return gHeight; } 
    std::string GetTitle() const { return gTitle; } 
    Morpion::Core::theme GetCurrentTheme();
}; 
} // namespace Graphics 
} // namespace Morpion