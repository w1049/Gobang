#include <stack>
#include <cstring>

#include "ChessPad.h"

enum ChessType { WIN5, ALIVE4, DIE4, LOWDIE4, ALIVE3, TIAO3, DIE3, ALIVE2, LOWALIVE2, DIE2, NOTHREAT };
const int8_t ChessPad::dx[4] = { 0, 1, 1, 1 }, ChessPad::dy[4] = { 1, 0, 1, -1 };
// class ChessPad {
// private:
//     uint8_t pad[15][15]; // 棋盘
//     std::stack<ChessPiece> piece[2]; // 棋子; 下棋顺序

// 下棋子. 不再判断是否合法. 调用前请先调用check. 禁手也可强行放置.(由Game类决定是否放置，这里只需实现放置).
uint8_t ChessPad::place(ChessPiece p) {
    pad[p.getPosX()][p.getPosY()] = p.getPid();
    //pid=1先手,pid=2后手.
    piece[p.getPid()-1].push_back(p);
    return 0;
}

// 是否禁手
uint8_t ChessPad::isBanned(ChessPiece p) const {
    if (p.getPid() == 2) return 0;
    // 判断禁手并返回1
    return 0;
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
    for (int i = 0; i < 4; i++)
        if (getType(p, i) == WIN5) return 1;
    return 0;
}

uint8_t ChessPad::p(uint8_t i, uint8_t j) const {
    return pad[i][j];
}

void ChessPad::writePad(uint8_t p[15][15]) const {
    memcpy(p, pad, sizeof(pad));
}

const std::vector<ChessPiece>& ChessPad::getPiece(int i) const {
    return piece[i];
}

ChessPad::ChessPad() {
    memset(pad, 0, sizeof(pad));
    piece[0].clear();
    piece[1].clear();
}

int ChessPad::getType(ChessPiece p, int8_t direc, ChessPiece extra) const {
    // if (rec[p.getPosX()][p.getPosY()][direc]) return rec[p.getPosX()][p.getPosY()][direc];
    uint8_t line[9] = {};
    getLine(p, direc, line, extra);
    return getType(line);
}

void ChessPad::getLine(ChessPiece p, int8_t direc, uint8_t line[9], ChessPiece extra) const {
    int8_t dx = ChessPad::dx[direc], dy = ChessPad::dy[direc];
    int8_t x = p.getPosX(), y = p.getPosY();
    uint8_t pid = p.getPid(), cnt = 1;
    line[4] = pid;
    x += dx, y += dy;
    for (int8_t c = 5; c < 9; c++)
        if (x >= 0 && x < 15 && y >= 0 && y < 15) {
            if (x == extra.getPosX() && y == extra.getPosY()) line[c] = extra.getPid();
            else line[c] = pad[x][y];
            x += dx, y += dy;
        }
        else line[c] = 3 - pid;
    x = p.getPosX(), y = p.getPosY();
    x -= dx, y -= dy;
    for (int8_t c = 3; c >= 0; c--)
        if (x >= 0 && x < 15 && y >= 0 && y < 15) {
            if (x == extra.getPosX() && y == extra.getPosY()) line[c] = extra.getPid();
            else line[c] = pad[x][y];
            x -= dx, y -= dy;
        }
        else line[c] = 3 - pid;
}

// 旗形判断参考：https://www.cnblogs.com/songdechiu/p/5768999.html

