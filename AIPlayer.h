#ifndef __AI_PLAYER_H__
#define __AI_PLAYER_H__

#include "vector"

#include "ChessPad.h"
#include "Player.h"

class AIPlayer : public Player {
public:
    AIPlayer(uint8_t);
    ChessPiece getNextPos(const ChessPad*);
private:
    uint8_t rec[15][15][4];
    void getVec(const ChessPad*);
    std::vector<ChessPiece> vec;
    int f(uint8_t, const ChessPad*, int[11], ChessPiece);
    int g(uint8_t, const ChessPad*, int&, int&, ChessPiece);
    int getExScore(int[]);
};

#endif