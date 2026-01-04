#include "core/game_engine.h" 
#include <iostream> 
#include <cstdlib> 
#include <windows.h> 
#include <thread>

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8); 
    // Initialisation de l'aléatoire 
    std::srand(static_cast<unsigned int>(std::time(nullptr))); 
     
    std::cout << "🎮Démarrage du morpion" << std::endl; 
    std::cout << "======================" << std::endl; 
     
    // 🏗 Création du moteur de jeu 
    Morpion::Core::Game engine("morpion", 1280.f, 960.f);
     
    // ⚙Initialisation 
    if (!engine.Initialize()) { 
        std::cerr << "❌Erreur: Impossible d'initialiser le moteur de jeu" << std::endl;
        return -1; 
    }
     
    // Boucle principale 
    engine.Run(); 
     
    // Arrêt propre 
    engine.Shutdown(); 
     
    std::cout << "👋arret du jeu. Au revoir !" << std::endl; 
    return 0; 
} 
