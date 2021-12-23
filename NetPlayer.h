#ifndef __NET_PLAYER_H__
#define __NET_PLAYER_H__

#include <qmutex.h>
#include <qwaitcondition.h>

#include "ChessPad.h"
#include "Player.h"

class NetPlayer : public Player {
public:
    NetPlayer(int);
    ChessPiece getNextPiece(const ChessPad&);
    void infoFailed(const ChessPiece&, int);
    int command(const ChessPad&);
    int8_t Tx = -1, Ty = -1;
    int cmd = -1;
    QMutex mutex;
    QWaitCondition hasCmd;
    int undoLimit = 0;
    // private:
};

#endif
