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
    themes[1].couleur = {10, 10, 44, 15};
    themes[2].couleur = {144, 238, 144};

    //BGtexture
    BGtexture = IMG_LoadTexture(gRenderer , "assets/texture/backgroud.png");
    
    //chargement des icones par theme

    //theme1
    themes[0].TUI.home = IMG_LoadTexture(gRenderer ,"assets/texture/theme1/marron/homem.png");
    themes[0].TUI.internet  = IMG_LoadTexture(gRenderer ,"assets/texture/theme1/marron/internetm.png");
    themes[0].TUI.music = IMG_LoadTexture(gRenderer ,"assets/texture/theme1/marron/musicm.png");
    themes[0].TUI.music_off = IMG_LoadTexture(gRenderer ,"assets/texture/theme1/marron/music_offm.png");
    themes[0].TUI.no_sound = IMG_LoadTexture(gRenderer , "assets/texture/theme1/marron/volume_offm.png");
    themes[0].TUI.settings = IMG_LoadTexture(gRenderer ,"assets/texture/theme1/marron/settingsm.png");
    themes[0].TUI.sound = IMG_LoadTexture(gRenderer ,"assets/texture/theme1/marron/volumem.png");
    
    //thme2
    themes[1].TUI.home = IMG_LoadTexture(gRenderer ,"assets/texture/theme2/bleu/homeb.png");
    themes[1].TUI.internet  = IMG_LoadTexture(gRenderer ,"assets/texture/theme2/bleu/internet.png");
    themes[1].TUI.music = IMG_LoadTexture(gRenderer ,"assets/texture/theme2/bleu/musicb.png");
    themes[1].TUI.music_off = IMG_LoadTexture(gRenderer ,"assets/texture/theme2/bleu/music_offb.png");
    themes[1].TUI.no_sound = IMG_LoadTexture(gRenderer ,"assets/texture/theme2/bleu/volume_offb.png");
    themes[1].TUI.settings = IMG_LoadTexture(gRenderer ,"assets/texture/theme2/bleu/settingsb.png");
    themes[1].TUI.sound = IMG_LoadTexture(gRenderer ,"assets/texture/theme2/bleu/volumeb.png");

    //theme3
    themes[2].TUI.home = IMG_LoadTexture(gRenderer ,"assets/texture/theme3/green/homeg.png");
    themes[2].TUI.internet  = IMG_LoadTexture(gRenderer ,"assets/texture/theme3/green/internetg.png");
    themes[2].TUI.music = IMG_LoadTexture(gRenderer ,"assets/texture/theme3/green/musicg.png");
    themes[2].TUI.music_off = IMG_LoadTexture(gRenderer ,"assets/texture/theme3/green/music_offg.png");
    themes[2].TUI.no_sound = IMG_LoadTexture(gRenderer ,"assets/texture/theme3/green/volume_offg.png");
    themes[2].TUI.settings = IMG_LoadTexture(gRenderer ,"assets/texture/theme3/green/settingsg.png");
    themes[2].TUI.sound = IMG_LoadTexture(gRenderer ,"assets/texture/theme3/green/volumeg.png");


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

    for (int i = 0; i < 3; i++)
    {
        SDL_DestroyTexture(themes[i].TUI.home);
        themes[i].TUI.home = nullptr;
        SDL_DestroyTexture(themes[i].TUI.internet);
        themes[i].TUI.internet = nullptr;
        SDL_DestroyTexture(themes[i].TUI.music);
        themes[i].TUI.music = nullptr;
        SDL_DestroyTexture(themes[i].TUI.music_off);
        themes[i].TUI.music_off = nullptr;
        SDL_DestroyTexture(themes[i].TUI.no_sound);
        themes[i].TUI.no_sound = nullptr;
        SDL_DestroyTexture(themes[i].TUI.settings);
        themes[i].TUI.settings = nullptr;
        SDL_DestroyTexture(themes[i].TUI.sound);
        themes[i].TUI.sound = nullptr;
    }
    
}

Morpion::Core::theme Window::GetCurrentTheme()
{
    return themes[gThemeIs-1];
    
}

} // namespace Graphics
} // namespace Morpion