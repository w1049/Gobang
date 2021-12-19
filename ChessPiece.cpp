#include <cmath>

#include "ChessPiece.h"

ChessPiece::ChessPiece(int pid = 0, int8_t posX = 0, int8_t posY = 0)
    : pid(pid), posX(posX), posY(posY) {}

int ChessPiece::getPid() const { return pid; }

int8_t ChessPiece::getX() const { return posX; }

int8_t ChessPiece::getY() const { return posY; }

ChessPiece& ChessPiece::set(int p, int8_t x, int8_t y) {
    pid = p, posX = x, posY = y;
    return *this;
}

ChessPiece& ChessPiece::setPid(int p) {
    pid = p;
    return *this;
}

bool ChessPiece::operator<(const ChessPiece& p) const {
    return abs(posX - 7) + abs(posY - 7) > abs(p.posX - 7) + abs(p.posY - 7);
}