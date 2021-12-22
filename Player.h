#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "ChessPad.h"

class Player {
protected:
    int pid;
    int type;

public:
    Player(int, int);
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;
    int getPid();
    int getType();
    // 传入当前情况, 获取下一步的位置.
    virtual ChessPiece getNextPiece(const ChessPad&) = 0;
    // 玩家可能输入指令
    virtual int command(const ChessPad&);
    virtual ~Player();
};

#endif
