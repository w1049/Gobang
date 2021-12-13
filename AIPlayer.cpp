#include <cstring>
#include <iostream>
#include <cmath>
#include "AIPlayer.h"
#include "ChessPiece.h"

enum ChessType { WIN5, ALIVE4, DIE4, LOWDIE4, ALIVE3, TIAO3, DIE3, ALIVE2, LOWALIVE2, DIE2, NOTHREAT };
const int kk[10] = { 5, 4, 4, 4, 3, 3, 3, 2, 2, 2 };

AIPlayer::AIPlayer(uint8_t p = 0) {
    pid = p;
    type = 1;
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
    ChessPiece maxP(pid, 0, 0);
    int maxVal = -1E9-7, tmp;
    getVec(chessPad);
    for (auto p : vec) {
        int a, b;
        tmp = g(pid, chessPad, a, b, p) + 14 - abs(p.getPosX() - 7) - abs(p.getPosY() - 7);
        std::cerr << (int)p.getPosX() << "," << (int)p.getPosY() << ":" << tmp << "|" << a << "vs" << b << std::endl;
        if (tmp > maxVal) {
            maxVal = tmp;
            maxP = p;
        }
    }
    return maxP;
}

const int sc[11] = { 1000000, 100000, 5000, 4000, 1000, 900, 50, 100, 90, 20, 0 };

int AIPlayer::g(uint8_t pid, const ChessPad* chessPad, int &a, int &b, ChessPiece p) {
    int typ1[11] = {}, typ2[11] = {};
    a = f(pid, chessPad, typ1, p), b = f(3 - pid, chessPad, typ2, p);
    int die4 = typ2[DIE4] + typ2[LOWDIE4];
    int alive3 = typ2[ALIVE3] + typ2[TIAO3];
    if (typ2[ALIVE4] || die4 >= 2 || die4 && alive3 || alive3 >= 2) {
        die4 = typ1[DIE4] + typ1[LOWDIE4];
        alive3 = typ1[ALIVE3] + typ1[TIAO3];
        if (die4 >= 2 || die4 && alive3) b += 10000;// 双死4 死4活3
        if (alive3 >= 2) b += 5000;// 双活3
        a -= typ1[ALIVE4] * sc[ALIVE4];
    }
//    std::cerr << "AAA:" << alive3 << std::endl;
    return a - b;
}

int AIPlayer::f(uint8_t pid, const ChessPad* chessPad, int types[11], ChessPiece extra) {
	auto& myp = chessPad->getPiece(pid - 1);
	int typenum[11] = {}, ret = 0, t;
//	uint8_t rec[15][15][4] = {};
	memset(rec, 0, sizeof(rec));
	for (auto p : myp) {
		memset(typenum, 0, sizeof(typenum));
		for (int i = 0; i < 4; i++) {
            t = chessPad->getType(p, i, extra);
			++typenum[t], ++types[t];
		}
		ret += getExScore(typenum);
	}
    // for extra
    if (extra.getPid() == pid) {
        memset(typenum, 0, sizeof(typenum));
        for (int i = 0; i < 4; i++) {
            t = chessPad->getType(extra, i, extra);
            ++typenum[t], ++types[t];
        }
        ret += getExScore(typenum);
    }
    // for extra
    for (int i = 0; i < 10; i++) /*types[i] /= kk[i], */std::cerr << types[i] << " ";
    std::cerr << "\n";
    int die4 = types[DIE4] + types[LOWDIE4];
    int alive3 = types[ALIVE3] + types[TIAO3];
    if (die4 >= 2 || die4 && alive3) ret += 10000;// 双死4 死4活3
    if (alive3 >= 2) ret += 5000;// 双活3
    // 均为必杀
    for (int i = 0; i < 11; i++) ret += types[i] * sc[i];
    return ret;
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