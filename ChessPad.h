#ifndef __CHESS_PAD_H__
#define __CHESS_PAD_H__

#include <vector>

#include "ChessPiece.h"

class ChessPad {
private:
    uint8_t pad[15][15]; // 棋盘
    std::vector<ChessPiece> piece[2]; // 棋子; 下棋顺序
    uint8_t isBanned(ChessPiece) const;
    const static int8_t dx[4], dy[4];
public:
    ChessPad();
    int getType(ChessPiece p, int8_t direc, ChessPiece = { 0, 0, 0 }) const;
    void getLine(ChessPiece p, int8_t direc, uint8_t[9], ChessPiece extra) const;
    int getType(uint8_t[9]) const;
    uint8_t place(ChessPiece);
    // 下棋子. 不再判断是否合法. 调用前请先调用check. 禁手也可强行放置.(由Game类决定是否放置，这里只需实现放置).
    uint8_t check(ChessPiece) const;
    // 判断是否可以放置棋子. 返回值为0代表可以放置，1代表该位置已有棋子，2代表可以放置, 但禁手（暂不区分禁手原因）.
    uint8_t judge(ChessPiece) const;
    // 判断下完该棋子后是否胜利. 返回值为0表示不胜利，1表示胜利，2表示平局.
    uint8_t p(uint8_t, uint8_t) const;
    const std::vector<ChessPiece>& getPiece(int) const;
};

#endif