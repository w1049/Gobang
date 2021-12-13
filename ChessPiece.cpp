#include "ChessPiece.h"

ChessPiece::ChessPiece(int8_t pid = 0, int8_t posX = 0, int8_t posY = 0)
    : pid(pid), posX(posX), posY(posY) {}

int8_t ChessPiece::getPid() { return pid; }

int8_t ChessPiece::getPosX() { return posX; }

int8_t ChessPiece::getPosY() { return posY; }

ChessPiece& ChessPiece::set(int8_t p, int8_t x, int8_t y) {
    pid = p, posX = x, posY = y;
    return *this;
}