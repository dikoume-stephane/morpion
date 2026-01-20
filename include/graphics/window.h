#pragma once 

#include <SDL3/SDL.h> 
#include <string> 
#include "../core/structs.h" 
//#include "libs/SDL3/SDL3_image/SDL_image.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>

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

public: 
    int gThemeIs = 1;
    Morpion::Core::theme themes[3];
    SDL_Texture* BGtexture = nullptr;
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

    //setter
    void SetThemeIs(int valeur) { gThemeIs = valeur; }
     
    // GETTERS 
    SDL_Renderer* GetRenderer() const { return gRenderer; }
    SDL_Window* GetgWindow() const { return gWindow; } 
    bool IsInitialized() const { return gInitializedstatus; } 
    float GetWidth() const { return gWidth; } 
    float GetHeight() const { return gHeight; } 
    std::string GetTitle() const { return gTitle; } 
    Morpion::Core::theme GetCurrentTheme();
}; 
} // namespace Graphics 
} // namespace Morpion