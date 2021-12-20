#ifndef __GAME_H__
#define __GAME_H__

#include "Player.h"
#include "ChessPad.h"

class Game {
protected:
    Player *p[2];
    ChessPad *chessPad;
    int turn = 1;
public:
    int step();
    void start();
    bool undo();
    virtual void reco(const ChessPiece&) = 0;
    virtual void info(int) = 0;
    virtual void infoGameOver(int pid) = 0; // ��ʾ��Ϸ������չʾʤ�߻�ƽ��
    virtual void displayPad() = 0; // ��ʾ����. ��һ����ʾ����ʱʹ��.
    virtual void refreshPad(const ChessPiece&) = 0; // ˢ������. ���ı��������.
};

// ��Ϸ��������������Ϸ������󲿷��������. ����μ���Ϸ����.
// �ɵ�������Player��һ��ClassPad�еĺ���.
// �ɵ���������麯���������������.
// ��Ϸ������ֱ�� return. ���Ժ��ټӻ��壩


#endif