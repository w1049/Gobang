#ifndef __CHESS_PIECE_H__
#define __CHESS_PIECE_H__

#include <vector>

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

const int INF = 1000000007;

class ChessPiece {
private:
    int pid;
    int8_t posX;
    int8_t posY;
public:
    ChessPiece() = default;
    ChessPiece(int, int8_t, int8_t);
    int getPid() const;
    int8_t getX() const;
    int8_t getY() const;
    ChessPiece& set(int, int8_t, int8_t);
    ChessPiece& setPid(int);
    bool operator<(const ChessPiece& p) const;
};

typedef std::vector<ChessPiece> cpv;

#endif