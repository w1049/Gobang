#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "ChessPad.h"

class Player {
protected:
    int8_t pid;
    int8_t type;
public:
    int8_t getPid();
    int8_t getType();
    // 传入当前情况, 获取下一步的位置. 对于玩家是等待一个输入, 对于AI是计算
    virtual ChessPiece getNextPos(const ChessPad&) = 0;
};

#endif