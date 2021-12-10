#ifndef __GAME_H__
#define __GAME_H__

#include "Player.h"
#include "ChessPad.h"

class Game {
protected:
    ChessPad *chessPad;
    Player *p[2];
    uint8_t mode;                   // 禁手规则. 0:无禁手; 1:有提示,禁止放置; 2:无提示,直接判负.
public:
    void gameMain();
    
    virtual void infoGameOver(uint8_t pid) = 0; // 提示游戏结束，展示胜者或平局
    virtual void displayPad() = 0; // 显示棋盘. 第一次显示棋盘时使用.
    virtual void refreshPad(ChessPiece) = 0; // 刷新棋盘. 仅改变个别棋子.
};

// 游戏主函数，控制游戏流程与大部分输入输出. 具体参见游戏流程.
// 可调用两个Player与一个ClassPad中的函数.
// 可调用下面的虚函数来进行输入输出.
// 游戏结束后直接 return. （以后再加悔棋）


#endif