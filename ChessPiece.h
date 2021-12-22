#ifndef __CHESS_PIECE_H__
#define __CHESS_PIECE_H__

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <string>
#include <vector>

typedef signed char int8_t;
typedef unsigned char uint8_t;

const int INF = 1000000007;

enum ChessType {
    WIN5,
    ALIVE4,
    DIE4,
    LOWDIE4,
    ALIVE3,
    TIAO3,
    DIE3,
    ALIVE2,
    LOWALIVE2,
    DIE2,
    NOTHREAT
};

namespace GameServer {
const int8_t REMOVE = 1;
const int8_t RECOMMEND = 2;
const int8_t TIPS = 3;
const int8_t GAMEOVER = 4;
const int8_t PLACE = 5;
const int8_t CLICK = 6;
const int8_t COMMAND = 7;
}

class ChessPiece {
private:
    int pid;
    int8_t posX;
    int8_t posY;

public:
    ChessPiece(int = 0, int8_t = -1, int8_t = -1);
    int getPid() const;
    int8_t getX() const;
    int8_t getY() const;
    ChessPiece& set(int, int8_t, int8_t);
    ChessPiece& setPid(int);
    bool operator<(const ChessPiece& p) const;
};

typedef std::vector<ChessPiece> cpv;

#endif
