#ifndef __CMD_GAME_H__
#define __CMD_GAME_H__

#include "Game.h"

class CmdGame : public Game {
public:
    void gameInit();
    void infoGameOver(int8_t pid); // 提示游戏结束，展示胜者或平局
    void displayPad(); // 显示棋盘. 第一次显示棋盘时使用.
    void refreshPad(ChessPiece); // 刷新棋盘. 仅改变个别棋子.
    ~CmdGame();
};

#endif