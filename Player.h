#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "ChessPad.h"

class Player {
protected:
    int pid;
    int type;
public:
    Player(int, int);
    int getPid();
    int getType();
    // ���뵱ǰ���, ��ȡ��һ����λ��. ��������ǵȴ�һ������, ����AI�Ǽ���
    virtual ChessPiece getNextPos(const ChessPad&) = 0;
};

#endif