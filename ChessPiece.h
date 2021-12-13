#ifndef __CHESS_PIECE_H__
#define __CHESS_PIECE_H__

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

class ChessPiece {
private:
    uint8_t pid;
    uint8_t posX;
    uint8_t posY;
public:
    ChessPiece() = default;
    ChessPiece(uint8_t, uint8_t, uint8_t);
    uint8_t getPid();
    uint8_t getPosX();
    uint8_t getPosY();
    ChessPiece& set(uint8_t, uint8_t, uint8_t);
};

#endif