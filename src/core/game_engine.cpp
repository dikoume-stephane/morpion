#include "core/game_engine.h"
#include <iostream>

namespace Morpion
{
    namespace Core
    {

        // 🏗 CONSTRUCTEUR
        Game::Game(const std::string& title, float width, float height)
            : gWindow(title, width, height), grenderer(gWindow.GetRenderer()), joueur1(nullptr), joueur2(nullptr),
             joueuractuel(nullptr), CurrentTheme(gWindow.GetCurrentTheme()), gRunningstatus(false) {}

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

            gRunningstatus = true;
            std::cout << "✅ Moteur de jeu initialisé" << std::endl;
            return true;
        }

        void Game::loadGrille(float hauteur, float largeur, int taille)
        {
            SDL_FRect cadre;
            int marge = 4;
            
            float dimensionReference = (hauteur < largeur) ? hauteur : largeur;
            
            int taillecase = (dimensionReference - (taille + 1) * marge) / taille;

            // Recalcul des ajustements pour centrer la grille
            int ajustx = (largeur - ((taillecase * taille) + (taille + 1) * marge)) / 2;
            int ajusty = (hauteur - ((taillecase * taille) + (taille + 1) * marge)) / 2;

            // Vérifie si la grille existe déjà avec la bonne taille
            bool modeMiseAJour = (grille.size() == taille * taille);

            for (int i = 0; i < taille; i++)
            {
                for (int j = 0; j < taille; j++)
                {
                    // Calcul de la nouvelle position du rectangle
                    cadre.x = ajustx + marge + j * (taillecase + marge);
                    cadre.y = ajusty + marge + i * (taillecase + marge);
                    cadre.h = taillecase;
                    cadre.w = taillecase;

                    if (modeMiseAJour)
                    {
                        // CAS 1 : Redimensionnement
                        int index = (i * taille) + j;
                        
                        // On met à jour SEULEMENT le cadre visuel
                        grille[index].cadre = cadre;
                        
                    }
                    else
                    {
                        // CAS 2 : Initialisation
                        // La grille est vide, on crée les cases
                        Case c;
                        c.cadre = cadre;
                        c.etat = 0; // Vide par défaut
                        grille.push_back(c);
                    }
                }
            }
        }

