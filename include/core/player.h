#pragma once 

#include "structs.h" 

namespace Morpion {
namespace Core {

enum class playertype {
    PLAYER1,
    PLAYER2,
    ORDI
};

class Player {
private:
    int wincounter;
    bool isactive;
    int ordilevel;
    playertype type;

public:
    void reinitialize();
    int ordibasic();
    int ordimoyen();
    int minimax();
};
}
}