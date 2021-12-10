#ifndef __AI_PLAYER_H__
#define __AI_PLAYER_H__

#include "ChessPad.h"
#include "Player.h"

class AIPlayer : public Player {
public:
    AIPlayer(uint8_t);
    ChessPiece getNextPos(const ChessPad*);
private:
    uint8_t pad[15][15];
    int f();
};

#endif