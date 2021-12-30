#include "CmdGame.h"

#include <iomanip>
#include <iostream>

#include "AIPlayer.h"
#include "CmdPlayer.h"

using std::cin;
using std::cout;
using std::endl;

CmdGame::CmdGame(int type, bool mode) {
    switch (type) {
    case 1:
        p[0] = new CmdPlayer(1);
        p[1] = new AIPlayer(2);
        break;
    case 2:
        p[0] = new AIPlayer(1);
        p[1] = new CmdPlayer(2);
        break;
    default:
        p[0] = new CmdPlayer(1);
        p[1] = new CmdPlayer(2);
        break;
    }
    chessPad = new ChessPad(mode);
}

void CmdGame::infoRecommend(const ChessPiece& p) {
    cout << "Recommand: " << char(p.getY() + 'A') << (int)p.getX() + 1 << endl;
}

void CmdGame::infoTips(int pid) {
    cout << "当前局势：" << -AI::g(3 - turn, *chessPad) << endl;
    ChessPiece p;
    cpv banned, win5;
    for (int8_t x = 0; x < 15; x++)
        for (int8_t y = 0; y < 15; y++) {
            int code = chessPad->checkState(p.set(pid, x, y));
            if (code == 3 || code == 4 || code == 5) banned.push_back(p);
        }
    for (int8_t x = 0; x < 15; x++)
        for (int8_t y = 0; y < 15; y++) {
            if (!chessPad->checkState(p.set(3 - pid, x, y)) &&
                chessPad->judgeWinner(p))
                win5.push_back(p);
        }
    if (!banned.empty()) {
        cout << "警告：禁手点 ";
        for (auto p : banned)
            cout << char(p.getY() + 'A') << (int)p.getX() + 1 << " ";
        cout << endl;
    }
    if (!win5.empty()) {
        cout << "警告：成五点 ";
        for (auto p : win5)
            cout << char(p.getY() + 'A') << (int)p.getX() + 1 << " ";
        cout << endl;
    }
}

void CmdGame::infoGameOver(int pid) {
    cout << (pid == 1 ? "●" : "○");
    if (p[pid - 1]->getType() == 0) {
        cout << "电脑赢了！" << endl;
    } else {
        cout << "玩家" << (int)pid << "赢了! " << endl;
    }

}  // 提示游戏结束，展示胜者或平局

const char* toS(int x) {
    if (x == 0)
        return "++ ";
    else if (x == 1)
        return "● ";
    else
        return "○ ";
}

void CmdGame::infoPlace(const ChessPiece& p) {
    system("cls");
    cout << "   A  B  C  D  E  F  G  H  I  J  K  L  M  N  O" << endl;
    for (int8_t i = 0; i < 15; i++) {
        cout << std::setw(2) << (int)(i + 1) << " ";
        for (int8_t j = 0; j < 15; j++) {
            if (i == p.getX() && j == p.getY())
                cout << "\b(" << toS(chessPad->getByPos(i, j)) << "\b)";
            else
                cout << toS(chessPad->getByPos(i, j));
        }
        cout << endl;
    }
}
