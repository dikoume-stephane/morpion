#include "graphics/window.h"


namespace Morpion {
namespace Graphics {

// 🏗 CONSTRUCTEUR
Window::Window(const std::string& title, float width, float height)
    : gTitle(title), gWidth(width), gHeight(height), 
      gWindow(nullptr), gRenderer(nullptr), gInitializedstatus(false), gThemeIs(1) {}

// 🗑 DESTRUCTEUR
Window::~Window() {
    Shutdown();
}

// ⚙️ INITIALISATION
bool Window::Initialize() {
    
    gWindow = SDL_CreateWindow(gTitle.c_str(), static_cast<int>(gWidth), static_cast<int>(gHeight), SDL_WINDOW_RESIZABLE);
    if (!gWindow) {
        std::cerr << "❌ Erreur création fenêtre: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, NULL);
    if (!gRenderer) {
        std::cerr << "❌ Erreur création renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        return false;
    }

    loadthemes();

    gInitializedstatus = true;
    std::cout << "✅ Fenêtre initialisée: " << gTitle << " (" << gWidth << "x" << gHeight << ")" << std::endl;
    return true;
}

//detruit tous les outils 
void Window::Shutdown() {
    if (gRenderer) {
        SDL_DestroyRenderer(gRenderer);
        gRenderer = nullptr;
    }
    if (gWindow) {
        SDL_DestroyWindow(gWindow);
        gWindow = nullptr;
    }

    destroythemes(themes);
    SDL_Quit();
    gInitializedstatus = false;
    std::cout << " fermeture de la Fenêtre " << std::endl;
}

// 🎨 NETTOYAGE DE L'ÉCRAN
void Window::Clear(const Core::Color& color) {
    if (gRenderer) {
        SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(gRenderer);
    }
}

// 🔄 AFFICHAGE
void Window::Present() {
    if (gRenderer) {
        SDL_RenderPresent(gRenderer);
    }
}

void Window::loadthemes()
{
    
    themes[0].piont[0] = IMG_LoadTexture(gRenderer , "assets/texture/theme1/vide1.png");
    themes[0].piont[1] = IMG_LoadTexture(gRenderer , "assets/texture/theme1/X1.png");
    themes[0].piont[2] = IMG_LoadTexture(gRenderer , "assets/texture/theme1/O1.png");
    themes[1].piont[0] = IMG_LoadTexture(gRenderer , "assets/texture/theme2/vide2.png");
    themes[1].piont[1] = IMG_LoadTexture(gRenderer , "assets/texture/theme2/X2.png");
    themes[1].piont[2] = IMG_LoadTexture(gRenderer , "assets/texture/theme2/O2.png");
    themes[2].piont[0] = IMG_LoadTexture(gRenderer , "assets/texture/theme3/vide3.png");
    themes[2].piont[1] = IMG_LoadTexture(gRenderer , "assets/texture/theme3/X3.png");
    themes[2].piont[2] = IMG_LoadTexture(gRenderer , "assets/texture/theme3/O3.png");
    
    themes[0].couleur = {0, 0, 0,10};
    themes[1].couleur = {10, 10, 44};
    themes[2].couleur = {144, 238, 144};

    for (int i=0 ; i<3 ; i++)
    {
        for (int j=0 ; j<3 ; j++)
        {
            if(!themes[i].piont[j])
            {
                std::cerr <<"erreur de chargement du theme "<<i<<"texture "<<j<<SDL_GetError()<<std::endl;
            }
        }
    }
    
}

void Window::destroythemes(Morpion::Core::theme* themes)
{
    int i,j;
    for (i=0 ; i<3 ; i++)
    {
        for (j=0 ; j<3 ; j++)
        {
            if (themes[i].piont[j])
            {
                SDL_DestroyTexture(themes[i].piont[j]);
                themes[i].piont[j] = nullptr;
            }
        }
    }
}

Morpion::Core::theme Window::GetCurrentTheme()
{
    return themes[gThemeIs-1];
    
}

} // namespace Graphics
} // namespace Morpion