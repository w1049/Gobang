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
    uint8_t pad[15][15];
    uint8_t line[9];
    void getVec(const ChessPad*);
    std::vector<ChessPiece> vec;
    int f(uint8_t, const ChessPad*);
    void getLine(ChessPiece, int8_t);
    void getLine(ChessPiece, int8_t, int8_t);
    int getType();
    int getScores(int[]);
};

#endif