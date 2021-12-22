#include "QtGame.h"

#include <QDebug>

#include "AIPlayer.h"
#include "QtPlayer.h"
#include "gamewindow.h"

namespace render {
extern QMutex drawmutex;
extern ChessPad currentPad;
extern cpv banned, win5;
extern ChessPiece rcmd;
extern int winnerid;
extern int points;
}  // namespace render
using namespace render;

class GameWindow;
extern GameWindow* GW;

QtGame::QtGame(int type, bool mode) {
    switch (type) {
    case 1:
        p[0] = new QtPlayer(1);
        p[1] = new AIPlayer(2);
        break;
    case 2:
        p[0] = new AIPlayer(1);
        p[1] = new QtPlayer(2);
        break;
    default:
        p[0] = new QtPlayer(1);
        p[1] = new QtPlayer(2);
        break;
    }
    chessPad = new ChessPad(mode);
    connect(this, &QtGame::upd, GW, &GameWindow::upd);
}

void QtGame::start() {
    while (1) {
        int code = 0;
        if (p[turn - 1]->getType()) {
            infoTips(turn);
            code = p[turn - 1]->command(*chessPad);
            if (code == 1) {  // undo
                if (!undo()) continue;
                if (p[turn - 1]->getType() == 0 ||
                    p[turn - 1]->getType() == 2 || p[2 - turn]->getType() == 2)
                    if (!undo()) continue;
                continue;
            } else if (code == 2) {  // ask
                AIPlayer ai(turn);
                ChessPiece p = ai.getNextPiece(*chessPad);
                infoRecommend(p);
                continue;
            }
        }
        if (step(code == 3)) break;
    }
}

void QtGame::infoRemove() {
    drawmutex.lock();
    currentPad.remove();
    banned.clear();
    win5.clear();
    rcmd.set(0, -1, -1);
    emit upd(turn);  // 提示改变回合
    drawmutex.unlock();
}

void QtGame::infoRecommend(const ChessPiece& p) {
    drawmutex.lock();
    rcmd = p;
    emit upd(0);
    drawmutex.unlock();
}

void QtGame::infoTips(int pid) {
    drawmutex.lock();
    points = -AI::g(3 - turn, *chessPad);
    banned.clear();
    win5.clear();
    ChessPiece p;
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
    emit upd(0);
    drawmutex.unlock();
}

void QtGame::infoGameOver(int pid) { winnerid = pid; }

void QtGame::infoPlace(const ChessPiece& p) {
    drawmutex.lock();
    currentPad.place(p);
    banned.clear();
    win5.clear();
    rcmd.set(0, -1, -1);
    emit upd(3 - turn);  // 提示改变回合
    drawmutex.unlock();
}
