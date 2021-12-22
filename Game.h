#ifndef __GAME_H__
#define __GAME_H__

#include "ChessPad.h"
#include "Player.h"

class Game {
protected:
    Player *p[2];
    ChessPad *chessPad;
    int turn = 1;

public:
    Game() = default;
    // 不允许复制
    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;
    bool step(bool = 0);  // 棋局前进一步，bool = 1 表示直接判负
    void start();         // 棋局开始，主体函数
    bool undo();          // 悔一步棋
    virtual void infoRecommend(const ChessPiece &) = 0;  // 推荐棋子
    virtual void infoTips(int) = 0;                      // 显示局势分等
    virtual void infoGameOver(
        int pid) = 0;  // 提示游戏结束，展示胜者. 若 pid = 3，则平局
    virtual void infoPlace(const ChessPiece &) = 0;  // 刷新棋盘，添加棋子
    virtual void infoRemove();  // 刷新棋盘，移除棋子
    virtual ~Game();
};

#endif
