#ifndef __CHESS_PAD_H__
#define __CHESS_PAD_H__

#include "ChessPiece.h"
using std::array;

class ChessPad {
private:
    array<array<int8_t, 15>, 15> pad{};     // 棋盘
    cpv PList;                              // 下子顺序
    int isBanned(const ChessPiece&) const;  // 判断是否禁手
    const static int8_t dx[4], dy[4];       // 四个方向
    bool mode;                              // 禁手模式 0为不禁手
public:
    ChessPad(bool);  // 是否禁手
    // 获取棋子（不必真的下子）在某方向的类型. bool 表示是否判断长连.
    // 如果是长连，返回 INF
    int getType(const ChessPiece&, int direc, bool = 0) const;
    // 获取以棋子（不必真的下子）为中心的一条线，存入数组中
    void getLine(const ChessPiece&, int direc, int8_t[9]) const;
    // 根据线判断中心棋子的类型. 如果是长连，中心棋子将被置为0
    int getType(int8_t[9]) const;
    // 下棋子，不论是否合法
    int place(const ChessPiece&);
    // 移除棋子
    void remove();
    // 判断是否可以放置棋子.
    // 返回值为0代表可以放置，1代表该位置已有棋子，2代表出界，（开启禁手时）3 4
    // 5代表可以放置，但禁手
    int checkState(const ChessPiece&) const;
    // 判断下完该棋子后是否胜利（该棋子已下）. 返回值为0表示不胜利，1表示胜利
    bool judgeWinner(const ChessPiece&) const;
    int getByPos(int8_t, int8_t) const;
    const cpv& getPiecesList() const;
};

#endif
