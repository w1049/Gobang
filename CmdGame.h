#ifndef __CMD_GAME_H__
#define __CMD_GAME_H__

#include "Game.h"

class CmdGame : public Game {
public:
    CmdGame(int, int);
    void start();
    void info(int);
    void infoGameOver(int pid); // ��ʾ��Ϸ������չʾʤ�߻�ƽ��
    void displayPad(); // ��ʾ����. ��һ����ʾ����ʱʹ��.
    void refreshPad(const ChessPiece&); // ˢ������. ���ı��������.
    ~CmdGame();
};

#endif