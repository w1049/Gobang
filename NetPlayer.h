#ifndef __NET_PLAYER_H__
#define __NET_PLAYER_H__

#include "ChessPad.h"
#include "Player.h"
#include <winsock.h>

class NetPlayer : public Player {
public:
    NetPlayer(int, SOCKET*);
    ChessPiece getNextPos(const ChessPad&);
    void infoFailed(const ChessPiece&, int8_t);
    int command(const ChessPad&);
    const SOCKET* getSock();
private:
    int8_t Tx, Ty;
    SOCKET* sock;
};

#endif