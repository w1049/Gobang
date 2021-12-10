#include <iostream>

#include "CmdPlayer.h"
#include "ChessPiece.h"

using std::cin;
using std::cout;
using std::endl;

CmdPlayer::CmdPlayer(uint8_t p = 0) {
    pid = p;
}

ChessPiece CmdPlayer::getNextPos(const ChessPad* pad) {
    ChessPiece p;
    uint8_t reason;
    while (1) {
        cout << "轮到" << (pid == 1 ? "●" : "○") << "方下棋。请输入坐标，形式为“A8”\n（不含引号，若格式不对我也不知道会发生什么）: " << endl;
        char cy;
        int x;
        cin >> cy >> x;
        if (reason = pad->check(p = ChessPiece(pid, x-1, cy-'A'))) {
            infoFailed(p, reason);
        }
        else return p;
    }
}

void CmdPlayer::infoFailed(ChessPiece p, uint8_t reason) {
    cout << "由于原因" << (int)reason << ", 你不能在(" << (int)p.getPosX() << "," << (int)p.getPosY() << ")下棋." << endl;
}