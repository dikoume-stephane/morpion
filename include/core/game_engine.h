#pragma once

#include <vector>
#include "structs.h"
#include "player.h"
#include "../graphics/window.h"
#include "../../libs/imgui/imgui.h"
#include "../../libs/imgui/backends/imgui_impl_sdl3.h"
#include "../../libs/imgui/backends/imgui_impl_sdlrenderer3.h"

namespace Morpion
{
    namespace Core
    {
        class Player;
    }
}
namespace Morpion
{
    namespace Core
    {

        enum class GameState
        {
            MENU,
            PLAYING,
            GAMEOVER
        };

        class Game
        {
        private:
            // 🔒 ÉTAT DU MOTEUR
            Graphics::Window gWindow;
            SDL_Renderer *grenderer;
            Player *joueur1;
            Player *joueur2;
            Player *joueuractuel;
            Morpion::Core::theme CurrentTheme;
            bool gRunningstatus;
            int IAlevel = 0;
            int gGrilleTaile = 3;
            int modeselect = 1;
            std::vector<Case> grille;
            std::vector<int> indiceGagants;
            GameState Etatactuel = Morpion::Core::GameState::MENU;

        public:
            Uint32 timeOfLastMove = 0;
            bool waitingForIA = false;
            int Idplayer = 1;
            float momentVictoire = 0.f;
            int nbAlignerPourGagner = 3;
            bool winner = false;
            bool iawin = false;

        public:
            // 🏗 CONSTRUCTEUR
            Game(const std::string &title, float width, float height);

            // ⚙️ MÉTHODES PRINCIPALES
            bool Initialize();
            void InitialiserPartie(int mode, int tailleGrille, int nivdif);
            void Run();
            void Shutdown();
            void loadGrille(float hauteur, float largeur, int taille);

            // 🎮 GESTION D'ÉVÉNEMENTS
            void HandleEvents();
            void HandleInput(SDL_Event even);

            // LOGIQUE DU JEU
            static bool checkwin(const std::vector<Case>& grille, int gGrilleTaile, int Idplayer, std::vector<int>* indiceGagants, bool* winner, int nbAlignerPourGagner);
            static bool analyseSegment(const std::vector<Case>& grille, int depart, int pas, int gGrilleTaile, int Idplayer, std::vector<int>* indiceGagants, bool* winner, int nbRequis);

        private:
            // MÉTHODES INTERNES
            void Render();
            void RenderT();
            void loadvoid();
            void RenderUI();
            void CenteredText(const char *text);
            void IUshutdown();
            void changegrillsize(int nouvTaille, float h, float w);
            void AfficherFinDePartie();
            void ResetPartie();
            void bordurecasegagnantes(SDL_Renderer *renderer, std::vector<int> &indiceGagants, Color couleurjoueur, std::vector<Case> &grille);
        };
        bool SDL_PointInFRect(SDL_Point *p, SDL_FRect *r);

    } // namespace Core
} // namespace Morpion