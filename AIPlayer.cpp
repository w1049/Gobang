#include <cstring>
#include <iostream>
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

ChessPiece AIPlayer::getNextPos(const ChessPad* chessPad) {
    if (chessPad->getPiece(pid - 1).size() == 0 && chessPad->getPiece(2 - pid).size() == 0) return ChessPiece(pid, 7, 7);
    ChessPiece maxP(pid, 0, 0);
//    if (cin.fail() || x < 1 || x > 15 || cy < 0 || cy >= 15) continue;
//    if (reason = pad->check(p = ChessPiece(pid, x-1, cy))) {
//        infoFailed(p, reason);
    int maxVal = -1E9-7, tmp;
    getVec(chessPad);
	chessPad->writePad(pad);
    for (auto p : vec) {
        pad[p.getPosX()][p.getPosY()] = pid;
        tmp = f(pid, chessPad) - f(3 - pid, chessPad);
        std::cerr << (int)p.getPosX() << "," << (int)p.getPosY() << ":" << tmp << std::endl;
        if ((tmp) > maxVal) {
            maxVal = tmp;
            maxP = p;
        }
        pad[p.getPosX()][p.getPosY()] = 0;
    }
    return maxP;
}

int AIPlayer::f(uint8_t pid, const ChessPad* chessPad) {
	auto& myp = chessPad->getPiece(pid - 1);
	int typenum[11] = {}, ret = 0;
	uint8_t rec[15][15][4] = {};
	memset(rec, 0, sizeof(rec));
	for (auto p : myp) {
		memset(typenum, 0, sizeof(typenum));
		for (int i = 0; i < 4; i++) {
			++typenum[getType(p, i)];
		}
		ret += getScores(typenum);
	}
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

	int left, right;//开始和中心线断开的位置
	int colorleft, colorright;//开始和中心线断开的颜色，0或者hid
	int cnt = 1;//中心线有多少个，初始化

	for (int i = 1; i <= 4; i++) {
		if (line[4 - i] == pid)
			cnt++;//同色
		else {
			left = 4 - i;//保存断开位置
			colorleft = line[4 - i];//保存断开颜色
			break;
		}
	}
	for (int i = 1; i <= 4; i++) {
		if (line[4 + i] == pid)
			cnt++;//同色
		else {
			right = 4 + i;//保存断开位置
			colorright = line[4 + i];//保存断开颜色
			break;
		}
	}


	if (cnt >= 5)//中心线5连
		return WIN5;//5连珠

	if (cnt == 4)//中心线4连
	{
		if (colorleft == 0 && colorright == 0)//两边断开位置均空
			return ALIVE4;//活四
		else if (colorleft == hid && colorright == hid)//两边断开位置均非空
			return NOTHREAT;//没有威胁
		else if (colorleft == 0 || colorright == 0)//两边断开位置只有一个空
			return DIE4;//死四
	}

	if (cnt == 3) {//中心线3连
		int colorleft1 = line[left - 1];
		int colorright1 = line[right + 1];

		if (colorleft == 0 && colorright == 0)//两边断开位置均空
		{

			if (colorleft1 == hid && colorright1 == hid)//均为对手棋子
				return DIE3;
			else if (colorleft1 == pid || colorright1 == pid)//只要一个为自己的棋子
				return LOWDIE4;
			else if (colorleft1 == 0 || colorright1 == 0)//只要有一个空
				return ALIVE3;

		}
		else if (colorleft == hid && colorright == hid)//两边断开位置均非空
		{
			return NOTHREAT;//没有威胁
		}
		else if (colorleft == 0 || colorright == 0)//两边断开位置只有一个空
		{

			if (colorleft == hid) {//左边被对方堵住
				if (colorright1 == hid)//右边也被对方堵住
					return NOTHREAT;
				if (colorright1 == 0)//右边均空
					return DIE3;
				if (colorright1 == pid)
					return LOWDIE4;

			}
			if (colorright == hid) {//右边被对方堵住
				if (colorleft1 == hid)//左边也被对方堵住
					return NOTHREAT;
				if (colorleft1 == 0)//左边均空
					return DIE3;
				if (colorleft1 == pid)//左边还有自己的棋子
					return LOWDIE4;
			}
		}
	}

	if (cnt == 2) {//中心线2连
		int colorleft1 = line[left - 1];
		int colorright1 = line[right + 1];
		int colorleft2 = line[left - 2];
		int colorright2 = line[right + 2];

		if (colorleft == 0 && colorright == 0)//两边断开位置均空
		{
			if ((colorright1 == 0 && colorright2 == pid) ||
				(colorleft1 == 0 && colorleft2 == pid))
				return DIE3;//死3
			else if (colorleft1 == 0 && colorright1 == 0)
				return ALIVE2;//活2

			if ((colorright1 == pid && colorright2 == hid) ||
				(colorleft1 == pid && colorleft2 == hid))
				return DIE3;//死3

			if ((colorright1 == pid && colorright2 == pid) ||
				(colorleft1 == pid && colorleft2 == pid))
				return LOWDIE4;//死4

			if ((colorright1 == pid && colorright2 == 0) ||
				(colorleft1 == pid && colorleft2 == 0))
				return TIAO3;//跳活3
			//其他情况在下边返回NOTHREAT
		}
		else if (colorleft == hid && colorright == hid)//两边断开位置均非空
		{
			return NOTHREAT;
		}
		else if (colorleft == 0 || colorright == 0)//两边断开位置只有一个空
		{
			if (colorleft == hid) {//左边被对方堵住
				if (colorright1 == hid || colorright2 == hid) {//只要有对方的一个棋子
					return NOTHREAT;//没有威胁
				}
				else if (colorright1 == 0 && colorright2 == 0) {//均空
					return DIE2;//死2
				}
				else if (colorright1 == pid && colorright2 == pid) {//均为自己的棋子
					return LOWDIE4;//死4
				}
				else if (colorright1 == pid || colorright2 == pid) {//只有一个自己的棋子
					return DIE3;//死3
				}
			}
			if (colorright == hid) {//右边被对方堵住
				if (colorleft1 == hid || colorleft2 == hid) {//只要有对方的一个棋子
					return NOTHREAT;//没有威胁
				}
				else if (colorleft1 == 0 && colorleft2 == 0) {//均空
					return DIE2;//死2
				}
				else if (colorleft1 == pid && colorleft2 == pid) {//均为自己的棋子
					return LOWDIE4;//死4
				}
				else if (colorleft1 == pid || colorleft2 == pid) {//只有一个自己的棋子
					return DIE3;//死3
				}
			}
		}
	}

	if (cnt == 1) {//中心线1连
		int colorleft1 = line[left - 1];
		int colorright1 = line[right + 1];
		int colorleft2 = line[left - 2];
		int colorright2 = line[right + 2];
		int colorleft3 = line[left - 3];
		int colorright3 = line[right + 3];

		if (colorleft == 0 && colorleft1 == pid &&
			colorleft2 == pid && colorleft3 == pid)
			return LOWDIE4;
		if (colorright == 0 && colorright1 == pid &&
			colorright2 == pid && colorright3 == pid)
			return LOWDIE4;

		if (colorleft == 0 && colorleft1 == pid &&
			colorleft2 == pid && colorleft3 == 0 && colorright == 0)
			return TIAO3;
		if (colorright == 0 && colorright1 == pid &&
			colorright2 == pid && colorright3 == 0 && colorleft == 0)
			return TIAO3;

		if (colorleft == 0 && colorleft1 == pid &&
			colorleft2 == pid && colorleft3 == hid && colorright == 0)
			return DIE3;
		if (colorright == 0 && colorright1 == pid &&
			colorright2 == pid && colorright3 == hid && colorleft == 0)
			return DIE3;

		if (colorleft == 0 && colorleft1 == 0 &&
			colorleft2 == pid && colorleft3 == pid)
			return DIE3;
		if (colorright == 0 && colorright1 == 0 &&
			colorright2 == pid && colorright3 == pid)
			return DIE3;

		if (colorleft == 0 && colorleft1 == pid &&
			colorleft2 == 0 && colorleft3 == pid)
			return DIE3;
		if (colorright == 0 && colorright1 == pid &&
			colorright2 == 0 && colorright3 == pid)
			return DIE3;

		if (colorleft == 0 && colorleft1 == pid &&
			colorleft2 == 0 && colorleft3 == 0 && colorright == 0)
			return LOWALIVE2;
		if (colorright == 0 && colorright1 == pid &&
			colorright2 == 0 && colorright3 == 0 && colorleft == 0)
			return LOWALIVE2;

		if (colorleft == 0 && colorleft1 == 0 &&
			colorleft2 == pid && colorleft3 == 0 && colorright == 0)
			return LOWALIVE2;
		if (colorright == 0 && colorright1 == 0 &&
			colorright2 == pid && colorright3 == 0 && colorleft == 0)
			return LOWALIVE2;

		//其余在下边返回没有威胁

	}
	return NOTHREAT;//返回没有威胁
}

