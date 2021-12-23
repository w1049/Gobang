#include "QtPlayer.h"

#include "GameWindow.h"

QtPlayer::QtPlayer(int p, int type) : Player(p, type) {}

ChessPiece QtPlayer::getNextPiece(const ChessPad&) {
    return ChessPiece(pid, Tx, Ty);
}

void QtPlayer::infoFailed(const ChessPiece&, int) {
    // qDebug() << "由于原因" << (int)reason << ", 你不能在" << char(p.getY() +
    // 'A') << (int)p.getX() + 1 << "下棋." << '\n';
}

extern GameWindow* GW;
int QtPlayer::command(const ChessPad& pad) {
    ChessPiece p;
    int reason;
    while (1) {
        mutex.lock();
        hasCmd.wait(&mutex);
        if (cmd == 1) {
            mutex.unlock();
            return 1;
        } else if (cmd == 2) {
            mutex.unlock();
            if (pid == 1) GW->setAIButtonEnabled(0);
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
