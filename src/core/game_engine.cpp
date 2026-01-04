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
            grenderer = gWindow.GetRenderer();

            //initialisation des composantes imgui
            ImGui::CreateContext();
            ImGui_ImplSDL3_InitForSDLRenderer(gWindow.GetgWindow(), grenderer);
            ImGui_ImplSDLRenderer3_Init(grenderer);

            //initialisation de la grille
            loadGrille(gWindow.GetHeight(), gWindow.GetWidth(),gGrilleTaile);
            gRunningstatus = true;
            std::cout << "✅ Moteur de jeu initialisé" << std::endl;
            return true;
        }

        void Game::loadGrille(float hauteur, float largeur, int taille)
        {
            SDL_FRect cadre;
            Case c; 
            int marge = 4;
            int taillecase = (hauteur -(taille+1)*marge)/taille;

            int ajustx = (largeur -((taillecase*taille)+(taille+1)*marge))/2;
            int ajusty = (hauteur -((taillecase*taille)+(taille+1)*marge))/2;
            for (int i = 0; i < taille; i++)
            {
                for (int j = 0 ; j < taille; j++)
                {
                    cadre.x = ajustx+marge+j*(taillecase+marge);
                    cadre.y = ajusty+marge+i*(taillecase+marge);
                    cadre.h = taillecase;
                    cadre.w = taillecase;

                    c.cadre = cadre;
                    c.etat =0;
                    grille.push_back(c);
                }
            }
            std::cout <<"bon"<<std::endl;
        }

        // 🎮 BOUCLE PRINCIPALE
        void Game::Run()
        {
            std::cout << "🎯 Démarrage de la boucle de jeu..." << std::endl;
            CurrentTheme = gWindow.GetCurrentTheme();
            
            while (gRunningstatus)
            {
                CurrentTheme = gWindow.GetCurrentTheme();
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
            IUshutdown();
        }

        // 🎮 GESTION DES ÉVÉNEMENTS
        void Game::HandleEvents()
        {
            SDL_Event event;
           while (SDL_PollEvent(&event)) 
            {
                ImGui_ImplSDL3_ProcessEvent(&event);
                if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) ) 
                {
                    gRunningstatus = false;
                }
                  
                if (event.type == SDL_EVENT_WINDOW_RESIZED)
                {
                    int h,w;
                    SDL_GetWindowSize(gWindow.GetgWindow(), &w, &h);
                    loadGrille(h, w,gGrilleTaile);
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

                for ( i = 0; i < gGrilleTaile*gGrilleTaile; i++)
                {
                    if (SDL_PointInFRect(&mouse , &grille.at(i).cadre))
                    {
                        if (grille.at(i).etat == 0 )
                        {
                            grille.at(i).etat = player;
                            player = (player == 1) ? 2 : 1;
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
            for (i=0 ; i < gGrilleTaile*gGrilleTaile ; i++)
            {
                if ( grille.at(i).etat == 1)
                {
                    SDL_RenderTexture(grenderer , CurrentTheme.piont[1] , NULL , &grille.at(i).cadre);
                }

                if ( grille.at(i).etat == 2)
                {
                    SDL_RenderTexture(grenderer , CurrentTheme.piont[2] , NULL , &grille.at(i).cadre);
                }
            }
        }

        void Game::loadvoid()
        {
            int i;
            for (i=0 ; i < gGrilleTaile*gGrilleTaile ; i++)
            {
                SDL_RenderFillRect(grenderer , &grille.at(i).cadre );
                if (grille.at(i).etat == 0)
                {
                SDL_RenderTexture(grenderer , CurrentTheme.piont[0] , NULL , &grille.at(i).cadre);
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
            int them =1;

            ImGui_ImplSDLRenderer3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowPos(ImVec2(1180, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(100, 110), ImGuiCond_Always);
            // Votre UI identique
            ImGui::Begin("options", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
            ImGui::Text("--------");
            
            if (ImGui::Selectable("classic", them == 1))
            {
                them = 1;
                gWindow.SetThemeIs(them);
            }
            if (ImGui::Selectable("galaxi", them == 2))
            {
                them = 2;
                gWindow.SetThemeIs(them);
            }
            if (ImGui::Selectable("champ", them == 3))
            {
                them = 3;
                gWindow.SetThemeIs(them);
            }
            

            ImGui::End();

            ImGui::Render();
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(),grenderer);
        }

        void Game::IUshutdown()
        {
            ImGui_ImplSDLRenderer3_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();
        }

        bool SDL_PointInFRect(SDL_Point* p ,SDL_FRect* r )
        {
            return ( p->x >= r->x && p->x <= (r->x+r->w) && p->y >= r->y && p->y <= (r->y+r->h));
        }
    } // namespace Core
} // namespace Morpion