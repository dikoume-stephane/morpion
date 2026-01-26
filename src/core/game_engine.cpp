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
            ImGuiIO& io = ImGui::GetIO();
            io.Fonts->AddFontFromFileTTF("assets/polices/Sekuya-Regular.ttf", 17.0f);
            ImGui_ImplSDL3_InitForSDLRenderer(gWindow.GetgWindow(), grenderer);
            ImGui_ImplSDLRenderer3_Init(grenderer);

            ImGuiStyle& style = ImGui::GetStyle();

            style.WindowRounding = 9.0f; // Arrondi de la fenêtre
            style.FrameRounding = 8.0f;  // Arrondi des boutons et cases de saisie
            style.GrabRounding = 8.0f;   // Arrondi des curseurs
            style.PopupRounding = 10.0f; // Arrondi des menus surgissants
            
            // enlever les bordures brusques
            //style.WindowBorderSize = 0.0f;
            //style.FrameBorderSize = 1.0f;
            style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
            style.SelectableTextAlign = ImVec2(0.5f, 0.5f);

            //initialisation des texture ppour les icones 
            loadicones();
            gRunningstatus = true;
            std::cout << "✅ Moteur de jeu initialisé" << std::endl;
            return true;
        }

        void Game::loadicones()
        {
            icones.menu = IMG_LoadTexture(grenderer,"assets/texture/general/menu.png");
            icones.blackM = IMG_LoadTexture(grenderer,"assets/texture/general/dark_mode.png");
            icones.cancel = IMG_LoadTexture(grenderer,"assets/texture/general/cancel.png");
            icones.lightM = IMG_LoadTexture(grenderer,"assets/texture/general/light.png");
            icones.quit = IMG_LoadTexture(grenderer,"assets/texture/general/quit.png");
            icones.replay = IMG_LoadTexture(grenderer,"assets/texture/general/replay.png");
           
        }
        
        void Game::destroyicones()
        {
            SDL_DestroyTexture(icones.menu);
            icones.menu = nullptr;
            SDL_DestroyTexture(icones.blackM);
            icones.blackM = nullptr;
            SDL_DestroyTexture(icones.cancel);
            icones.cancel = nullptr;
            SDL_DestroyTexture(icones.lightM);
            icones.lightM = nullptr;
            SDL_DestroyTexture(icones.quit);
            icones.quit = nullptr;
            SDL_DestroyTexture(icones.replay);
            icones.replay = nullptr;
        }


        void Game::loadGrille(float hauteur, float largeur, int taille)
        {
            SDL_FRect cadre;
            int marge = 10;
            
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
            joueur1 = new Player(playertype::PLAYER, 1, 0, Color::Red());

            if (mode == 1)
            {
                joueur2 = new Player(playertype::ORDI, 2, 0, Color::Blue());
                joueur2->Setordilevel(nivdif);
            }
            else
            {
                joueur2 = new Player(playertype::PLAYER, 2, 0, Color::Blue());
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

                //si c'est le tour de l'IA
                if (Etatactuel == GameState::PLAYING && joueuractuel->gettype() == playertype::ORDI)
                {
                    if (!winner)
                    {
                        // peti delai pour que l'ia ne reponde pas instantanement
                        Uint32 currentTime = SDL_GetTicks();
                        if (currentTime - timeOfLastMove >= 700)
                        {
                            int coupIA = joueuractuel->choisirCoup(grille, nbAlignerPourGagner);
                            if (coupIA != -1)
                            {
                                grille.at(coupIA).etat = joueuractuel->getid();
                                if (checkwin(grille, gGrilleTaile, joueuractuel->getid(), &indiceGagants, &winner, nbAlignerPourGagner))
                                {
                                iawin = true;
                                }
                                else
                                {
                                    joueuractuel = joueur1;
                                }
                            }
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
            destroyicones();
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
                if (!winner)
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
                                    timeOfLastMove = SDL_GetTicks(); // Enregistre le moment du clic
                                    if (checkwin(grille, gGrilleTaile, joueuractuel->getid(), &indiceGagants, &winner, nbAlignerPourGagner)) break;
                                    waitingForIA = (joueur2->gettype() == playertype::ORDI);
                                    joueuractuel = (joueuractuel == joueur1) ? joueur2 : joueur1;
                                }
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
                if ((joueuractuel != nullptr && checkwin(grille, gGrilleTaile, joueuractuel->getid(), &indiceGagants, &winner, nbAlignerPourGagner)) || iawin)
                {
                    bordurecasegagnantes(grenderer, indiceGagants, joueuractuel->getwincolor(), grille);
                    if (momentVictoire == 0) momentVictoire = SDL_GetTicks();
                    // 1. On attend un petit délai (ex: 2s) pour laisser l'animation de victoire respirer
                    if ((SDL_GetTicks() - momentVictoire) >= 3000)
                    {
                        Etatactuel = GameState::GAMEOVER;
                    }
                    
                }

                //pour le match null
                if (matchnull() && !winner)
                {
                    Color colo = {169, 106, 255};
                    remplirindices();
                    bordurecasegagnantes(grenderer, indice, colo, grille);
                    if (momentmatchnull == 0) momentmatchnull = SDL_GetTicks();
                    if (SDL_GetTicks() - momentmatchnull >= 2500)
                    {
                        Etatactuel = GameState::GAMEOVER;
                    }
                }
                loadvoid();
                RenderT();
            }
            // Ici on ajouterait l'interface utilisateur
            if (Etatactuel == GameState::MENU)
            {
                SDL_RenderTexture(grenderer, gWindow.BGtexture, NULL, NULL);
            }
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

        void Game::bordurecasegagnantes(SDL_Renderer* renderer, std::vector<int>& indiceGagants, Color couleurjoueur, std::vector<Case>& grille)
        {
            // FORCE LE MODE ICI (car ImGui peut le changer)
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            // 1. Gestion du temps pour l'effet visuel 🌊
            // On utilise un sinus pour faire varier l'opacité entre 50% et 100% de la valeur initiale
            float temps = SDL_GetTicks() * 0.0041f; 
            float oscillation = (sinf(temps) + 1.0f) / 2.0f; // Donne une valeur entre 0.0 et 1.0
            
            // On applique l'oscillation au canal Alpha
            Uint8 alphaAnime = (Uint8)(couleurjoueur.a * (0.3f + 0.8f * oscillation));
            SDL_SetRenderDrawColor(renderer, couleurjoueur.r, couleurjoueur.g, couleurjoueur.b, alphaAnime);

            // 2. Dessin des bordures pour chaque case gagnante
            for (int idx : indiceGagants) {
                // Sécurité : on vérifie que l'indice est bien dans la grille
                if (idx >= 0 && idx < (int)grille.size()) {
                    const SDL_FRect& r = grille[idx].cadre;

                    // 3. Calcul de l'épaisseur proportionnelle (10% de la largeur) 📐
                    float epaisseur = r.w * 0.02f;

                    // 4. Création du rectangle de bordure (Option B : plus large que la case)
                    SDL_FRect rectBordure;
                    rectBordure.x = r.x - epaisseur;
                    rectBordure.y = r.y - epaisseur;
                    rectBordure.w = r.w + (epaisseur * 2.0f);
                    rectBordure.h = r.h + (epaisseur * 2.0f);

                    // 5. Rendu du rectangle plein
                    SDL_RenderFillRect(renderer, &rectBordure);
                }
            }
        }

        // 📊 INTERFACE UTILISATEUR
        void Game::RenderUI()
        {   
            int w, h;
            SDL_GetWindowSize(gWindow.GetgWindow(), &w, &h);
            ImGui_ImplSDLRenderer3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            // 1. MasterCanvas Invisible
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2((float)w, (float)h));
            ImGui::Begin("Canvas", nullptr, 
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
                ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
        
            // 2. Avatars avec Bordures
            if (Etatactuel != GameState::MENU)
            {
                float size = (h > w) ? h : w;
                float panelz = size * 0.1f;
                float posY = (h - panelz) / 2.0f; 
                float margin = 20.0f;
        
                // On définit l'épaisseur de la bordure pour ces deux fenêtres
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Bordure blanche
                ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Fond Rouge
        
                // FLAGS : NoTitleBar enlève les boutons (fermer/réduire) et le nom
                // NoResize et NoMove empêchent l'utilisateur de les déformer
                ImGuiWindowFlags avatarFlags = ImGuiWindowFlags_NoTitleBar | 
                                              ImGuiWindowFlags_NoResize | 
                                              ImGuiWindowFlags_NoMove | 
                                              ImGuiWindowFlags_NoCollapse;
        
                // Fenêtre Joueur 1
                ImGui::SetNextWindowPos(ImVec2(margin, posY));
                ImGui::SetNextWindowSize(ImVec2(panelz, panelz));
                ImGui::Begin("Avatar_J1", nullptr, avatarFlags);
                    CenteredText("JOUEUR 1");
                ImGui::End();
        
                // Fenêtre Joueur 2
                ImGui::SetNextWindowPos(ImVec2(w - panelz - margin, posY));
                ImGui::SetNextWindowSize(ImVec2(panelz, panelz));
                ImGui::Begin("Avatar_J2", nullptr, avatarFlags);
                    CenteredText(modeselect == 1 ? "IA" : "JOUEUR 2");
                ImGui::End();
        
                // On nettoie les styles pour ne pas affecter les autres fenêtres
                ImGui::PopStyleColor(2);
                ImGui::PopStyleVar();
            }
        
            ImGui::End(); // Fin Canvas
            int them = 1;

            int h1 = 0.4*h, w1 = 0.5*w;

            // MENU
            if (Etatactuel == GameState::MENU)
            {
                ImGui::SetNextWindowPos(ImVec2(((w - w1) / 2), ((h - h1) / 2)), ImGuiCond_Always);
                ImGui::SetNextWindowSize(ImVec2(w1, h1), ImGuiCond_Always);
                ImGui::SetNextWindowBgAlpha(0.2f);
                ImGui::Begin("menu principale", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
                CenteredText("Morpion Pro");

                ImGui::NewLine();
                CenteredText("mode de jeu");

                if (ImGui::Selectable("solo VS IA", modeselect == 1)) modeselect = 1;
                if (ImGui::Selectable("Duo (2 players)", modeselect == 2)) modeselect = 2;
                ImGui::Spacing();
                ImGui::Spacing();

                CenteredText("taille de grille");
                float windW = ImGui::GetWindowSize().x;
                ImGui::SetCursorPosX((windW - windW*0.4) * 0.5f);
                if (ImGui::RadioButton("3x3 ", gGrilleTaile == 3)) gGrilleTaile = 3;
                ImGui::SameLine();
                if (ImGui::RadioButton("4x4 ", gGrilleTaile == 4)) gGrilleTaile = 4;
                ImGui::SameLine();
                if (ImGui::RadioButton("5x5 ", gGrilleTaile == 5)) gGrilleTaile = 5;
                ImGui::Spacing();
            
                CenteredText("Type de victoire");
                ImGui::SetCursorPosX((windW - windW*0.4) * 0.5f);
                if (gGrilleTaile == 4) {
                    ImGui::RadioButton("3 alignés", &nbAlignerPourGagner, 3); ImGui::SameLine();
                    ImGui::RadioButton("4 alignés", &nbAlignerPourGagner, 4);
                } else if (gGrilleTaile == 5) {
                    ImGui::RadioButton("4 alignés", &nbAlignerPourGagner, 4); ImGui::SameLine();
                    ImGui::RadioButton("5 alignés", &nbAlignerPourGagner, 5);
                } else {
                    nbAlignerPourGagner = 3; // Forcé pour 3x3
                    ImGui::Text("3 alignements requis");
                }
                ImGui::Spacing();
                ImGui::Spacing();

                
                if (modeselect == 1)
                {
                    CenteredText("dificulté de l'IA");
                    float windowW = ImGui::GetWindowSize().x;
                    ImGui::SetCursorPosX((windowW - windowW*0.4) * 0.5f);
                    ImGui::Combo("niveau", &IAlevel, "Facile\0Intermediaire\0Imbattable\0");
                    ImGui::Spacing();
                }

                ImGui::NewLine();
                float windowWidth = ImGui::GetWindowSize().x;
                ImGui::SetCursorPosX((windowWidth - 0.4*w) * 0.5f);

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.60f, 0.85f, 0.10f, 1.00f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 1.00f, 0.20f, 1.00f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.50f, 0.75f, 0.00f, 1.00f));
                if (ImGui::Button("LANCER LA PARTIE", ImVec2(0.4*w,0.1*h)))
                {
                   InitialiserPartie(modeselect, gGrilleTaile, IAlevel);
                }
                ImGui::PopStyleColor(3);

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

            //message de fin de partie
            if (Etatactuel == GameState::GAMEOVER)
            {
                AfficherFinDePartie();
            }

            ImGui::Render();
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(),grenderer);
        }

        void Game::CenteredText(const char* text)
        {
            float windowWidth = ImGui::GetWindowSize().x;
            float textWidth = ImGui::CalcTextSize(text).x;
            
            // On déplace le curseur horizontalement
            ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
            ImGui::Text(text);
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

        void Game::AfficherFinDePartie()
        {
            // 2. Configuration du Popup
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(350, 250)); // Un peu plus large pour les icônes
            
            // On force l'ouverture du popup une seule fois au passage en GAMEOVER
            if (Etatactuel == GameState::GAMEOVER && !ImGui::IsPopupOpen("Resultat")) {
                ImGui::OpenPopup("Resultat");
            }

            if (ImGui::BeginPopupModal("Resultat", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) 
            {
                // --- TEXTE DU GAGNANT ---
                ImGui::Spacing();
                std::string msg;
                
                if (joueuractuel->getid() == 1 && !matchnull())
                {
                    msg = "JOUEUR 1 A GAGNE !";
                }else if (joueuractuel->getid() == 2 && !matchnull())
                {
                    msg = "JOUEUR 2 A GAGNE !";
                }else
                {
                    if (iawin)
                    {
                        msg = "L'ORDINATEUR A GAGNE !";
                    }else{
                        msg = "MATCH NULL !!";
                    }
                }

                CenteredText(msg.c_str());
                ImGui::Separator();
                ImGui::Spacing();

                // --- BOUTON RECOMMENCER (BLEU) ---
                // On change temporairement la couleur du bouton
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.7f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
                
                // ImageButton : ID unique, Texture, Taille, UVs, Bordure, Fond, Teinte
                if (ImGui::ImageButton("##retry", (ImTextureID)icones.replay, ImVec2(32, 32))) {
                    ResetPartie();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine(); 
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8); // Aligner le texte avec l'icône
                ImGui::Text("RECOMMENCER");
                
                ImGui::PopStyleColor(2);

                ImGui::Spacing();

                // --- BOUTON MENU PRINCIPAL (ROUGE) ---
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));

                if (ImGui::ImageButton("##menu", (ImTextureID)icones.menu, ImVec2(32, 32))) {
                    Etatactuel = GameState::MENU;
                    ResetPartie();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
                ImGui::Text("MENU PRINCIPAL");

                ImGui::PopStyleColor(2);

                ImGui::EndPopup();
            }
        }

        void Game::ResetPartie()
        {
            // 1. Vider les indices gagnants pour arrêter le dessin des bordures
            indiceGagants.clear(); 
            indice.clear();
            
            // 2. Remettre l'IA à zéro et winner à false
            iawin = false;
            winner = false;

            //on annule la victoire
            momentVictoire = 0;
            momentmatchnull = 0;
            
            // 3. Réinitialiser la grille
            for(auto& c : grille) {
                c.etat = 0;
            }

            // 4. Repasser en mode jeu (si on n'est pas retourné au menu)
            if (Etatactuel != GameState::MENU) {
                Etatactuel = GameState::PLAYING;
                joueuractuel = joueur1; // Le joueur 1 recommence
            }
        }
        bool Game::matchnull()
        {
            int occupe = 0;
            for (auto c : grille)
            {
                if ( c.etat != 0) occupe ++;
            }
            return occupe == grille.size();
        }

        void Game::remplirindices()
        {
            int c = 0;
            for (auto d : grille)
            {
                indice.push_back(c);
                c ++;
            }
        }

        bool Game::analyseSegment(const std::vector<Case>& grille, int depart, int pas, int gGrilleTaile, int Idplayer, std::vector<int>* indiceGagants, bool* winner, int nbRequis)
        {
            std::vector<int> suiteActuelle;

            for (int k = 0; k < gGrilleTaile; k++)
            {
                int idx = depart + (k * pas);

                // Sécurité index
                if (idx < 0 || idx >= (int)grille.size()) continue;

                if (grille.at(idx).etat == Idplayer)
                {
                    suiteActuelle.push_back(idx);
                    
                    // Si on a atteint le nombre requis de pions consécutifs
                    if ((int)suiteActuelle.size() == nbRequis)
                    {
                        if (indiceGagants)
                        {
                            indiceGagants->clear(); // On vide pour n'avoir que les indices gagnants
                            *indiceGagants = suiteActuelle;
                        }
                        if (winner) *winner = true;
                        return true;
                    }
                }
                else
                {
                    // On casse la suite si on trouve une case vide ou à l'adversaire
                    suiteActuelle.clear();
                }
            }
            return false;
        }

        bool Game::checkwin(const std::vector<Case>& grille, int gGrilleTaile, int Idplayer, std::vector<int>* indiceGagants, bool* winner, int nbAlignerPourGagner)
        {
            int n = nbAlignerPourGagner; // 3, 4 ou 5 selon ton choix au menu

            // --- 1. LIGNES ---
            for (int r = 0; r < gGrilleTaile; r++)
                if (analyseSegment(grille, r * gGrilleTaile, 1, gGrilleTaile, Idplayer, indiceGagants, winner, n)) return true;

            // --- 2. COLONNES ---
            for (int c = 0; c < gGrilleTaile; c++)
                if (analyseSegment(grille, c, gGrilleTaile, gGrilleTaile, Idplayer, indiceGagants, winner, n)) return true;

            // --- 3. DIAGONALES DESCENDANTES (\) ---
            // On vérifie les diagonales partant de la première ligne
            for (int c = 0; c <= gGrilleTaile - n; c++)
                if (analyseSegment(grille, c, gGrilleTaile + 1, gGrilleTaile - c, Idplayer, indiceGagants, winner, n)) return true;
            
            // On vérifie les diagonales partant de la première colonne (en sautant le coin 0,0 déjà fait)
            for (int r = 1; r <= gGrilleTaile - n; r++)
                if (analyseSegment(grille, r * gGrilleTaile, gGrilleTaile + 1, gGrilleTaile - r, Idplayer, indiceGagants, winner, n)) return true;

            // --- 4. DIAGONALES ASCENDANTES (/) ---
            // On vérifie les diagonales partant de la première ligne (côté droit)
            for (int c = n - 1; c < gGrilleTaile; c++)
                if (analyseSegment(grille, c, gGrilleTaile - 1, c + 1, Idplayer, indiceGagants, winner, n)) return true;

            // On vérifie les diagonales partant de la dernière colonne (en sautant le coin déjà fait)
            for (int r = 1; r <= gGrilleTaile - n; r++)
                if (analyseSegment(grille, (r * gGrilleTaile) + (gGrilleTaile - 1), gGrilleTaile - 1, gGrilleTaile - r, Idplayer, indiceGagants, winner, n)) return true;

            return false;
        }

        bool SDL_PointInFRect(SDL_Point* p ,SDL_FRect* r )
        {
            return ( p->x >= r->x && p->x <= (r->x+r->w) && p->y >= r->y && p->y <= (r->y+r->h));
        }

        //style de l'UI
        int Game::gThemeV3(int hue07, int alt07, int nav07, int lit01, int compact01, int border01, int shape0123)
        {
            bool rounded = shape0123 == 2;

            // V3 style from ImThemes
            ImGuiStyle& style = ImGui::GetStyle();
            
            const float _8 = compact01 ? 4 : 8;
            const float _4 = compact01 ? 2 : 4;
            const float _2 = compact01 ? 0.5 : 1;

            style.Alpha = 1.0f;
            style.DisabledAlpha = 0.3f;

            style.WindowPadding = ImVec2(4, _8);
            style.FramePadding = ImVec2(4, _4);
            style.ItemSpacing = ImVec2(_8, _2 + _2);
            style.ItemInnerSpacing = ImVec2(4, 4);
            style.IndentSpacing = 16;
            style.ScrollbarSize = compact01 ? 12 : 18;
            style.GrabMinSize = compact01 ? 16 : 20;

            style.WindowBorderSize = border01;
            style.ChildBorderSize = border01;
            style.PopupBorderSize = border01;
            style.FrameBorderSize = 0;

            style.WindowRounding = 4;
            style.ChildRounding = 6;
            style.FrameRounding = shape0123 == 0 ? 0 : shape0123 == 1 ? 4 : 12;
            style.PopupRounding = 4;
            style.ScrollbarRounding = rounded * 8 + 4;
            style.GrabRounding = style.FrameRounding;

            style.TabBorderSize = 0;
            style.TabBarBorderSize = 2;
            style.TabBarOverlineSize = 2;
            style.TabCloseButtonMinWidthSelected = -1; // -1:always visible, 0:visible when hovered, >0:visible when hovered if minimum width
            style.TabCloseButtonMinWidthUnselected = -1;
            style.TabRounding = rounded;

            style.CellPadding = ImVec2(8.0f, 4.0f);

            style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
            style.WindowMenuButtonPosition = ImGuiDir_Right;

            style.ColorButtonPosition = ImGuiDir_Right;
            style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
            style.SelectableTextAlign = ImVec2(0.5f, 0.5f);
            style.SeparatorTextAlign.x = 1.00f;
            style.SeparatorTextBorderSize = 1;
            style.SeparatorTextPadding = ImVec2(0,0);

            style.WindowMinSize = ImVec2(32.0f, 16.0f);
            style.ColumnsMinSpacing = 6.0f;

            // diamond sliders
            style.CircleTessellationMaxError = shape0123 == 3 ? 4.00f : 0.30f; 

            
            auto dim = [&](ImVec4 hi) {
                float h,s,v; ImGui::ColorConvertRGBtoHSV(hi.x,hi.y,hi.z, h,s,v);
                ImVec4 dim = ImColor::HSV(h,s,lit01 ? v*0.65:v*0.65, hi.w).Value;
                if( hi.z > hi.x && hi.z > hi.y ) return ImVec4(dim.x,dim.y,hi.z,dim.w);
                return dim;
            };

            const ImVec4 cyan    = ImVec4(000/255.f, 192/255.f, 255/255.f, 1.00f);
            const ImVec4 red     = ImVec4(230/255.f, 000/255.f, 000/255.f, 1.00f);
            const ImVec4 yellow  = ImVec4(240/255.f, 210/255.f, 000/255.f, 1.00f);
            const ImVec4 orange  = ImVec4(255/255.f, 144/255.f, 000/255.f, 1.00f);
            const ImVec4 lime    = ImVec4(192/255.f, 255/255.f, 000/255.f, 1.00f);
            const ImVec4 aqua    = ImVec4(000/255.f, 255/255.f, 192/255.f, 1.00f);
            const ImVec4 magenta = ImVec4(255/255.f, 000/255.f,  88/255.f, 1.00f);
            const ImVec4 purple  = ImVec4(192/255.f, 000/255.f, 255/255.f, 1.00f);

            ImVec4 alt = cyan;
            /**/ if( alt07 == 0 || alt07 == 'C' ) alt = cyan;
            else if( alt07 == 1 || alt07 == 'R' ) alt = red;
            else if( alt07 == 2 || alt07 == 'Y' ) alt = yellow;
            else if( alt07 == 3 || alt07 == 'O' ) alt = orange;
            else if( alt07 == 4 || alt07 == 'L' ) alt = lime;
            else if( alt07 == 5 || alt07 == 'A' ) alt = aqua;
            else if( alt07 == 6 || alt07 == 'M' ) alt = magenta;
            else if( alt07 == 7 || alt07 == 'P' ) alt = purple;
            if( lit01 ) alt = dim(alt);

            ImVec4 hi = cyan, lo = dim(cyan);
            /**/ if( hue07 == 0 || hue07 == 'C' ) lo = dim( hi = cyan );
            else if( hue07 == 1 || hue07 == 'R' ) lo = dim( hi = red );
            else if( hue07 == 2 || hue07 == 'Y' ) lo = dim( hi = yellow );
            else if( hue07 == 3 || hue07 == 'O' ) lo = dim( hi = orange );
            else if( hue07 == 4 || hue07 == 'L' ) lo = dim( hi = lime );
            else if( hue07 == 5 || hue07 == 'A' ) lo = dim( hi = aqua );
            else if( hue07 == 6 || hue07 == 'M' ) lo = dim( hi = magenta );
            else if( hue07 == 7 || hue07 == 'P' ) lo = dim( hi = purple );
        //    if( lit01 ) { ImVec4 tmp = hi; hi = lo; lo = lit(tmp); }

            ImVec4 nav = orange;
            /**/ if( nav07 == 0 || nav07 == 'C' ) nav = cyan;
            else if( nav07 == 1 || nav07 == 'R' ) nav = red;
            else if( nav07 == 2 || nav07 == 'Y' ) nav = yellow;
            else if( nav07 == 3 || nav07 == 'O' ) nav = orange;
            else if( nav07 == 4 || nav07 == 'L' ) nav = lime;
            else if( nav07 == 5 || nav07 == 'A' ) nav = aqua;
            else if( nav07 == 6 || nav07 == 'M' ) nav = magenta;
            else if( nav07 == 7 || nav07 == 'P' ) nav = purple;
            if( lit01 ) nav = dim(nav);

            const ImVec4
            link  = ImVec4(0.26f, 0.59f, 0.98f, 1.00f),
            grey0 = ImVec4(0.04f, 0.05f, 0.07f, 1.00f),
            grey1 = ImVec4(0.08f, 0.09f, 0.11f, 1.00f),
            grey2 = ImVec4(0.10f, 0.11f, 0.13f, 1.00f),
            grey3 = ImVec4(0.12f, 0.13f, 0.15f, 1.00f),
            grey4 = ImVec4(0.16f, 0.17f, 0.19f, 1.00f),
            grey5 = ImVec4(0.18f, 0.19f, 0.21f, 1.00f);

            #define Luma(v,a) ImVec4((v)/100.f,(v)/100.f,(v)/100.f,(a)/100.f)

            style.Colors[ImGuiCol_Text]                      = Luma(100,100);
            style.Colors[ImGuiCol_TextDisabled]              = Luma(39,100);
            style.Colors[ImGuiCol_WindowBg]                  = grey1;
            style.Colors[ImGuiCol_ChildBg]                   = ImVec4(0.09f, 0.10f, 0.12f, 1.00f);
            style.Colors[ImGuiCol_PopupBg]                   = grey1;
            style.Colors[ImGuiCol_Border]                    = grey4;
            style.Colors[ImGuiCol_BorderShadow]              = grey1;
            style.Colors[ImGuiCol_FrameBg]                   = ImVec4(0.11f, 0.13f, 0.15f, 1.00f);
            style.Colors[ImGuiCol_FrameBgHovered]            = grey4;
            style.Colors[ImGuiCol_FrameBgActive]             = grey4;
            style.Colors[ImGuiCol_TitleBg]                   = grey0;
            style.Colors[ImGuiCol_TitleBgActive]             = grey0;
            style.Colors[ImGuiCol_TitleBgCollapsed]          = grey1;
            style.Colors[ImGuiCol_MenuBarBg]                 = grey2;
            style.Colors[ImGuiCol_ScrollbarBg]               = grey0;
            style.Colors[ImGuiCol_ScrollbarGrab]             = grey3;
            style.Colors[ImGuiCol_ScrollbarGrabHovered]      = lo;
            style.Colors[ImGuiCol_ScrollbarGrabActive]       = hi;
            style.Colors[ImGuiCol_CheckMark]                 = alt;
            style.Colors[ImGuiCol_SliderGrab]                = lo;
            style.Colors[ImGuiCol_SliderGrabActive]          = hi;
            style.Colors[ImGuiCol_Button]                    = ImVec4(0.10f, 0.11f, 0.14f, 1.00f);
            style.Colors[ImGuiCol_ButtonHovered]             = lo;
            style.Colors[ImGuiCol_ButtonActive]              = grey5;
            style.Colors[ImGuiCol_Header]                    = grey3;
            style.Colors[ImGuiCol_HeaderHovered]             = lo;
            style.Colors[ImGuiCol_HeaderActive]              = hi;
            style.Colors[ImGuiCol_Separator]                 = ImVec4(0.13f, 0.15f, 0.19f, 1.00f);
            style.Colors[ImGuiCol_SeparatorHovered]          = lo;
            style.Colors[ImGuiCol_SeparatorActive]           = hi;
            style.Colors[ImGuiCol_ResizeGrip]                = Luma(15,100);
            style.Colors[ImGuiCol_ResizeGripHovered]         = lo;
            style.Colors[ImGuiCol_ResizeGripActive]          = hi;
            style.Colors[ImGuiCol_InputTextCursor]           = Luma(100,100);
            style.Colors[ImGuiCol_TabHovered]                = grey3;
            style.Colors[ImGuiCol_Tab]                       = grey1;
            style.Colors[ImGuiCol_TabSelected]               = grey3;
            style.Colors[ImGuiCol_TabSelectedOverline]       = hi;
            style.Colors[ImGuiCol_TabDimmed]                 = grey1;
            style.Colors[ImGuiCol_TabDimmedSelected]         = grey1;
            style.Colors[ImGuiCol_TabDimmedSelectedOverline] = lo;
            style.Colors[ImGuiCol_PlotLines]                 = grey5;
            style.Colors[ImGuiCol_PlotLinesHovered]          = lo;
            style.Colors[ImGuiCol_PlotHistogram]             = grey5;
            style.Colors[ImGuiCol_PlotHistogramHovered]      = lo;
            style.Colors[ImGuiCol_TableHeaderBg]             = grey0;
            style.Colors[ImGuiCol_TableBorderStrong]         = grey0;
            style.Colors[ImGuiCol_TableBorderLight]          = grey0;
            style.Colors[ImGuiCol_TableRowBg]                = grey3;
            style.Colors[ImGuiCol_TableRowBgAlt]             = grey2;
            style.Colors[ImGuiCol_TextLink]                  = link;
            style.Colors[ImGuiCol_TextSelectedBg]            = Luma(39,100);
            style.Colors[ImGuiCol_TreeLines]                 = Luma(39,100);
            style.Colors[ImGuiCol_DragDropTarget]            = nav;
            style.Colors[ImGuiCol_NavCursor]                 = nav;
            style.Colors[ImGuiCol_NavWindowingHighlight]     = lo;
            style.Colors[ImGuiCol_NavWindowingDimBg]         = Luma(0,63);
            style.Colors[ImGuiCol_ModalWindowDimBg]          = Luma(0,63);

            if( lit01 ) {
                for(int i = 0; i < ImGuiCol_COUNT; i++) {
                    float H, S, V;
                    ImVec4& col = style.Colors[i];
                    ImGui::ColorConvertRGBtoHSV( col.x, col.y, col.z, H, S, V );
                    if( S < 0.5 ) V = 1.0 - V, S *= 0.15;
                    ImGui::ColorConvertHSVtoRGB( H, S, V, col.x, col.y, col.z );
                }
            }

            #undef Luma
            return 0;
        }

        #if 0
        // demo
        int theme = 0;
        static int hue = 0, alt = 0, nav = 0, shape = 1;
        static bool light = 0, compact = 0, borders = 0;
        theme |= ImGui::SliderInt("Hue", &hue, 0, 7);
        theme |= ImGui::SliderInt("Alt", &alt, 0, 7);
        theme |= ImGui::SliderInt("Nav", &nav, 0, 7);
        theme |= ImGui::SliderInt("Shape", &shape, 0, 3);
        theme |= ImGui::Checkbox("Light", &light);
        theme |= ImGui::Checkbox("Compact", &compact);
        theme |= ImGui::Checkbox("Borders", &borders);
        if(theme) igThemeV3(hue["CRYOLAMP"], alt["CRYOLAMP"], nav["CRYOLAMP"], light, compact, borders, shape);
        #endif
        
    } // namespace Core
} // namespace Morpion