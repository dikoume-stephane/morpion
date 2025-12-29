#include <iostream>
#include <cstdlib>
#include <SDL3/SDL.h>
#include "SDL_image.h"

#ifdef main
#undef main
#define main SDL_MainThreadCallback
#endif

struct Case 
{
    SDL_FRect cadre;
    int etat = 0;
};

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

struct theme
{
    SDL_Texture* piont[3];
    Color couleur;
    theme() {piont[0]=piont[1]=piont[2]=nullptr; couleur;}
};

void loadthemes(theme themes[3], SDL_Renderer* gRenderer)
{
    themes[0].piont[0] = IMG_LoadTexture(gRenderer , "assets/texture/theme1/vide1.png");
    themes[0].piont[1] = IMG_LoadTexture(gRenderer , "assets/texture/theme1/X1.png");
    themes[0].piont[2] = IMG_LoadTexture(gRenderer , "assets/texture/theme1/O1.png");
    themes[1].piont[0] = IMG_LoadTexture(gRenderer , "assets/texture/theme1/vide2.png");
    themes[1].piont[1] = IMG_LoadTexture(gRenderer , "assets/texture/theme1/X2.png");
    themes[1].piont[2] = IMG_LoadTexture(gRenderer , "assets/texture/theme1/O2.png");
    themes[2].piont[0] = IMG_LoadTexture(gRenderer , "assets/texture/theme1/vide3.png");
    themes[2].piont[1] = IMG_LoadTexture(gRenderer , "assets/texture/theme1/X3.png");
    themes[2].piont[2] = IMG_LoadTexture(gRenderer , "assets/texture/theme1/O3.png");
    
    themes[0].couleur = {205, 133, 63};
    themes[1].couleur = {10, 10, 44};
    themes[2].couleur = {144, 238, 144};

    if(!themes[0].piont)
    {
        std::cerr <<"erreur de chargement du theme1 "<<SDL_GetError()<<std::endl;
    }

    if(!themes[1].piont)
    {
        std::cerr <<"erreur de chargement du theme1 "<<SDL_GetError()<<std::endl;
    }

    if(!themes[2].piont)
    {
        std::cerr <<"erreur de chargement du theme1 "<<SDL_GetError()<<std::endl;
    }
}

void destroythemes(theme* themes)
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

bool SDL_PointInFRect(SDL_Point* p ,SDL_FRect* r )
{
    return ( p->x >= r->x && p->x <= (r->x+r->w) && p->y >= r->y && p->y <= (r->y+r->h));
}
int main() {
    // 1. Initialiser SDL pour la vidéo
    SDL_Init(SDL_INIT_VIDEO);

    // 2. Créer une fenêtre de 1280x960 pixels
    SDL_Window* fenetre = SDL_CreateWindow("fenetre interactive", 1280, 960,SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if(!fenetre)
    {
        std::cout<<"erreur avec la creation de la fenetre SDL_CeateWindow:"<<SDL_GetError()<<std::endl;
        SDL_Quit();

        return 1;
    }

    // 3. Créer un moteur de rendu pour dessiner dans la fenêtre
    SDL_Renderer* renderer = SDL_CreateRenderer(fenetre, NULL);

    if(!renderer)
    {
        std::cout<<"erreur avec la creation du moteur de rendu SDL_CreateRenderer:"<<SDL_GetError()<<std::endl;
        SDL_DestroyWindow(fenetre);
        SDL_Quit();

        return 1;
    }

    theme themes[3];
    loadthemes(themes ,renderer);
    SDL_Texture* texture[2] = { IMG_LoadTexture(renderer , "assets/texture/theme2/X2.png"),IMG_LoadTexture(renderer , "assets/texture/theme2/O2.png")};
    if(!texture[0])
    {
        std::cout <<"erreur de chargement de l'image1:"<<SDL_GetError()<<std::endl;
    }

    if(!texture[1])
    {
        std::cout <<"erreur de chargement de l'image1:"<<SDL_GetError()<<std::endl;
    }
    // 4. Boucle principale
    bool running = true;
    SDL_Event event;
    float x=250.0f , y=101.0f;
    int player = 1, a = 0, i, j;
    float c =x,b =y;
    const float taille=250.0f;
    Case bord[9];
    theme CurrentTheme = themes[0];

    //remplir le tableau de rectangle avec des coordonées variables 
    for(i=0 ;i<3 ;i++)
    {
        for(j=0 ;j<3 ;j++)
        {
            bord[a].cadre={c,b,taille,taille};
            c= c+taille+7;
            a++;
        }
        c= x;
        b= b+taille+7;
         
    }

    while (running) 
    {
        // 5. Gérer les événements (fermeture de fenêtre)
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) ) 
            {
                running = false;
            }
            
            // pou gerer les clique sur le plateau 
            if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    SDL_Point mouse = {static_cast<int>(event.button.x) , static_cast<int>(event.button.y)};

                    for ( i = 0; i < 9; i++)
                    {
                        if (SDL_PointInFRect(&mouse , &bord[i].cadre))
                        {
                            if (bord[i].etat == 0 && player == 1)
                            {
                                bord[i].etat = 1;
                                player = 2;
                            }

                            if (bord[i].etat == 0 && player == 2)
                            {
                                bord[i].etat = 2;
                                player = 1;
                            }
                            
                        }
                    }
                    
                    
                }
                
            }
        }    
        
        // 6. Définir la couleur de fond (gris clair ici)
        SDL_SetRenderDrawColor(renderer, CurrentTheme.couleur.r, CurrentTheme.couleur.g, CurrentTheme.couleur.b, CurrentTheme.couleur.a);

        // 7. Effacer l’écran avec la couleur choisie
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        //dessiner les rectangles sur le renderer
        for (i=0 ; i<9 ; i++)
        {
                if ( bord[i].etat == 1)
                {
                    SDL_RenderTexture(renderer , CurrentTheme.piont[1] , NULL , &bord[i].cadre);
                }

                if ( bord[i].etat == 2)
                {
                    SDL_RenderTexture(renderer , CurrentTheme.piont[2] , NULL , &bord[i].cadre);
                }
            
        }

            
        for (i=0 ; i<9 ; i++)
        {
            SDL_RenderFillRect(renderer , &bord[i].cadre );
            SDL_RenderTexture(renderer , CurrentTheme.piont[0] , NULL , &bord[i].cadre);
            bord[i].etat = 0;
        }

        //chargement des images en fonction du player
        for (i=0 ; i<9 ; i++)
        {
            if ( bord[i].etat == 1)
            {
                SDL_RenderTexture(renderer , texture[0] , NULL , &bord[i].cadre);
            }

            if ( bord[i].etat == 2)
            {
                SDL_RenderTexture(renderer , texture[1] , NULL , &bord[i].cadre);
            }
        }
        
        // 8. Afficher le rendu à l’écran
        SDL_RenderPresent(renderer);
    }

    // 9. Nettoyer les ressources
    for (i=0 ; i<2 ; i++)
    {
        SDL_DestroyTexture(texture[i]);
    }
    destroythemes(themes);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(fenetre);
    SDL_Quit();

    return 0;
}
