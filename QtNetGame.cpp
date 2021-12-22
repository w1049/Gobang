#include "QtNetGame.h"

#include <QDebug>

#include "AIPlayer.h"
#include "QtPlayer.h"
#include "gamewindow.h"
#include "NetPlayer.h"
#include "mainwindow.h"

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
namespace GameServer {
extern QByteArray sendBlock;
extern QMutex blockMutex;
extern QWaitCondition blockCond;
}
using namespace GameServer;

QtNetGame::QtNetGame(int type, bool mode) {
    switch (type) {
    case 1:
        p[0] = new QtPlayer(1);
        p[1] = new NetPlayer(2);
        break;
    case 2:
        p[0] = new NetPlayer(1);
        p[1] = new QtPlayer(2);
        break;
    }
    chessPad = new ChessPad(mode);
    connect(this, &QtNetGame::sendData, GW, &GameWindow::sendData);
    connect(this, &QtNetGame::upd, GW, &GameWindow::upd);
}

void QtNetGame::start() {
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

QDataStream& operator<<(QDataStream& o, const ChessPiece& p) {
    o << (int8_t)p.getPid() << p.getX() << p.getY();
    return o;
}

QDataStream& operator>>(QDataStream& i, ChessPiece& p) {
    int8_t a, b, c;
    i >> a >> b >> c;
    p.set(a, b, c);
    return i;
}

void QtNetGame::infoRemove() {
    // send remove
    // send turn
    blockMutex.lock();
    sendBlock.clear();
    QDataStream out(&sendBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_10);
    out << REMOVE << (int8_t)turn;
    emit sendData();
    blockCond.wait(&blockMutex);
    blockMutex.unlock();

    drawmutex.lock();
    currentPad.remove();
    banned.clear();
    win5.clear();
    rcmd.set(0, -1, -1);
    emit upd(turn);  // 提示改变回合
    drawmutex.unlock();
}

void QtNetGame::infoRecommend(const ChessPiece& p) {
    //send recommend
    //send p

    if (this->p[p.getPid() - 1]->getType() == 1) {
        drawmutex.lock();
        rcmd = p;
        emit upd(0);
        drawmutex.unlock();
    } else {
        blockMutex.lock();
        sendBlock.clear();
        QDataStream out(&sendBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_10);
        out << RECOMMEND << p;
        emit sendData();
        blockCond.wait(&blockMutex);
        blockMutex.unlock();
    }
}

void QtNetGame::infoTips(int pid) {
    int points;
    cpv banned, win5;
    points = -AI::g(3 - turn, *chessPad);
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
    if (this->p[pid - 1]->getType() == 1) {
        drawmutex.lock();
        ::banned = banned;
        ::win5 = win5;
        ::points = points;
        emit upd(0);
        drawmutex.unlock();
    } else {
        // if pid == remotepid then
        //send tips
        //send points
        //send banned & win5
        //send points and winnned win5
        blockMutex.lock();
        sendBlock.clear();
        QDataStream out(&sendBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_10);

        out << TIPS << points;
        out << (uint8_t)banned.size();
        for (auto p : banned) out << p;
        out << (uint8_t)win5.size();
        for (auto p : win5) out << p;
        emit sendData();
        blockCond.wait(&blockMutex);
        blockMutex.unlock();
    }
}

void QtNetGame::infoGameOver(int pid) {
    // send gameover
    // send pid
    blockMutex.lock();
    sendBlock.clear();
    QDataStream out(&sendBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_10);
    qDebug() << "infoGameOver" << pid;
    out << GAMEOVER << (int8_t)pid;
    emit sendData();
    blockCond.wait(&blockMutex);
    blockMutex.unlock();

    winnerid = pid;
}

void QtNetGame::infoPlace(const ChessPiece& p) {
    // send place
    // send p 
    // send turn
    blockMutex.lock();
    sendBlock.clear();
    QDataStream out(&sendBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_10);
    out << PLACE << p << (int8_t)turn;
    qDebug() << p.getX() << p.getY();
    emit sendData();
    blockCond.wait(&blockMutex);
    blockMutex.unlock();

    drawmutex.lock();
    currentPad.place(p);
    banned.clear();
    win5.clear();
    rcmd.set(0, -1, -1);
    emit upd(3 - turn);  // 提示改变回合
    drawmutex.unlock();
}
