#ifndef __AI_PLAYER_H__
#define __AI_PLAYER_H__

#include "ChessPad.h"
#include "Player.h"

class AIPlayer : public Player {
public:
    AIPlayer(int);
    ChessPiece getNextPos(const ChessPad&);
private:
    int8_t r;
    int evaluate(ChessPad& pad, const ChessPiece& p);
    void generate(ChessPad& chessPad, cpv& v, int8_t);
    int f(int8_t, const ChessPad&, int[11]);
    int g(int8_t, const ChessPad&);
    int getExScore(int[]);
    int dfs(ChessPiece&, int d, ChessPad& pad, int8_t, int = -INF, int = INF);
    int depth;
    
};

#endif