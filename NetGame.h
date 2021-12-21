#ifndef __NET_GAME_H__
#define __NET_GAME_H__

#include "Game.h"
#include "NetPlayer.h"

class NetGame : public Game {
public:
    NetGame(NetPlayer*, int);
    void info(int);
    void reco(const ChessPiece&);
    void infoGameOver(int pid); // 提示游戏结束，展示胜者或平局
    void displayPad(); // 显示棋盘. 第一次显示棋盘时使用.
    void refreshPad(const ChessPiece&); // 刷新棋盘. 仅改变个别棋子.
    ~NetGame();
};

#endif