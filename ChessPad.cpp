#include <stack>
#include <cstring>

#include "ChessPad.h"

// class ChessPad {
// private:
//     uint8_t pad[15][15]; // 棋盘
//     std::stack<ChessPiece> piece[2]; // 棋子; 下棋顺序

// 下棋子. 不再判断是否合法. 调用前请先调用check. 禁手也可强行放置.(由Game类决定是否放置，这里只需实现放置).
uint8_t ChessPad::place(ChessPiece p) {
    pad[p.getPosX()][p.getPosY()] = p.getPid();
    //pid=1先手,pid=2后手.
    piece[p.getPid()-1].push(p);
    return 0;
}

// 是否禁手
uint8_t ChessPad::isBanned(ChessPiece p) const {
    if (p.getPid() == 2) return 0;
    // 判断禁手并返回1
    return 0;
}

uint8_t ChessPad::check45(ChessPiece p, int8_t dx, int8_t dy) const {
    int8_t x = p.getPosX(), y = p.getPosY();
    uint8_t pid = p.getPid(), cnt = 1;
    x += dx, y += dy;
    while (x >= 0 && x < 15 && y >= 0 && y < 15) {
        if (pad[x][y] == pid) cnt++, x += dx, y += dy;
        else break;
    }
    x = p.getPosX(), y = p.getPosY();
    x -= dx, y -= dy;
    while (x >= 0 && x < 15 && y >= 0 && y < 15) {
        if (pad[x][y] == pid) cnt++, x -= dx, y -= dy;
        else break;
    }
    return cnt > 4;
}

// 判断是否可以放置棋子. 返回值为0代表可以放置，1代表该位置已有棋子，2代表可以放置, 但禁手（暂不区分禁手原因），3代表出界.
uint8_t ChessPad::check(ChessPiece p) const {
    uint8_t x = p.getPosX(), y = p.getPosY();
    if (x < 0 || x >= 15 || y < 0 || y >= 15) return 3;
    if (pad[x][y]) return 1;
    if (isBanned(p)) return 2;
    return 0;
}

// 判断下完该棋子后是否胜利.（该棋子已下）返回值为0表示不胜利，1表示胜利，2表示平局.
uint8_t ChessPad::judge(ChessPiece p) const {
    if (check45(p, 1, 0) || check45(p, 0, 1) || check45(p, 1, 1) || check45(p, 1, -1)) return 1;
    return 0;
}

uint8_t ChessPad::p(uint8_t i, uint8_t j) const {
    return pad[i][j];
}

ChessPad::ChessPad() {
    memset(pad, 0, sizeof(pad));
}