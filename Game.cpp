#include "Game.h"
#include "Player.h"
#include "AIPlayer.h"

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

bool Game::undo() {
    if (chessPad->getPiece().empty()) {
        return 0;
    }
    ChessPiece p = chessPad->getPiece().back();
    turn = p.getPid();
    chessPad->remove();
    displayPad();
    return 1;
}

void Game::start() {
    displayPad();
    while (1) {
        int code = 0;
        if (p[turn - 1]->getType()) {
            info(turn);
            code = p[turn - 1]->command(*chessPad);
            if (code == 1) { // undo
                if (!undo()) continue;
                if (p[turn - 1]->getType() == 0 || p[turn - 1]->getType() == 2 || p[2 - turn]->getType() == 2) if (!undo()) continue;
            }
            else if (code == 2) { // ask
                AIPlayer ai = AIPlayer(turn);
                ChessPiece p = ai.getNextPos(*chessPad);
                reco(p);
            }
        }
        if (code) continue;
        code = step();
        if (code) break;
    }
}

    // virtual infoGameOver(int8_t pid) = 0; // 提示游戏结束，展示胜者或平局
    // virtual infoPlaceFailed(ChessPiece, int8_t reason) = 0; // 提示无法放置棋子
    // virtual displayPad() = 0; // 显示棋盘. 第一次显示棋盘时使用.
    // virtual refreshPad() = 0; // 刷新棋盘. 仅改变个别棋子.


// 游戏主函数，控制游戏流程与大部分输入输出. 具体参见游戏流程.
// 可调用两个Player与一个ClassPad中的函数.
// 可调用下面的虚函数来进行输入输出.
// 游戏结束后直接 return. （以后再加悔棋）