int AIPlayer::getScores(int typenum[]) {
	int die4 = typenum[DIE4] + typenum[LOWDIE4];
	int alive3 = typenum[ALIVE3] + typenum[TIAO3];
	int alive2 = typenum[ALIVE2] + typenum[LOWALIVE2];

	if (typenum[WIN5] >= 1) return 100000;//赢5
	if (typenum[ALIVE4] >= 1 || die4 >= 2 || die4 >= 1 && alive3 >= 1) return 10000;//活4 双死4 死4活3
	if (alive3 >= 2) return 5000;//双活3
	if (typenum[DIE3] >= 1 && typenum[ALIVE3] >= 1) return 1000;//死3高级活3
	if (typenum[DIE3] >= 1) return 500;//高级死4
	if (typenum[LOWDIE4] >= 1) return 400;//低级死4
	if (typenum[ALIVE3] >= 1) return 100;//单活3
	if (typenum[TIAO3] >= 1) return 90;//跳活3
	if (alive2 >= 2) return 50;//双活2
	if (typenum[ALIVE2] >= 1) return 10;//活2
	if (typenum[LOWALIVE2] >= 1) return 9;//低级活2
	if (typenum[DIE3] >= 1) return 5;//死3
	if (typenum[DIE2] >= 1) return 2;//死2
	return 1;//没有威胁
}