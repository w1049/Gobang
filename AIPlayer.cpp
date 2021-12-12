#include <cstring>
#include <iostream>
#include <cmath>
#include "AIPlayer.h"
#include "ChessPiece.h"

enum ChessType { WIN5, ALIVE4, DIE4, LOWDIE4, ALIVE3, TIAO3, DIE3, ALIVE2, LOWALIVE2, DIE2, NOTHREAT };

AIPlayer::AIPlayer(uint8_t p = 0) {
    pid = p;
    type = 1;
    memset(pad, 0, sizeof(pad));
}
void AIPlayer::getVec(const ChessPad* chessPad) {
    ChessPiece p;
    vec.clear();
    for (uint8_t x = 0; x < 15; x++)
        for (uint8_t y = 0; y < 15; y++)
            if (!chessPad->check(p.set(pid, x, y))) vec.push_back(p);
}

int types[11];

ChessPiece AIPlayer::getNextPos(const ChessPad* chessPad) {
    if (chessPad->getPiece(pid - 1).size() == 0 && chessPad->getPiece(2 - pid).size() == 0) return ChessPiece(pid, 7, 7);
    ChessPiece maxP(pid, 0, 0);
    int maxVal = -1E9-7, tmp;
    getVec(chessPad);
	chessPad->writePad(pad);
    for (auto p : vec) {
        pad[p.getPosX()][p.getPosY()] = pid;
        int a, b;
        tmp = g(pid, chessPad, a, b);
        std::cerr << (int)p.getPosX() << "," << (int)p.getPosY() << ":" << tmp << "|" << a << "vs" << b << std::endl;
        if (tmp > maxVal) {
            maxVal = tmp;
            maxP = p;
        } else if (tmp == maxVal) {
            int a = abs(p.getPosX() - 7) + abs(p.getPosY() - 7);
            int b = abs(maxP.getPosX() - 7) + abs(maxP.getPosY() - 7);
            if (b < a) {
                maxVal = tmp;
                maxP = p;
            }
        }
        pad[p.getPosX()][p.getPosY()] = 0;
    }
    return maxP;
}

const int sc[11] = { 100000, 10000, 500, 400, 100, 90, 5, 10, 9, 2, 0 };

int AIPlayer::g(uint8_t pid, const ChessPad* chessPad, int &a, int &b) {
    int typ1[11] = {}, typ2[11] = {};
    a = f(pid, chessPad, typ1), b = f(3 - pid, chessPad, typ2);
    int ret = a - b;
    int die4 = typ2[DIE4] + typ2[LOWDIE4];
    int alive3 = typ2[ALIVE3] + typ2[TIAO3];
    if (typ2[ALIVE4] || die4 >= 2 || die4 && alive3 || alive3 >= 2) {
        die4 = typ1[DIE4] + typ1[LOWDIE4];
        alive3 = typ1[ALIVE3] + typ1[TIAO3];
        if (die4 >= 2 || die4 && alive3) ret -= 10000;// 双死4 死4活3
        if (alive3 >= 2) ret -= 5000;// 双活3
        ret -= typ1[ALIVE4] * sc[ALIVE4];
    }
    return ret;
}

int AIPlayer::f(uint8_t pid, const ChessPad* chessPad, int types[11]) {
	auto& myp = chessPad->getPiece(pid - 1);
	int typenum[11] = {}, ret = 0, t;
	uint8_t rec[15][15][4] = {};
	memset(rec, 0, sizeof(rec));
	for (auto p : myp) {
		memset(typenum, 0, sizeof(typenum));
		for (int i = 0; i < 4; i++) {
            t = getType(p, i);
			++typenum[t], ++types[t];
		}
		ret += getExScore(typenum);
	}
    int die4 = types[DIE4] + types[LOWDIE4];
    int alive3 = types[ALIVE3] + types[TIAO3];
    if (die4 >= 2 || die4 && alive3) ret += 10000;// 双死4 死4活3
    if (alive3 >= 2) ret += 5000;// 双活3
    // 均为必杀
    for (int i = 0; i < 11; i++) ret += types[i] * sc[i];
    return ret;
}

int AIPlayer::getType(ChessPiece p, int8_t direc) {
	getLine(p, direc);
	if (rec[p.getPosX()][p.getPosY()][direc]) return rec[p.getPosX()][p.getPosY()][direc];
	else return getType();
}

void AIPlayer::getLine(ChessPiece p, int8_t direc) {
    switch (direc) {
    case 0: getLine(p, 1, 0); break;
    case 1: getLine(p, 0, 1); break;
    case 2: getLine(p, 1, 1); break;
    case 3: getLine(p, 1, -1); break;
    }
}

void AIPlayer::getLine(ChessPiece p, int8_t dx, int8_t dy) {
    int8_t x = p.getPosX(), y = p.getPosY();
    uint8_t pid = p.getPid(), cnt = 1;
    line[4] = pad[x][y];
    x += dx, y += dy;
    for (int8_t c = 5; c < 9; c++)
        if (x >= 0 && x < 15 && y >= 0 && y < 15)
            line[c] = pad[x][y], x += dx, y += dy;
        else line[c] = 3 - pid;
    x = p.getPosX(), y = p.getPosY();
    x -= dx, y -= dy;
    for (int8_t c = 3; c >= 0; c--)
        if (x >= 0 && x < 15 && y >= 0 && y < 15)
            line[c] = pad[x][y], x -= dx, y -= dy;
        else line[c] = 3 - pid;
}

// 旗形判断参考：https://www.cnblogs.com/songdechiu/p/5768999.html

int AIPlayer::getType() {
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

int AIPlayer::getExScore(int typenum[]) {
	int die4 = typenum[DIE4] + typenum[LOWDIE4];
	int alive3 = typenum[ALIVE3] + typenum[TIAO3];
	int alive2 = typenum[ALIVE2] + typenum[LOWALIVE2];
    if (alive2 >= 2) return 50;//双活2
    //if (die4 >= 2 || die4 && alive3) return 10000;//双死4 死4活3
    //if (alive3 >= 2) return 5000;//双活3
    //if (typenum[DIE3] && typenum[ALIVE3]) return 2000;//死3高级活3
    return 0;
}