int ChessPad::getType(uint8_t line[9]) const {
    int pid = line[4];
    int hid = 3 - pid;
    int l, r;     //开始和中心线断开的位置
    int cnt = 1;  //中心线有多少个，初始化
    for (int i = 1; i <= 4; i++)
        if (line[4 - i] == pid) cnt++;
        else {
            l = 4 - i;  //保存断开位置
            break;
        }
    for (int i = 1; i <= 4; i++)
        if (line[4 + i] == pid) cnt++;
        else {
            r = 4 + i;  //保存断开位置
            break;
        }
    if (cnt > 4) return WIN5;
    int cl = line[l], cr = line[r];  // cl := (cl == hid)
    if (cnt == 4) {                       //中心线4连
        if (!cl && !cr)                   //两边断开位置均空
            return ALIVE4;                //活四
        else if (cl == hid && cr == hid)  //两边断开位置均非空
            return NOTHREAT;              //没有威胁
        else if (!cl || !cr)              //两边断开位置只有一个空
            return DIE4;                  //死四
    }
    if (cnt == 3) {        //中心线3连
        if (!cl && !cr) {  //两边断开位置均空
            if (line[l - 1] == hid && line[r + 1] == hid)  //均为对手棋子
                return DIE3;
            else if (line[l - 1] == pid || line[r + 1] == pid)  //只要一个为自己的棋子
                return LOWDIE4;
            else if (!line[l - 1] || !line[r + 1])  //只要有一个空
                return ALIVE3;
        }
        else if (cl == hid && cr == hid) {  //两边断开位置均非空
            return NOTHREAT;                  //没有威胁
        }
        else if (!cl || !cr) {         //两边断开位置只有一个空
            if (cl == hid) {             //左边被对方堵住
                if (line[r + 1] == hid)  //右边也被对方堵住
                    return NOTHREAT;
                if (!line[r + 1])  //右边均空
                    return DIE3;
                if (line[r + 1] == pid) return LOWDIE4;
            }
            if (cr == hid) {             //右边被对方堵住
                if (line[l - 1] == hid)  //左边也被对方堵住
                    return NOTHREAT;
                if (!line[l - 1])  //左边均空
                    return DIE3;
                if (line[l - 1] == pid)  //左边还有自己的棋子
                    return LOWDIE4;
            }
        }
    }
    if (cnt == 2) {        //中心线2连
        if (!cl && !cr) {  //两边断开位置均空
            if (!line[r + 1] && line[r + 2] == pid || !line[l - 1] && line[l - 2] == pid)
                return DIE3;  //死3
            else if (!line[l - 1] && !line[r + 1])
                return ALIVE2;  //活2
            if (line[r + 1] == pid && line[r + 2] == hid || line[l - 1] == pid && line[l - 2] == hid)
                return DIE3;  //死3
            if (line[r + 1] == pid && line[r + 2] == pid || line[l - 1] == pid && line[l - 2] == pid)
                return LOWDIE4;  //死4
            if (line[r + 1] == pid && !line[r + 2] || line[l - 1] == pid && !line[l - 2])
                return TIAO3;  //跳活3
                               //其他情况在下边返回NOTHREAT
            if (!line[l - 1] && line[r + 1] == hid || line[l - 1] == hid && !line[r + 1])
                return LOWALIVE2;
        }
        else if (cl == hid && cr == hid) {  //两边断开位置均非空
            return NOTHREAT;
        }
        else if (!cl || !cr) {  //两边断开位置只有一个空
            if (cl == hid) {      //左边被对方堵住
                if (line[r + 1] == hid || line[r + 2] == hid) {  //只要有对方的一个棋子
                    return NOTHREAT;       //没有威胁
                }
                else if (!line[r + 1] && !line[r + 2]) {  //均空
                    return DIE2;                            //死2
                }
                else if (line[r + 1] == pid && line[r + 2] == pid) {  //均为自己的棋子
                    return LOWDIE4;               //死4
                }
                else if (line[r + 1] == pid || line[r + 2] == pid) {  //只有一个自己的棋子
                    return DIE3;                  //死3
                }
            }
            if (cr == hid) {  //右边被对方堵住
                if (line[l - 1] == hid || line[l - 2] == hid) {  //只要有对方的一个棋子
                    return NOTHREAT;       //没有威胁
                }
                else if (!line[l - 1] && !line[l - 2]) {  //均空
                    return DIE2;                            //死2
                }
                else if (line[l - 1] == pid && line[l - 2] == pid) {  //均为自己的棋子
                    return LOWDIE4;               //死4
                }
                else if (line[l - 1] == pid || line[l - 2] == pid) {  //只有一个自己的棋子
                    return DIE3;                  //死3
                }
            }
        }
    }
    if (cnt == 1) {  //中心线1连
        if (!cl && line[l - 1] == pid && line[l - 2] == pid && line[l - 3] == pid)
            return LOWDIE4;
        if (!cr && line[r + 1] == pid && line[r + 2] == pid && line[r + 3] == pid)
            return LOWDIE4;
        if (!cl && line[l - 1] == pid && line[l - 2] == pid && !line[l - 3] && !cr)
            return TIAO3;
        if (!cr && line[r + 1] == pid && line[r + 2] == pid && !line[r + 3] && !cl)
            return TIAO3;
        if (!cl && line[l - 1] == pid && line[l - 2] == pid && line[l - 3] == hid && !cr)
            return DIE3;
        if (!cr && line[r + 1] == pid && line[r + 2] == pid && line[r + 3] == hid && !cl)
            return DIE3;
        if (!cl && !line[l - 1] && line[l - 2] == pid && line[l - 3] == pid)
            return DIE3;
        if (!cr && !line[r + 1] && line[r + 2] == pid && line[r + 3] == pid)
            return DIE3;
        if (!cl && line[l - 1] == pid && !line[l - 2] && line[l - 3] == pid)
            return DIE3;
        if (!cr && line[r + 1] == pid && !line[r + 2] && line[r + 3] == pid)
            return DIE3;
        if (!cl && line[l - 1] == pid && !line[l - 2] && !line[l - 3] && !cr)
            return LOWALIVE2;
        if (!cr && line[r + 1] == pid && !line[r + 2] && !line[r + 3] && !cl)
            return LOWALIVE2;
        if (!cl && !line[l - 1] && line[l - 2] == pid && !line[l - 3] && !cr)
            return LOWALIVE2;
        if (!cr && !line[r + 1] && line[r + 2] == pid && !line[r + 3] && !cl)
            return LOWALIVE2;
        //其余在下边返回没有威胁
    }
    return NOTHREAT;  //返回没有威胁
}
