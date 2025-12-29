#include "core/game_engine.h"
#include <iostream>
#include <sstream>

namespace Morpion
{
    namespace Core
    {

        // 🏗 CONSTRUCTEUR
        Game::Game(const std::string& title, float width, float height)
            : gWindow(title, width, height), grenderer(gWindow.GetRenderer()), CurrentTheme(gWindow.GetCurrentTheme()), 
            gRunningstatus(false), 
            gIsPaused(false) {}

        // ⚙️ INITIALISATION
        bool Game::Initialize()
        {
            if (!gWindow.Initialize())
            {
                return false;
            }
            
            gRunningstatus = true;
            std::cout << "✅ Moteur de jeu initialisé" << std::endl;
            return true;
        }

        // 🎮 BOUCLE PRINCIPALE
        void Game::Run()
        {
            std::cout << "🎯 Démarrage de la boucle de jeu..." << std::endl;
            CurrentTheme = gWindow.GetCurrentTheme();
            loadbord();

            while (gRunningstatus)
            {
                HandleEvents();
                Render();
                
                // Limitation à ~60 FPS
                SDL_Delay(16);
            }
        }

        // 🧹 FERMETURE
        void Game::Shutdown()
        {
            gRunningstatus = false;
            std::cout << "arret du Moteur de jeu " << std::endl;
        }

        // 🎮 GESTION DES ÉVÉNEMENTS
        void Game::HandleEvents()
        {
            SDL_Event event;
           while (SDL_PollEvent(&event)) 
            {
                if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) ) 
                {
                    gRunningstatus = false;
                }
                        
                if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
                {
                    HandleInput(event);
                }
                
            }
        }

        // ⌨️ GESTION DES TOUCHES
        void Game::HandleInput(SDL_Event even)
        {
            int i;
            
            if (even.button.button == SDL_BUTTON_LEFT)
            {
                SDL_Point mouse = {static_cast<int>(even.button.x) , static_cast<int>(even.button.y)};

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

        
        // 🎨 RENDU
        void Game::Render()
        {
            gWindow.Clear(CurrentTheme.couleur);
            loadvoid();

            RenderT();
            // Ici on ajouterait l'interface utilisateur
            RenderUI();
            
            gWindow.Present();
        }

        void Game::RenderT()
        {
            int i;
            //chargement des images en fonction du player
            for (i=0 ; i<9 ; i++)
            {
                if ( bord[i].etat == 1)
                {
                    SDL_RenderTexture(grenderer , CurrentTheme.piont[1] , NULL , &bord[i].cadre);
                }

                if ( bord[i].etat == 2)
                {
                    SDL_RenderTexture(grenderer , CurrentTheme.piont[2] , NULL , &bord[i].cadre);
                }
            }
        }

        void Game::loadvoid()
        {
            int i;
            SDL_SetRenderDrawColor(grenderer, 255, 0, 0, 255);
            for (i=0 ; i<9 ; i++)
            {
                SDL_RenderFillRect(grenderer , &bord[i].cadre );
                if (bord[i].etat == 0)
                {
                SDL_RenderTexture(grenderer , CurrentTheme.piont[0] , NULL , &bord[i].cadre);
                }
            }

        }

        void Game::loadbord()
        {
            float x=250.0f , y=101.0f;
            int a = 0, i, j;
            float c =x,b =y;
            const float taille=250.0f;
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
        }

        // 📊 INTERFACE UTILISATEUR
        void Game::RenderUI()
        {
            // Pour l'instant, interface texte dans la console
            // Une vraie interface graphique serait implémentée ici
        }

        bool SDL_PointInFRect(SDL_Point* p ,SDL_FRect* r )
        {
            return ( p->x >= r->x && p->x <= (r->x+r->w) && p->y >= r->y && p->y <= (r->y+r->h));
        }
    } // namespace Core
} // namespace Morpion