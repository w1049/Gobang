#ifndef __AI_PLAYER_H__
#define __AI_PLAYER_H__

#include "ChessPad.h"
#include "Player.h"

class AIPlayer : public Player {
public:
    AIPlayer(int8_t);
    ChessPiece getNextPos(const ChessPad&);
private:
    void generate(const ChessPad& chessPad, cpv& v, int8_t);
    int f(int8_t, const ChessPad&, int[11]);
    int g(int8_t, const ChessPad&);
    int getExScore(int[]);
    int dfs(ChessPiece&, int d, ChessPad& pad, int8_t, int = -1e9-7, int = 1e9 + 7);
    int depth;
    int a, b;
};

#endif