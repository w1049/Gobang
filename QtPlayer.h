#ifndef __QT_PLAYER_H__
#define __QT_PLAYER_H__

#include <qmutex.h>
#include <qwaitcondition.h>

#include "ChessPad.h"
#include "Player.h"

class QtPlayer : public Player {
public:
    QtPlayer(int);
    ChessPiece getNextPiece(const ChessPad&);
    void infoFailed(const ChessPiece&, int);
    int command(const ChessPad&);
    int8_t Tx, Ty;
    int cmd = -1;
    QMutex mutex;
    QWaitCondition hasCmd;
    // private:
};

#endif
