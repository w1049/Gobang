#include "ChessPad.h"

const int8_t ChessPad::dx[4] = {0, 1, 1, 1}, ChessPad::dy[4] = {1, 0, 1, -1};

int ChessPad::getByPos(int8_t i, int8_t j) const { return pad[i][j]; }

const cpv &ChessPad::getPiecesList() const { return PList; }

ChessPad::ChessPad(bool mode) : mode(mode) {}

int ChessPad::place(const ChessPiece &p) {
    pad[p.getX()][p.getY()] = p.getPid();
    PList.push_back(p);
    return 0;
}

int ChessPad::isBanned(const ChessPiece &p) const {
    bool f = 0;  // 长连
    // 白棋无禁手
    if (p.getPid() == 2) return 0;
    // 记录下棋后将会形成的旗形
    int typ[11] = {};
    for (int i = 0, t; i < 4; i++) {
        t = getType(p, i, 1);
        if (t == INF)
            f = 1;  // 长连
        else
            ++typ[t];
    }
    if (f && !typ[WIN5]) return 3;  // 长连禁手. 如果同时成5，禁手失效
    if (typ[ALIVE3] + typ[TIAO3] >= 2) return 4;  // 33禁手
    if (typ[DIE4] + typ[LOWDIE4] >= 2) return 5;  // 44禁手
    return 0;
}

int ChessPad::checkState(const ChessPiece &p) const {
    int8_t x = p.getX(), y = p.getY();
    if (x < 0 || x >= 15 || y < 0 || y >= 15) return 2;  // 出界
    if (pad[x][y]) return 1;                             // 重叠
    if (mode) return isBanned(p);                        // 禁手
    return 0;
}

bool ChessPad::judgeWinner(const ChessPiece &p) const {
    // 任一方向成5就赢
    for (int i = 0; i < 4; i++)
        if (getType(p, i) == WIN5) return 1;
    return 0;
}

void ChessPad::remove() {
    ChessPiece p = PList.back();
    PList.pop_back();
    pad[p.getX()][p.getY()] = 0;
}

int ChessPad::getType(const ChessPiece &p, int direc, bool judgeLong) const {
    int8_t line[9] = {};
    // 先获取一条线
    getLine(p, direc, line);
    // 判断类型
    int ret = getType(line);
    // 判断长连
    if (judgeLong && !line[4]) return INF;
    return ret;
}

void ChessPad::getLine(const ChessPiece &p, int direc, int8_t line[9]) const {
    int pid = p.getPid();
    line[4] = pid;  // 中间直接放上 pid
    int8_t dx = ChessPad::dx[direc], dy = ChessPad::dy[direc];

    // 正向找4个
    int8_t x = p.getX(), y = p.getY();
    x += dx, y += dy;
    for (int8_t c = 5; c < 9; c++)
        if (x >= 0 && x < 15 && y >= 0 && y < 15)
            line[c] = pad[x][y], x += dx, y += dy;
        else
            line[c] = 3 - pid;

    // 逆向找4个
    x = p.getX(), y = p.getY();
    x -= dx, y -= dy;
    for (int8_t c = 3; c >= 0; c--)
        if (x >= 0 && x < 15 && y >= 0 && y < 15)
            line[c] = pad[x][y], x -= dx, y -= dy;
        else
            line[c] = 3 - pid;
}

