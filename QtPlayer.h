#ifndef __QT_PLAYER_H__
#define __QT_PLAYER_H__

#include <qmutex.h>
#include <qwaitcondition.h>

#include "ChessPad.h"
#include "Player.h"

class QtPlayer : public Player {
public:
    QtPlayer(int, int = 1);
    ChessPiece getNextPiece(const ChessPad&);
    void infoFailed(const ChessPiece&, int);
    int command(const ChessPad&);
    int8_t Tx = -1, Ty = -1;
    int cmd = -1;
    QMutex mutex;
    QWaitCondition hasCmd;
    int undoLimit = 0;
};

#endif
