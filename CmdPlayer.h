#ifndef __CMD_PLAYER_H__
#define __CMD_PLAYER_H__

#include "ChessPad.h"
#include "Player.h"

class CmdPlayer : public Player {
public:
    CmdPlayer(uint8_t);
    ChessPiece getNextPos(const ChessPad&);
    void infoFailed(ChessPiece, uint8_t);
};

#endif