// 旗形判断参考：https://www.cnblogs.com/songdechiu/p/5768999.html
int ChessPad::getType(int8_t line[9]) const {
    int pid = line[4];
    int hid = 3 - pid;
    int l, r;     // 开始和中心线断开的位置
    int cnt = 1;  // 中心线有多少个，初始化
    for (int i = 1; i <= 4; i++)
        if (line[4 - i] == pid)
            cnt++;
        else {
            l = 4 - i;  // 保存断开位置
            break;
        }
    for (int i = 1; i <= 4; i++)
        if (line[4 + i] == pid)
            cnt++;
        else {
            r = 4 + i;  // 保存断开位置
            break;
        }
    if (cnt > 5) {
        line[4] = 0;  // 长连
        return WIN5;
    }
    if (cnt == 5) return WIN5;
    int cl = line[l], cr = line[r];       // cl := (cl == hid)
    if (cnt == 4) {                       // 中心线4连
        if (!cl && !cr)                   // 两边断开位置均空
            return ALIVE4;                // 活四
        else if (cl == hid && cr == hid)  // 两边断开位置均非空
            return NOTHREAT;              // 没有威胁
        else if (!cl || !cr)              // 两边断开位置只有一个空
            return DIE4;                  // 死四
    }
    if (cnt == 3) {        // 中心线3连
        if (!cl && !cr) {  // 两边断开位置均空
            if (line[l - 1] == hid && line[r + 1] == hid)  // 均为对手棋子
                return DIE3;
            else if (line[l - 1] == pid ||
                     line[r + 1] == pid)  // 只要一个为自己的棋子
                return LOWDIE4;
            else if (!line[l - 1] || !line[r + 1])  // 只要有一个空
                return ALIVE3;
        } else if (cl == hid && cr == hid) {  // 两边断开位置均非空
            return NOTHREAT;                  // 没有威胁
        } else if (!cl || !cr) {         // 两边断开位置只有一个空
            if (cl == hid) {             // 左边被对方堵住
                if (line[r + 1] == hid)  // 右边也被对方堵住
                    return NOTHREAT;
                if (!line[r + 1])  // 右边均空
                    return DIE3;
                if (line[r + 1] == pid) return LOWDIE4;
            }
            if (cr == hid) {             // 右边被对方堵住
                if (line[l - 1] == hid)  // 左边也被对方堵住
                    return NOTHREAT;
                if (!line[l - 1])  // 左边均空
                    return DIE3;
                if (line[l - 1] == pid)  // 左边还有自己的棋子
                    return LOWDIE4;
            }
        }
    }
    if (cnt == 2) {        // 中心线2连
        if (!cl && !cr) {  // 两边断开位置均空
            if ((!line[r + 1] && line[r + 2] == pid) ||
                (!line[l - 1] && line[l - 2] == pid))
                return DIE3;  // 死3
            else if (!line[l - 1] && !line[r + 1])
                return ALIVE2;  // 活2
            if ((line[r + 1] == pid && line[r + 2] == hid) ||
                (line[l - 1] == pid && line[l - 2] == hid))
                return DIE3;  // 死3
            if ((line[r + 1] == pid && line[r + 2] == pid) ||
                (line[l - 1] == pid && line[l - 2] == pid))
                return LOWDIE4;  // 死4
            if ((line[r + 1] == pid && !line[r + 2]) ||
                (line[l - 1] == pid && !line[l - 2]))
                return TIAO3;  // 跳活3
                               // 其他情况在下边返回 NOTHREAT
            if ((!line[l - 1] && line[r + 1] == hid) ||
                (line[l - 1] == hid && !line[r + 1]))
                return LOWALIVE2;
        } else if (cl == hid && cr == hid) {  // 两边断开位置均非空
            return NOTHREAT;
        } else if (!cl || !cr) {  // 两边断开位置只有一个空
            if (cl == hid) {      // 左边被对方堵住
                if (line[r + 1] == hid ||
                    line[r + 2] == hid) {  // 只要有对方的一个棋子
                    return NOTHREAT;       // 没有威胁
                } else if (!line[r + 1] && !line[r + 2]) {  // 均空
                    return DIE2;                            // 死2
                } else if (line[r + 1] == pid &&
                           line[r + 2] == pid) {  // 均为自己的棋子
                    return LOWDIE4;               // 死4
                } else if (line[r + 1] == pid ||
                           line[r + 2] == pid) {  // 只有一个自己的棋子
                    return DIE3;                  // 死3
                }
            }
            if (cr == hid) {  // 右边被对方堵住
                if (line[l - 1] == hid ||
                    line[l - 2] == hid) {  // 只要有对方的一个棋子
                    return NOTHREAT;       // 没有威胁
                } else if (!line[l - 1] && !line[l - 2]) {  // 均空
                    return DIE2;                            // 死2
                } else if (line[l - 1] == pid &&
                           line[l - 2] == pid) {  // 均为自己的棋子
                    return LOWDIE4;               // 死4
                } else if (line[l - 1] == pid ||
                           line[l - 2] == pid) {  // 只有一个自己的棋子
                    return DIE3;                  // 死3
                }
            }
        }
    }
    if (cnt == 1) {  // 中心线1连
        if (!cl && line[l - 1] == pid && line[l - 2] == pid &&
            line[l - 3] == pid)
            return LOWDIE4;
        if (!cr && line[r + 1] == pid && line[r + 2] == pid &&
            line[r + 3] == pid)
            return LOWDIE4;
        if (!cl && line[l - 1] == pid && line[l - 2] == pid && !line[l - 3] &&
            !cr)
            return TIAO3;
        if (!cr && line[r + 1] == pid && line[r + 2] == pid && !line[r + 3] &&
            !cl)
            return TIAO3;
        if (!cl && line[l - 1] == pid && line[l - 2] == pid &&
            line[l - 3] == hid && !cr)
            return DIE3;
        if (!cr && line[r + 1] == pid && line[r + 2] == pid &&
            line[r + 3] == hid && !cl)
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
        // 其余在下边返回没有威胁
    }
    return NOTHREAT;  // 返回没有威胁
}
