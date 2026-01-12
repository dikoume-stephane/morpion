#include "core/player.h"
#include "core/game_engine.h"
#include <iostream>
#include <cstdlib>  // Pour rand()
#include <algorithm> // Pour std::max et std::min

namespace Morpion
{
    namespace Core
    {
        // IDs des joueurs sur la grille (0 = vide)
        const int ID_HUMAIN = 1;
        const int ID_IA = 2;

        //CONSTRUCTEUR
        Player::Player(playertype t, int id, int wincount) 
        : type(t), Id(id), wincounter(wincount), 
        icone(nullptr), isactive(false) {

            // Initialisation du générateur aléatoire (à faire une seule fois idéalement dans le main, mais ok ici)
            std::srand(std::time(nullptr));
        }

        void Player::reinitialize() {
            // Si tu as des variables internes à remettre à zéro
        }

        // Le Cerveau Principal de l'IA

        int Player::choisirCoup(const std::vector<Case>& grille) {

            //au cas ou le centre n'est pas encore occupé
            int caseoccupe = 0;
            for (const auto& c : grille) if (c.etat != 0) caseoccupe ++;
            if (caseoccupe < 2)
            {
                int centre = grille.size() / 2;
                if (grille.at(centre).etat == 0) return centre;
            }
            // copie de la grille (modifiable pour les ia)
            std::vector<Case>& grilleModifiable = const_cast<std::vector<Case>&>(grille);
            // Aiguillage selon la difficulté
            switch (ordilevel) {
                case (int)IAlevel::FACILLE:
                    return coupAleatoire(grille);

                case (int)IAlevel::INTERMEDIAIRE:
                    // Profondeur faible (2 coups d'avance) = joue bien mais fait des erreurs 
                    // on recicle minimax au lieu de créer une fonction à part
                    return calculMeilleurcoup(grilleModifiable, 2); 

                case (int)IAlevel::IMBATABLE:
                    // Profondeur élevée (4 ou 5) + Alpha-Beta complet
                    return calculMeilleurcoup(grilleModifiable, 5);

                default:
                    return coupAleatoire(grille);
            }
        }

        //  Mode Facile (Aléatoire)

        int Player::coupAleatoire(const std::vector<Case>& grille) {
            std::vector<int> casesVides;
            
            // On liste les possibilités
            for (int i = 0; i < grille.size(); i++) {
                if (grille.at(i).etat == 0) {
                    casesVides.push_back(i);
                }
            }

            if (casesVides.empty()) return -1; // Plus de place

            // Tirage au sort
            int indexHasard = std::rand() % casesVides.size();
            return casesVides[indexHasard];
        }

        int Player::calculMeilleurcoup(std::vector<Case>& grille, int profondreur)
        {
            int meilleurScore = -200000;
            int meilleurCoup = -1;
            
            for (int i = 0; i < grille.size(); i++) {
                if (grille.at(i).etat == 0) {
                    grille.at(i).etat = ID_IA; // On joue
                    
                    // On lance la récursion
                    int val = minimax(grille, profondreur, false, -200000, 200000);
                    
                    grille.at(i).etat = 0; 

                    if (val > meilleurScore) {
                        meilleurScore = val;
                        meilleurCoup = i;
                    }
                }
            }
            return meilleurCoup;
        }

        // Le Moteur Minimax (Alpha-Bêta)

