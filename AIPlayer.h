#ifndef __AI_PLAYER_H__
#define __AI_PLAYER_H__

#include "ChessPad.h"
#include "Player.h"

class AIPlayer : public Player {
public:
    AIPlayer(int8_t);
    ChessPiece getNextPos(const ChessPad&);
private:
    void getVec(const ChessPad&);
    void generate(const ChessPad& chessPad, cpv& v, int8_t);
    cpv vec;
    int f(int8_t, const ChessPad&, int[11]);
    int g(int8_t, const ChessPad&);
    int getExScore(int[]);
    int dfs(int d, ChessPad& pad, int8_t);
    int depth;
    int a, b;
};

#endif