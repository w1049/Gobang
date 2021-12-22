/*
 * #ifndef __NET_PLAYER_H__
#define __NET_PLAYER_H__

#include <winsock.h>

#include "ChessPad.h"
#include "Player.h"

class NetPlayer : public Player {
public:
    NetPlayer(int, SOCKET*);
    ChessPiece getNextPiece(const ChessPad&);
    void infoFailed(const ChessPiece&, int);
    int command(const ChessPad&);
    const SOCKET* getSock();
private:
    int8_t Tx, Ty;
    SOCKET* sock;
};

#endif
*/
