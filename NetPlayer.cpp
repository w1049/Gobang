#include "NetPlayer.h"

#include <QDebug>
#include <QDataStream>
#include <QTcpSocket>

#include "ChessPiece.h"

NetPlayer::NetPlayer(int p) : Player(p, 3), Tx(0), Ty(0) {}

ChessPiece NetPlayer::getNextPiece(const ChessPad&) {
    return ChessPiece(pid, Tx, Ty);
}
namespace GameServer {
extern QTcpSocket *clientConnection;
}

using namespace GameServer;
void NetPlayer::infoFailed(const ChessPiece& p, int reason) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_10);
    out << RETRY;
    clientConnection->write(block);
    // qDebug() << "由于原因" << (int)reason << ", 你不能在" << char(p.getY() +
    // 'A') << (int)p.getX() + 1 << "下棋." << '\n';
}
// ask
// undo
// H8
int NetPlayer::command(const ChessPad& pad) {
    ChessPiece p;
    int reason;
    while (1) {
        mutex.lock();
        hasCmd.wait(&mutex); // wait for recieve
        if (cmd == 1) {
            mutex.unlock();
            return 1;
        } else if (cmd == 2) {
            mutex.unlock();
            return 2;
        }
        reason = pad.checkState(p.set(pid, Tx, Ty));
        if (reason == 1 || reason == 2) {
            infoFailed(p, reason);
            mutex.unlock();
        } else if (reason == 3 || reason == 4 || reason == 5) {
            mutex.unlock();
            return 3;
        } else {
            mutex.unlock();
            break;
        }
    }
    return 0;
}