        void Game::InitialiserPartie(int mode, int tailleGrille, int nivdif)
        {
            //nettoyage memoire
            if (joueur1) { delete joueur1; joueur1 = nullptr; }
            if (joueur2) { delete joueur2; joueur2 = nullptr; }
            //configuration de la grille
            this->gGrilleTaile = tailleGrille;
            grille.clear();
            loadGrille(gWindow.GetHeight(), gWindow.GetWidth(),gGrilleTaile);

            //creation des joueurs
            joueur1 = new Player(playertype::PLAYER, 1, 0);

            if (mode == 1)
            {
                joueur2 = new Player(playertype::ORDI, 2, 0);
                joueur2->Setordilevel(nivdif);
            }
            else
            {
                joueur2 = new Player(playertype::PLAYER, 2, 0);
            }

            joueuractuel = joueur1;
            Etatactuel = GameState::PLAYING;
            std::cout<<" partie lancée !!!"<<std::endl;

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
                if (joueuractuel != nullptr && checkwin(grille, gGrilleTaile, joueuractuel->getid())) Etatactuel = GameState::GAMEOVER;

                //si c'est le tour de l'IA
                if (Etatactuel == GameState::PLAYING && joueuractuel->gettype() == playertype::ORDI)
                {
                    // peti delai pour que l'ia ne reponde pas instantanement
                    SDL_Delay(500);

                    int coupIA = joueuractuel->choisirCoup(grille);
                    if (coupIA != -1)
                    {
                        grille.at(coupIA).etat = joueuractuel->getid();
                        if (checkwin(grille, gGrilleTaile, joueuractuel->getid()))
                        {
                            Etatactuel = GameState::GAMEOVER;
                        }
                        else
                        {
                            joueuractuel = joueur1;
                        }
                    }
                }
                Render();
                
                // Limitation à ~60 FPS
                SDL_Delay(16);
            }
        }

        // 🧹 FERMETURE
        void Game::Shutdown()
        {
            gRunningstatus = false;
            delete joueur1;
            delete joueur2;
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
            if (joueuractuel == nullptr) return;
            if (Etatactuel == GameState::PLAYING && joueuractuel->gettype() == playertype::PLAYER)
            {
                if (even.button.button == SDL_BUTTON_LEFT)
                {
                    SDL_Point mouse = {static_cast<int>(even.button.x) , static_cast<int>(even.button.y)};

                    for ( i = 0; i < grille.size(); i++)
                    { 
                        if (SDL_PointInFRect(&mouse , &grille.at(i).cadre))
                        {
                            if (grille.at(i).etat == 0 )
                            {
                                grille.at(i).etat = joueuractuel->getid();
                                joueuractuel = (joueuractuel == joueur1) ? joueur2 : joueur1;
                            }
                        }
                    }
                    
                    
                }
            }   
        }

        
        // 🎨 RENDU
        void Game::Render()
        {
            gWindow.Clear(CurrentTheme.couleur);

            if (Etatactuel == GameState::PLAYING)
            {
                loadvoid();
                RenderT();
            }
            // Ici on ajouterait l'interface utilisateur
            RenderUI();
            
            gWindow.Present();
        }

        void Game::RenderT()
        {
            int i;
            //chargement des images en fonction du player
            for (int j = 0 ; j < gGrilleTaile*gGrilleTaile ; j ++)
            {
                if ( grille.at(j).etat == 1)
                {
                    SDL_RenderTexture(grenderer , CurrentTheme.piont[1] , NULL , &grille.at(j).cadre);
                }
            }
            for (i=0 ; i < gGrilleTaile*gGrilleTaile ; i++)
            {
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

        // 📊 INTERFACE UTILISATEUR
        void Game::RenderUI()
        {
            int them = 1;
            // 1. Récupérer la taille actuelle de la fenêtre 
            int w, h;
            SDL_GetWindowSize(gWindow.GetgWindow(), &w, &h);

            int h1 = 0.3*h, w1 = 0.5*w;
            ImGui_ImplSDLRenderer3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            // MENU
            if (Etatactuel == GameState::MENU)
            {
                ImGui::SetNextWindowPos(ImVec2(((w - w1) / 2), ((h - h1) / 2)), ImGuiCond_Always);
                ImGui::SetNextWindowSize(ImVec2(w1, h1), ImGuiCond_Always);

                ImGui::Begin("menu principale", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::SeparatorText("mode de jeu");

                if (ImGui::Selectable("solo VS IA", modeselect == 1)) modeselect = 1;
                if (ImGui::Selectable("Duo (2 players)", modeselect == 2)) modeselect = 2;

                ImGui::SeparatorText("taille de grille");
                if (ImGui::RadioButton("3x3 ", gGrilleTaile == 3)) gGrilleTaile = 3;
                ImGui::SameLine();
                if (ImGui::RadioButton("4x4 ", gGrilleTaile == 4)) gGrilleTaile = 4;
                ImGui::SameLine();
                if (ImGui::RadioButton("5x5 ", gGrilleTaile == 5)) gGrilleTaile = 5;

                if (modeselect == 1)
                {
                    ImGui::SeparatorText("dificulté de l'IA");
                    ImGui::Combo("niveau", &IAlevel, "Facile\0Intermediaire\0Imbattable\0");
                }

                if (ImGui::Button("LANCER LA PARTIE", ImVec2(0.4*w,0.1*h)))
                {
                   InitialiserPartie(modeselect, gGrilleTaile, IAlevel);
                }
                ImGui::End();
            } 
            else 
            {
                // Largeur du panneau : 12% de la largeur de l'écran
                float Lpanel = w * 0.12f; 
                if (Lpanel < 150) Lpanel = 150;

                // Hauteur du panneau : 30% de la hauteur de l'écran
                float Hpanel = h * 0.3f;

                // Position Collé à droite (Largeur écran - Largeur panneau - petite marge)
                float posX = w - Lpanel - 4;
                float posY = 4; // Petite marge en haut

                ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiCond_Always);
                ImGui::SetNextWindowSize(ImVec2(Lpanel, Hpanel), ImGuiCond_Always);
                ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);

                ImGui::Begin("options", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize );

                if (ImGui::CollapsingHeader("Taille de grille"))
                {
                    
                    ImGui::Indent();
                    // Bouton pour 3x3
                    if (ImGui::RadioButton("3x3", gGrilleTaile == 3))
                    {
                        changegrillsize(3, h, w);
                    }
                    // Bouton pour 4x4
                    if (ImGui::RadioButton("4x4", gGrilleTaile == 4))
                    {
                        changegrillsize(4, h, w);
                    }
                    // Bouton pour 5x5
                    if (ImGui::RadioButton("5x5", gGrilleTaile == 5))
                    {
                        changegrillsize(5, h, w);
                    }

                    ImGui::Unindent();
                }

                //ImGui::Spacing();
                //pour les themes
                if(ImGui::CollapsingHeader("Themes"))
                {
                    ImGui::Indent();
                    
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
                    ImGui::Unindent();
                }

                ImGui::End();

            }

            ImGui::Render();
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(),grenderer);
        }

        void Game::IUshutdown()
        {
            ImGui_ImplSDLRenderer3_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();
        }

        void Game::changegrillsize(int nouvTaille, float h, float w)
        {
            if (gGrilleTaile == nouvTaille) return;

            gGrilleTaile = nouvTaille;

            //on nettoie l'encienne grille
            grille.clear();

            //on reinitialise le joueur
            Idplayer = 1;

            //on recharge
            loadGrille(h, w, gGrilleTaile);

        }

        bool Game::analyseSegment(const std::vector<Case>& grille, int depart, int pas, int gGrilleTaile, int Idplayer, std::vector<int>* indiceGagants)
        {
            for (int k = 0; k < gGrilleTaile; k++)
            {
                int idx = depart + (k * pas);

                // SÉCURITÉ : On vérifie si l'index est valide avant d'y accéder
                if (idx < 0 || idx >= (int)grille.size()) {
                    std::cout << "⚠️ ERREUR INDEX : Tentative d'accès à la case " << idx 
                            << " mais la grille ne fait que " << grille.size() << " cases !" << std::endl;
                    return false; 
                }
                if (grille.at(idx).etat != Idplayer) return false;
            }

            if (indiceGagants)
            {
                for (int k = 0; k < gGrilleTaile; k++)
                {
                    indiceGagants->push_back(depart + (k * pas));
                }
            }
            return true;
        }

        bool Game::checkwin(const std::vector<Case>& grille, int gGrilleTaile, int Idplayer, std::vector<int>* indiceGagants)
        {
            //lignes
            for (int r = 0; r < gGrilleTaile; r++)
            {
                if (analyseSegment(grille, r * gGrilleTaile, 1, gGrilleTaile, Idplayer, indiceGagants)) return true;
            }

            //colonnes
            for (int r = 0; r < gGrilleTaile; r++)
            {
                if (analyseSegment(grille, r, gGrilleTaile, gGrilleTaile, Idplayer, indiceGagants)) return true;
            }

            //diagonales
            if (analyseSegment(grille, 0, gGrilleTaile + 1, gGrilleTaile, Idplayer, indiceGagants)) return true;
            if (analyseSegment(grille, gGrilleTaile - 1, gGrilleTaile - 1, gGrilleTaile, Idplayer, indiceGagants)) return true;

            return false;
        }

        bool SDL_PointInFRect(SDL_Point* p ,SDL_FRect* r )
        {
            return ( p->x >= r->x && p->x <= (r->x+r->w) && p->y >= r->y && p->y <= (r->y+r->h));
        }
    } // namespace Core
} // namespace Morpion