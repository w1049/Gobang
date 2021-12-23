#ifndef __CMD_PLAYER_H__
#define __CMD_PLAYER_H__

#include "Player.h"

class CmdPlayer : public Player {
public:
    CmdPlayer(int);
    ChessPiece getNextPiece(const ChessPad&);
    void infoFailed(const ChessPiece&, int);
    int command(const ChessPad&);

private:
    int8_t Tx, Ty;
};

#endif
