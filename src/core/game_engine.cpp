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

            gRunningstatus = true;
            std::cout << "✅ Moteur de jeu initialisé" << std::endl;
            return true;
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
                std::string msg = (joueuractuel->getid() == 1) ? "JOUEUR 1 A GAGNE !" : "JOUEUR 2 A GAGNE !";
                if (iawin) msg = "L'ORDINATEUR A GAGNE !";
                
                CenteredText(msg.c_str());
                ImGui::Separator();
                ImGui::Spacing();

                // --- BOUTON RECOMMENCER (BLEU) ---
                // On change temporairement la couleur du bouton
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.7f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
                
                // ImageButton : ID unique, Texture, Taille, UVs, Bordure, Fond, Teinte
                if (ImGui::ImageButton("##retry", (ImTextureID)CurrentTheme.home, ImVec2(32, 32))) {
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

                if (ImGui::ImageButton("##menu", (ImTextureID)CurrentTheme.home, ImVec2(32, 32))) {
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
            
            // 2. Remettre l'IA à zéro et winner à false
            iawin = false;
            winner = false;

            //on annule la victoire
            momentVictoire = 0;
            
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
    } // namespace Core
} // namespace Morpion