#ifndef __NET_GAME_H__
#define __NET_GAME_H__

#include "Game.h"
#include "NetPlayer.h"

class NetGame : public Game {
public:
    NetGame(NetPlayer*, int);
    void info(int);
    void reco(const ChessPiece&);
    void infoGameOver(int pid); // ��ʾ��Ϸ������չʾʤ�߻�ƽ��
    void displayPad(); // ��ʾ����. ��һ����ʾ����ʱʹ��.
    void refreshPad(const ChessPiece&); // ˢ������. ���ı��������.
    ~NetGame();
};

#endif