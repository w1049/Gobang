#include <cstring>
#include <iostream>
#include "AIPlayer.h"
#include "ChessPiece.h"

AIPlayer::AIPlayer(uint8_t p = 0) {
    pid = p;
    type = 1;
    memset(pad, 0, sizeof(pad));
}

ChessPiece AIPlayer::getNextPos(const ChessPad* chessPad) {
    ChessPiece p;
//    if (cin.fail() || x < 1 || x > 15 || cy < 0 || cy >= 15) continue;
//    if (reason = pad->check(p = ChessPiece(pid, x-1, cy))) {
//        infoFailed(p, reason);
    chessPad->writePad(pad);
    int maxVal = -1E9-7, tmp;
    uint8_t maxX = 0, maxY = 0;
    for (uint8_t x = 0; x < 15; x++)
        for (uint8_t y = 0; y < 15; y++) {
            if (chessPad->check(p.set(pid, x, y)) != 0) continue;
            pad[x][y] = pid;
            if ((tmp = f()) > maxVal) {
                maxVal = tmp;
                maxX = x, maxY = y;
            }
            pad[x][y] = 0;
        }
    return p.set(pid, maxX, maxY);
}

int AIPlayer::f() {
    return 0;
}
