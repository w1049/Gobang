#ifndef __CHESS_PAD_H__
#define __CHESS_PAD_H__

#include <vector>

#include "ChessPiece.h"

class ChessPad {
private:
    int8_t pad[15][15]; // 棋盘
    // cpv piece[2]; // 棋子; 下棋顺序
    cpv list;
    int isBanned(const ChessPiece&) const;
    const static int8_t dx[4], dy[4];
    int8_t mode; // 禁手模式 0不禁
public:
    ChessPad(int);
    ChessPad(const ChessPad&);
    int getType(const ChessPiece&, int direc, int = 0) const;
    void getLine(const ChessPiece&, int direc, int8_t[9]) const;
    int getType(int8_t[9]) const;
    int place(const ChessPiece&);
    // 下棋子. 不再判断是否合法. 调用前请先调用check. 禁手也可强行放置.(由Game类决定是否放置，这里只需实现放置).
    int check(const ChessPiece&) const;
    // 判断是否可以放置棋子. 返回值为0代表可以放置，1代表该位置已有棋子，2代表可以放置, 但禁手（暂不区分禁手原因）.
    int judge(const ChessPiece&) const;
    // 判断下完该棋子后是否胜利. 返回值为0表示不胜利，1表示胜利，2表示平局.
    int p(int8_t, int8_t) const;
    const cpv& getPiece() const;
    void remove();
    int getMode();
};

#endif