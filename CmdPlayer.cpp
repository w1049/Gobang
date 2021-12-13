#include <iostream>

#include "CmdPlayer.h"
#include "ChessPiece.h"

using std::cin;
using std::cout;
using std::endl;

CmdPlayer::CmdPlayer(uint8_t p = 0) {
    pid = p;
    type = 0;
}

ChessPiece CmdPlayer::getNextPos(const ChessPad& pad) {
    ChessPiece p;
    uint8_t reason;
    while (1) {
        cout << "轮到" << (pid == 1 ? "●" : "○") << "方下棋。请输入坐标，形式为“A8”\n（不含引号，若格式不对我也不知道会发生什么）: " << endl;
        char cy;
        int x;
        cin.clear(); cin.sync();
        cin >> cy;
        if ('a' <= cy && cy <= 'z') cy -= 'a';
        else if ('A' <= cy && cy <= 'Z') cy -= 'A';
        else continue;
        cin >> x;
        if (cin.fail() || x < 1 || x > 15 || cy < 0 || cy >= 15) continue;
        if (reason = pad.check(p.set(pid, x-1, cy))) {
            infoFailed(p, reason);
        }
        else return p;
    }
}

void CmdPlayer::infoFailed(ChessPiece p, uint8_t reason) {
    cout << "由于原因" << (int)reason << ", 你不能在" << char(p.getPosY() + 'A') << (int)p.getPosX() + 1 << "下棋." << endl;
}