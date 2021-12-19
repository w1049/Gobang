#include "Game.h"
#include "Player.h"

int Game::step() {
    ChessPiece tmp = p[turn - 1]->getNextPos(*chessPad);
    chessPad->place(tmp);
    refreshPad(tmp);
    int code = chessPad->judge(tmp);
    if (code) {
        infoGameOver(turn);
        return 1;
    }
    turn = 3 - turn;
    return 0;
}
    
    // virtual infoGameOver(int8_t pid) = 0; // 提示游戏结束，展示胜者或平局
    // virtual infoPlaceFailed(ChessPiece, int8_t reason) = 0; // 提示无法放置棋子
    // virtual displayPad() = 0; // 显示棋盘. 第一次显示棋盘时使用.
    // virtual refreshPad() = 0; // 刷新棋盘. 仅改变个别棋子.


// 游戏主函数，控制游戏流程与大部分输入输出. 具体参见游戏流程.
// 可调用两个Player与一个ClassPad中的函数.
// 可调用下面的虚函数来进行输入输出.
// 游戏结束后直接 return. （以后再加悔棋）