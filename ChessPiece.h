#ifndef __CHESS_PIECE_H__
#define __CHESS_PIECE_H__

#include <vector>

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

class ChessPiece {
private:
    int8_t pid;
    int8_t posX;
    int8_t posY;
public:
    ChessPiece() = default;
    ChessPiece(int8_t, int8_t, int8_t);
    int8_t getPid();
    int8_t getPosX();
    int8_t getPosY();
    ChessPiece& set(int8_t, int8_t, int8_t);
    bool operator<(const ChessPiece& p) const;
};

typedef std::vector<ChessPiece> cpv;

#endif