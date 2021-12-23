#include "QtNetGame.h"

#include "AIPlayer.h"
#include "GameWindow.h"
#include "QtPlayer.h"

namespace render {
extern QMutex drawmutex;
extern ChessPad currentPad;
extern cpv banned, win5;
extern ChessPiece rcmd;
extern int winnerid;
extern int points;
}  // namespace render
using namespace render;

namespace GameServer {
extern QByteArray sendBlock;
extern QMutex blockMutex;
extern QWaitCondition blockCond;
extern bool infoBan, infoWin1, infoWin2, ai1, ai2;
extern int undo1, undo2;
}  // namespace GameServer
using namespace GameServer;

class GameWindow;
extern GameWindow* GW;

QtNetGame::QtNetGame(int type, bool mode) {
    switch (type) {
    case 1:
        p[0] = new QtPlayer(1);
        dynamic_cast<QtPlayer*>(p[0])->undoLimit = undo1;
        p[1] = new QtPlayer(2, 2);
        dynamic_cast<QtPlayer*>(p[1])->undoLimit = undo2;
        break;
    case 2:
        p[0] = new QtPlayer(1, 2);
        dynamic_cast<QtPlayer*>(p[0])->undoLimit = undo2;
        p[1] = new QtPlayer(2);
        dynamic_cast<QtPlayer*>(p[1])->undoLimit = undo1;
        break;
    }
    chessPad = new ChessPad(mode);
    connect(this, &QtNetGame::sendData, GW, &GameWindow::sendData);
    connect(this, &QtNetGame::upd, GW, &GameWindow::upd);
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

bool QtNetGame::canUndo(int pid) {
    QtPlayer* player = dynamic_cast<QtPlayer*>(p[pid - 1]);
    if (chessPad->getPiecesList().size() < 3 && player->getPid() == 2) return 0;
    if (player->undoLimit) {
        --player->undoLimit;
        return 1;
    } else {
        if (player->getPid() == 2) {
            blockMutex.lock();
            sendBlock.clear();
            QDataStream out(&sendBlock, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_0);
            out << (quint16)0;
            out << STOPUNDO;
            out.device()->seek(0);
            out << (quint16)(sendBlock.size() - sizeof(quint16));
            emit sendData();
            blockCond.wait(&blockMutex);
            blockMutex.unlock();
        }

        return 0;
    }
}

void QtNetGame::infoRemove() {
    // send remove
    // send turn
    blockMutex.lock();
    sendBlock.clear();
    QDataStream out(&sendBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << REMOVE << (int8_t)turn;
    out.device()->seek(0);
    out << (quint16)(sendBlock.size() - sizeof(quint16));
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
    // send recommend
    // send p
    if (this->p[p.getPid() - 1]->getType() == 1) {
        drawmutex.lock();
        rcmd = p;
        GW->setAIButtonEnabled(1);
        emit upd(0);
        drawmutex.unlock();
    } else {
        blockMutex.lock();
        sendBlock.clear();
        QDataStream out(&sendBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (quint16)0;
        out << RECOMMEND << p;
        out.device()->seek(0);
        out << (quint16)(sendBlock.size() - sizeof(quint16));
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
    if (infoBan)
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
        if (infoWin1) ::win5 = win5;
        ::points = points;
        emit upd(0);
        drawmutex.unlock();
    } else {
        if (!infoWin2) win5.clear();
        // if pid == remotepid then
        // send tips
        // send points
        // send banned & win5
        // send points and winnned win5
        blockMutex.lock();
        sendBlock.clear();
        QDataStream out(&sendBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (quint16)0;

        out << TIPS << points;
        out << (uint8_t)banned.size();
        for (auto p : banned) out << p;
        out << (uint8_t)win5.size();
        for (auto p : win5) out << p;
        out.device()->seek(0);
        out << (quint16)(sendBlock.size() - sizeof(quint16));
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
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    qDebug() << "infoGameOver" << pid;
    out << GAMEOVER << (int8_t)pid;
    out.device()->seek(0);
    out << (quint16)(sendBlock.size() - sizeof(quint16));
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
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << PLACE << p << (int8_t)turn;
    // qDebug() << p.getX() << p.getY();
    out.device()->seek(0);
    out << (quint16)(sendBlock.size() - sizeof(quint16));
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
