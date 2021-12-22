#ifndef __CMD_GAME_H__
#define __CMD_GAME_H__

#include "Game.h"

class CmdGame : public Game {
public:
    CmdGame(int, bool);
    void infoTips(int);
    void infoRecommend(const ChessPiece&);
    void infoGameOver(int pid);
    void infoPlace(const ChessPiece&);
};

#endif
