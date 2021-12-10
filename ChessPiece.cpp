#include "ChessPiece.h"

ChessPiece::ChessPiece(uint8_t pid = 0, uint8_t posX = 0, uint8_t posY = 0)
    : pid(pid), posX(posX), posY(posY) {}

uint8_t ChessPiece::getPid() { return pid; }

uint8_t ChessPiece::getPosX() { return posX; }

uint8_t ChessPiece::getPosY() { return posY; }

ChessPiece& ChessPiece::set(uint8_t p, uint8_t x, uint8_t y) {
    pid = p, posX = x, posY = y;
    return *this;
}