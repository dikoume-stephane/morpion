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
        Player::Player(playertype t, int id, int wincount, Color col) 
        : type(t), Id(id), wincounter(wincount), wincolor(col),
        icone(nullptr), isactive(false) {

            // Initialisation du générateur aléatoire (à faire une seule fois idéalement dans le main, mais ok ici)
            std::srand(std::time(nullptr));
        }

        void Player::reinitialize() {
            // Si tu as des variables internes à remettre à zéro
        }

        // Le Cerveau Principal de l'IA

        int Player::choisirCoup(const std::vector<Case>& grille, int nbrpg)
        {

            // copie de la grille (modifiable pour les ia)
            std::vector<Case>& grilleModifiable = const_cast<std::vector<Case>&>(grille);
            // Aiguillage selon la difficulté
            switch (ordilevel) {
                case (int)IAlevel::FACILLE:
                    return coupAleatoire(grille);

                case (int)IAlevel::INTERMEDIAIRE:
                    // Profondeur faible (2 coups d'avance) = joue bien mais fait des erreurs 
                    // on recicle minimax au lieu de créer une fonction à part
                    return calculMeilleurcoup(grilleModifiable, 2, nbrpg); 

                case (int)IAlevel::EXPERT:
                    // Profondeur élevée (4 ou 5) + Alpha-Beta complet
                    return calculMeilleurcoup(grilleModifiable, 5, nbrpg);

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

        /**
        * Point d'entrée pour l'IA : trouve l'index de la meilleure case à jouer.
        * @param n : nombre de pions à aligner pour gagner (3, 4 ou 5).
        */
        int Player::calculMeilleurcoup(std::vector<Case>& grille, int profondeur, int n)
        {
            // 1. Initialiser très bas pour que n'importe quel coup (même perdant) soit choisi
            int meilleurScore = -5000000; 
            int meilleurCoup = -1;
            int N = (int)sqrt(grille.size());
            
            // Plan B : Trouver la première case vide au cas où
            int planB = -1;
            for(int k=0; k<grille.size(); k++) {
                if(grille.at(k).etat == 0)
                {
                    planB = k;
                    break;
                }
            }

            std::cout << "IA reflechit (Profondeur " << profondeur << ")..." << std::endl;

            for (int i = 0; i < (int)grille.size(); i++) {
                if (grille.at(i).etat == 0) {
                    if (!aUnVoisinProche(grille, i, N)) continue;

                    grille.at(i).etat = ID_IA;
                    // Utilisation de valeurs alpha/beta très larges
                    int val = minimax(grille, profondeur - 1, false, -5000000, 5000000, n, i);
                    grille.at(i).etat = 0; 

                    if (val > meilleurScore) {
                        meilleurScore = val;
                        meilleurCoup = i;
                    }
                }
            }

            // Sécurité finale
            if (meilleurCoup == -1 || grille.at(meilleurCoup).etat != 0) {
                std::cout << "ALERTE : IA en difficulte, joue le plan B (case " << planB << ")" << std::endl;
                return planB; 
            }

            return meilleurCoup;
        }

        /**
        * Algorithme Minimax avec élagage Alpha-Bêta et détection locale de victoire.
        */
        int Player::minimax(std::vector<Case>& virtuelle, int profondeur, bool estIA, int alpha, int beta, int n, int dernierCoup)
        {
            
            // 1. Vérification de victoire basée sur le DERNIER coup joué
            if (dernierCoup != -1) {
                if (checkwinLocal(virtuelle, dernierCoup, (int)sqrt(virtuelle.size()), n)) {
                    // Si on vient de trouver un gagnant, on renvoie un score terminal
                    // Si estIA est vrai, cela veut dire que c'est l'humain qui a joué le dernierCoup
                    return estIA ? (-1000000 - profondeur) : (1000000 + profondeur);
                }
            }

            // 2. Condition d'arrêt (profondeur atteinte ou plateau plein)
            if (profondeur == 0) return scoreHeuristique(virtuelle, n);

            // 3. Boucle de simulation
            int N = (int)sqrt(virtuelle.size());
            if (estIA) { // MAXIMISER (IA)
                int meilleurScore = -100000;

                for (int i = 0; i < virtuelle.size(); i++) {
                    if (virtuelle.at(i).etat == 0) {
                        // 1. Simuler
                        if (!aUnVoisinProche(virtuelle, i, N)) continue;
                        virtuelle.at(i).etat = ID_IA;
                        
                        // 2. Appel récursif (c'est au tour de l'humain maintenant)
                        // Note : Pour le tout premier appel (racine), on veut récupérer le COUP, pas le score.
                        // Ici c'est la version récursive interne qui renvoie le score.
                        int score = minimax(virtuelle, profondeur - 1, false, alpha, beta, n, i);

                        // 3. Backtracking (Nettoyage) !!!
                        virtuelle.at(i).etat = 0;

                        // 4. Meilleur score ?
                        meilleurScore = std::max(meilleurScore, score);
                        
                        // 5. Alpha-Beta
                        alpha = std::max(alpha, score);
                        if (beta <= alpha) break; // Coupure alpha-bêta
                    }
                }
                
                return meilleurScore;

            } else { // MINIMISER (Humain)
                int meilleurScore = 100000;
                for (int i = 0; i < virtuelle.size(); i++) {
                    if (virtuelle.at(i).etat == 0) {
                        if (!aUnVoisinProche(virtuelle, i, N)) continue;

                        virtuelle.at(i).etat = ID_HUMAIN;
                        int score = minimax(virtuelle, profondeur - 1, true, alpha, beta, n, i);
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

        /**
        * Évalue la grille entière en scannant tous les segments possibles de taille 'n'.
        */
        int Player::scoreHeuristique(const std::vector<Case>& grille, int n) {
            int scoreTotal = 0;
            int N = sqrt(grille.size()); // Taille de la grille (racine carrée de grille.size())
            int centre = N / 2;

            // On parcourt la grille pour trouver tous les segments de longueur 'n'
            for (int r = 0; r < N; r++) {
                for (int c = 0; c < N; c++) {
                    // Horizontal
                    if (c <= N - n) scoreTotal += evaluerSegment(grille, r * N + c, 1, n);
                    // Vertical
                    if (r <= N - n) scoreTotal += evaluerSegment(grille, r * N + c, N, n);
                    // Diagonale (\)
                    if (r <= N - n && c <= N - n) scoreTotal += evaluerSegment(grille, r * N + c, N + 1, n);
                    // Diagonale (/)
                    if (r <= N - n && c >= n - 1) scoreTotal += evaluerSegment(grille, r * N + c, N - 1, n);
                }
            }
            // Petit bonus pour le centre (pour départager les scores égaux)
            for (int i = 0; i < grille.size(); i++) {
                if (grille[i].etat == ID_IA) {
                    int r = i / N;
                    int c = i % N;
                    if (r == centre && c == centre) scoreTotal += 50; 
                }
            }

            return scoreTotal;
        }

        /**
        * Attribue un score à un petit segment de 'n' cases.
        */
        int Player::evaluerSegment(const std::vector<Case>& grille, int depart, int pas, int n) {
            int nbIA = 0, nbHumain = 0;

            for (int k = 0; k < n; k++) {
                int etat = grille.at(depart + k * pas).etat;
                if (etat == ID_IA) nbIA++;
                else if (etat == ID_HUMAIN) nbHumain++;
            }

            // 1. Si le segment contient les deux types de pions, il est bloqué (0 point)
            if (nbIA > 0 && nbHumain > 0) return 0;

            // --- SYSTÈME DE POINTS RELATIF À N ---
            if (nbIA > 0) {
                if (nbIA == n) return 100000;      // VICTOIRE (ne devrait pas arriver ici mais par sécurité)
                if (nbIA == n - 1) return 10000;   // Menace immédiate d'attaque
                if (nbIA == n - 2) return 500;     // Construction d'attaque
                return 10;
            }

            if (nbHumain > 0) {
                // DEFENSE PRIORITAIRE
                if (nbHumain == n) return -100000;
                if (nbHumain == n - 1) return -50000; // BLOQUER L'HUMAIN EST PLUS IMPORTANT QUE TOUT
                if (nbHumain == n - 2) return -1000;   // Anticiper le danger
                return -20;
            }

            return 0;
        }

        /**
        * Filtre les cases vides : vrai si la case touche un pion existant (distance 1).
        */
        bool Player::aUnVoisinProche(const std::vector<Case>& grille, int index, int N) {
            int r = index / N;
            int c = index % N;

            // Rayon de 2 pour détecter les "trous" dans les alignements
            for (int dr = -2; dr <= 2; dr++) {
                for (int dc = -2; dc <= 2; dc++) {
                    if (dr == 0 && dc == 0) continue;
                    int nr = r + dr, nc = c + dc;
                    if (nr >= 0 && nr < N && nc >= 0 && nc < N) {
                        if (grille.at(nr * N + nc).etat != 0) return true;
                    }
                }
            }
            return false;
        }

        /**
        * Vérifie si le DERNIER pion posé a créé un alignement gagnant.
        */
        bool Player::checkwinLocal(const std::vector<Case>& grille, int dernierIdx, int N, int nRequis) {
            int r = dernierIdx / N;
            int c = dernierIdx % N;
            int id = grille.at(dernierIdx).etat;

            // Vérification sur les 4 axes passant par ce pion
            // Axe Horizontal
            if (Game::analyseSegment(grille, r * N, 1, N, id, nullptr, nullptr, nRequis)) return true;
            // Axe Vertical
            if (Game::analyseSegment(grille, c, N, N, id, nullptr, nullptr, nRequis)) return true;
            
            // Axe Diagonale \ . On remonte au début de la diagonale
            int dist = std::min(r, c);
            if (Game::analyseSegment(grille, (r - dist) * N + (c - dist), N + 1, N - std::abs(r - c), id, nullptr, nullptr, nRequis)) return true;

            // Axe Diagonale / .
            int dist2 = std::min(r, (N - 1) - c);
            if (Game::analyseSegment(grille, (r - dist2) * N + (c + dist2), N - 1, N - std::abs((N - 1 - c) - r), id, nullptr, nullptr, nRequis)) return true;

            return false;
        }


    }// namespace Core
}// namespace Morpion