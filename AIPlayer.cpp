#include <cstring>

#include "AIPlayer.h"
#include "ChessPiece.h"

AIPlayer::AIPlayer(uint8_t p = 0) {
    pid = p;
    memset(pad, 0, sizeof(pad));
}

ChessPiece AIPlayer::getNextPos(const ChessPad* chessPad) {
    ChessPiece p;
//    if (cin.fail() || x < 1 || x > 15 || cy < 0 || cy >= 15) continue;
//    if (reason = pad->check(p = ChessPiece(pid, x-1, cy))) {
//        infoFailed(p, reason);
    int maxVal = 0;
    uint8_t maxX = 0, maxY = 0;
    for (uint8_t x = 0; x < 15; x++)
        for (uint8_t y = 0; y < 15; y++) {
            if (!chessPad->check(p.set(pid, x, y))) continue;
            chessPad->writePad(pad);
            return p;
        }
    return p.set(pid, maxX, maxY);
}