        int Player::minimax(std::vector<Case>& virtuelle, int profondeur, bool estIA, int alpha, int beta) {
            
            // A. Vérifier si le jeu est fini (Victoire réelle)
            int etatJeu = verifierEtatJeu(virtuelle);
            if (etatJeu != 0) {
                return etatJeu; // +10000 ou -10000
            }

            // B. Limite de profondeur atteinte (Pas de victoire, on estime)
            if (profondeur == 0) {
                return scoreHeuristique(virtuelle);
            }

            // C. Boucle de simulation
            if (estIA) { // MAXIMISER (IA)
                int meilleurScore = -100000;
                int meilleurCoup = -1;

                for (int i = 0; i < virtuelle.size(); i++) {
                    if (virtuelle.at(i).etat == 0) {
                        // 1. Simuler
                        virtuelle.at(i).etat = ID_IA;
                        
                        // 2. Appel récursif (c'est au tour de l'humain maintenant)
                        // Note : Pour le tout premier appel (racine), on veut récupérer le COUP, pas le score.
                        // Ici c'est la version récursive interne qui renvoie le score.
                        int score = minimax(virtuelle, profondeur - 1, false, alpha, beta);

                        // 3. Backtracking (Nettoyage) !!!
                        virtuelle.at(i).etat = 0;

                        // 4. Meilleur score ?
                        if (score > meilleurScore) {
                            meilleurScore = score;
                            meilleurCoup = i;
                        }
                        
                        // 5. Alpha-Beta
                        alpha = std::max(alpha, score);
                        if (beta <= alpha) break; // Coupure alpha-bêta
                    }
                }
                
                // Astuce : Si on est à la racine de l'appel (profondeur max), on veut renvoyer l'INDEX du coup, pas le score.
                // Comme cette fonction est récursive, elle renvoie des scores. 
                // L'appelant (choisirCoup) devra être adapté ou on sépare la logique racine.
                // **VERSION SIMPLE ICI** : Cette fonction renvoie le SCORE pour la récursion. 
                // Pour renvoyer le coup à `choisirCoup`, il faut une petite astuce dans la boucle racine (voir note en bas).
                return meilleurScore;

            } else { // MINIMISER (Humain)
                int meilleurScore = 100000;
                for (int i = 0; i < virtuelle.size(); i++) {
                    if (virtuelle.at(i).etat == 0) {
                        virtuelle.at(i).etat = ID_HUMAIN;
                        int score = minimax(virtuelle, profondeur - 1, true, alpha, beta);
                        virtuelle.at(i).etat = 0;

                        meilleurScore = std::min(meilleurScore, score);
                        beta = std::min(beta, score);
                        if (beta <= alpha) break;
                    }
                }
                return meilleurScore;
            }
        }

        // L'Heuristique (Système de points)

        int Player::scoreHeuristique(const std::vector<Case>& grille) {
            int scoreTotal = 0;
            int N = sqrt(grille.size()); // Taille de la grille (racine carrée de grille.size())
            
            // On scanne toutes les lignes, colonnes et diagonales
            //lignes
           for ( int i = 0; i < N; i++)
           {
            scoreTotal += evaluerLigne(grille, i * N, 1);
           }
           //colonnes
           for ( int i = 0; i < N; i++)
           {
            scoreTotal += evaluerLigne(grille, i, N);
           }
           
           //diagonale descendante
           scoreTotal += evaluerLigne(grille, 0, N + 1);

           //diagonale montante
           scoreTotal += evaluerLigne(grille, N - 1, N - 1);

            return scoreTotal;
        }

        int Player::evaluerLigne(const std::vector<Case>& grille, int depart, int pas) {
            int nbIA = 0, nbHumain = 0, nbVides = 0;
            int N = sqrt(grille.size());
            // Analyse de 5 cases consécutives
            for (int k = 0; k < 5; k++) {
                int idx = depart + k * pas;
                if (idx >= grille.size()) break; // Sécurité

                if (grille.at(idx).etat == ID_IA) nbIA++;
                else if (grille.at(idx).etat == ID_HUMAIN) nbHumain++;
                else nbVides++;
            }

            // LE BARÈME 
            if (nbIA == N) return 10000; // Victoire
            if (nbHumain == N) return -10000; // Défaite

            if (nbIA == N - 1 && nbVides == 1) return 100;    // Victoire quasi certaine
            if (nbHumain == N - 1 && nbVides == 1) return -95; // Danger mortel (priorité absolue de blocage)
            
            if (nbIA == N - 2 && nbVides == 2) return 10;
            if (nbHumain == N - 2 && nbVides == 2) return -5;

            if (nbIA > 0 && nbHumain > 0) return 0; // Ligne bloquée (mélangée), vaut 0

            return nbIA; // Petit bonus pour les pions isolés
        }

        //  Vérification Victoire (Utilitaire) pour minimax

        int Player::verifierEtatJeu(const std::vector<Case>& grille) {
            // Cette fonction doit réutiliser ta logique de victoire existante (Game::checkWin)
            int taille = sqrt(grille.size());
            if (Game::checkwin(grille, taille, ID_IA, nullptr)) return 100000;
            if (Game::checkwin(grille, taille, ID_HUMAIN, nullptr)) return -100000;
            
            bool plein = true;
            for (const auto& c : grille)
            {
                if (c.etat == 0)
                {
                    plein = false;
                    break;
                }
            }
            if (plein) return 0;

            return 0; 
        }


    }// namespace Core
}// namespace Morpion