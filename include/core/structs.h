#pragma once 

#include <cstdint> 
#include <string>
#include <SDL3/SDL.h> 
#include <cmath> 
namespace Morpion { 
namespace Core { 
// 🏷 STRUCTS POUR LES DONNÉES SIMPLES 

struct Color { 
    uint8_t r; 
    uint8_t g; 
    uint8_t b; 
    uint8_t a; 
    // Constructeurs multiples 
    Color() : r(255), g(255), b(255), a(255) {}  // Blanc par défaut 
    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)  
        : r(red), g(green), b(blue), a(alpha) {} 
    // Couleurs prédéfinies 
    static Color Red() { return Color(255, 0, 0); } 
    static Color Green() { return Color(0, 255, 0); } 
    static Color Blue() { return Color(0, 0, 255); } 
}; 

struct Case 
{
    SDL_FRect cadre;
    int etat = 0;
    //constructeur
    Case(float x=0, float y=0, float w=1, float h=1, int s=0) : cadre{x,y,w,h}, etat(s) {};
};

struct textureUI
{
    SDL_Texture* home;
    SDL_Texture* settings;
    SDL_Texture* music;
    SDL_Texture* sound;
    SDL_Texture* no_sound;
    SDL_Texture* music_off;
    SDL_Texture* internet;

    textureUI() {home = settings = music = sound = no_sound = music_off = internet = nullptr;}
};

struct textureUIG
{
    SDL_Texture* menu;
    SDL_Texture* lightM;
    SDL_Texture* blackM;
    SDL_Texture* quit;
    SDL_Texture* replay;
    SDL_Texture* cancel;
};

struct theme
{
    SDL_Texture* piont[3];
    SDL_Texture* player1;
    SDL_Texture* player2;
    textureUI TUI;
    Color couleur;
    theme() { player1 = player2 = piont[0] = piont[1] = piont[2] = nullptr; couleur;}
};

} // namespace Core 
} // namespace Morpion