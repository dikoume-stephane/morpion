#pragma once 

#include <random>
#include <ctime>
#include "structs.h" 

namespace Morpion {
namespace Core {

enum class playertype {
    PLAYER,
    ORDI
};

enum class IAlevel {
    FACILLE,
    INTERMEDIAIRE,
    IMBATABLE
};

class Player {
private:
    playertype type;
    int wincounter;
    bool isactive;
    int Id;
    int ordilevel;
    Color wincolor;
    SDL_Texture* icone = nullptr;

public:
    //constructeur
    Player(playertype type, int Id, int wincounter, Color wincolor);

    //setter
    void seticone(SDL_Texture* ico) { icone = ico;}
    void Setwinclor(Color couleur) { wincolor = couleur; }
    void incrementerscore(){ wincounter++; }
    void Setordilevel(int level) { ordilevel = level;}

    //getter
    playertype gettype() const{ return type; }
    Color getwincolor() const { return wincolor;}
    int getscore() const{ return wincounter; }
    int getid() const{ return Id; }
    SDL_Texture* geticone() const{ return icone; }


    //fonctionnement
    void reinitialize();

    //methode pour IA
    int choisirCoup(const std::vector<Case>& grille, int nbrAalig);
    int coupAleatoire(const std::vector<Case>& grille);
    
    // Évalue la grille pour le Minimax (Victoire/Défaite/Nul)
    int calculMeilleurcoup(std::vector<Case>& grille, int profondeur, int n);
    bool aUnVoisinProche(const std::vector<Case>& grille, int index, int N);
    bool checkwinLocal(const std::vector<Case>& grille, int dernierIdx, int N, int nRequis);

    // Heuristique pour le 5x5 (Score de position)
    int scoreHeuristique(const std::vector<Case>& grille, int n);
    int evaluerSegment(const std::vector<Case>& grille, int depart, int pas, int n);

    // L'algorithme Minimax avec Alpha-Beta
    int minimax(std::vector<Case>& virtuelle, int profondeur, bool estIA, int alpha, int beta, int n, int dernierCoup);
};
}
}