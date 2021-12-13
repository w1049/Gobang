#ifndef __AI_PLAYER_H__
#define __AI_PLAYER_H__

#include "vector"

#include "ChessPad.h"
#include "Player.h"

typedef std::vector<ChessPiece> cpv;

class AIPlayer : public Player {
public:
    AIPlayer(uint8_t);
    ChessPiece getNextPos(const ChessPad&);
private:
    void getVec(const ChessPad&);
    void generate(const ChessPad& chessPad, cpv& v);
    cpv vec;
    int f(uint8_t, const ChessPad&, int[11], ChessPiece);
    int g(uint8_t, const ChessPad&, int&, int&, ChessPiece);
    int getExScore(int[]);
    void dfs(int d, ChessPad& pad);
    int depth;
};

#endif