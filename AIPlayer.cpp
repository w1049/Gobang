#include <cstring>
#include <iostream>
#include <cmath>
#include "AIPlayer.h"
#include "ChessPiece.h"

enum ChessType { WIN5, ALIVE4, DIE4, LOWDIE4, ALIVE3, TIAO3, DIE3, ALIVE2, LOWALIVE2, DIE2, NOTHREAT };
const int sc[11] = { 200000, 25000, 1250, 1000, 600, 500, 70, 100, 60, 10, 0 };
const int kk[10] = { 5, 4, 4, 4, 3, 3, 3, 2, 2, 2 };

AIPlayer::AIPlayer(int8_t p = 0) {
    pid = p;
    type = 1;
    depth = 1;
}

void AIPlayer::getVec(const ChessPad& chessPad) {
    ChessPiece p;
    vec.clear();
    for (int8_t x = 0; x < 15; x++)
        for (int8_t y = 0; y < 15; y++)
            if (!chessPad.check(p.set(pid, x, y))) vec.push_back(p);
}

void AIPlayer::generate(const ChessPad& chessPad, cpv &v, int8_t pid) {
    int8_t pad[15][15] = {}, empty = 0;
    v.clear();
    for (int k = 0; k <= 1; k++) {
        auto& plist = chessPad.getPiece(k);
        if (plist.empty()) ++empty;
        for (auto p : plist) {
            for (int8_t i = -2; i <= 2; i++)
                for (int8_t j = -2; j <= 2; j++) {
                    int8_t x = p.getPosX() + i, y = p.getPosY() + j;
                    if (!chessPad.check(ChessPiece(pid, x, y)) && !pad[x][y])
                        pad[x][y] = 1;
                }
        }
    }
    if (empty == 2) pad[7][7] = 1;
    for (int8_t x = 0; x < 15; x++)
        for (int8_t y = 0; y < 15; y++)
            if (pad[x][y]) v.push_back(ChessPiece(pid, x, y));
}


int types[11];

ChessPiece AIPlayer::getNextPos(const ChessPad& oriChessPad) {
    ChessPiece maxP(pid, 0, 0);
    int maxVal = -1E9-8, tmp;
    ChessPad chessPad(oriChessPad);
    generate(chessPad, vec, pid);
    for (auto p : vec) {
        chessPad.place(p);
        //tmp = g(pid, chessPad); // + 14 - abs(p.getPosX() - 7) - abs(p.getPosY() - 7);
        tmp = dfs(0, chessPad, pid);
        chessPad.remove(p.getPid());
        std::cerr << (int)p.getPosX() << "," << (int)p.getPosY() << ":" << tmp << "|" << a << "vs" << b << std::endl;
        if (tmp > maxVal) {
            maxVal = tmp;
            maxP = p;
        }
    }
    return maxP;
}

int AIPlayer::g(int8_t pid, const ChessPad& chessPad) {
    int typ1[11] = {}, typ2[11] = {};
    a = f(pid, chessPad, typ1), b = f(3 - pid, chessPad, typ2);
    int die4 = typ2[DIE4] + typ2[LOWDIE4];
    int alive3 = typ2[ALIVE3] + typ2[TIAO3];
    if (alive3) {
        if (!die4 && !typ1[ALIVE4]) b += alive3 * sc[TIAO3];
    }
    if (typ2[ALIVE4] || die4 >= 8 || die4 && alive3 || alive3 >= 6) {
        die4 = typ1[DIE4] + typ1[LOWDIE4];
        alive3 = typ1[ALIVE3] + typ1[TIAO3];
        if (die4 >= 8 || die4 && alive3) b += 10000;// 双死4 死4活3
        if (alive3 >= 6) b += 5000;// 双活3
        a -= typ1[ALIVE4] * sc[ALIVE4];
    }
//    std::cerr << "AAA:" << alive3 << std::endl;
    return a - b;
}

int AIPlayer::f(int8_t pid, const ChessPad& chessPad, int types[11]) {
	auto &myp = chessPad.getPiece(pid - 1);
	int typenum[11] = {}, ret = 0, t;
//	int8_t rec[15][15][4] = {};
	for (auto p : myp) {
		memset(typenum, 0, sizeof(typenum));
		for (int i = 0; i < 4; i++) {
            t = chessPad.getType(p, i);
			++typenum[t], ++types[t];
		}
		ret += getExScore(typenum);
	}
    // for (int i = 0; i < 10; i++) /*types[i] /= kk[i], */std::cerr << types[i] << " ";
    // std::cerr << "\n";
    int die4 = types[DIE4] + types[LOWDIE4];
    int alive3 = types[ALIVE3] + types[TIAO3];
    if (die4 >= 8 || die4 && alive3) ret += 10000;// 双死4 死4活3
    if (alive3 >= 6) ret += 5000;// 双活3
    // 均为必杀
    for (int i = 0; i < 11; i++) ret += types[i] * sc[i];
    return ret;
}

int AIPlayer::getExScore(int typenum[]) {
	int die4 = typenum[DIE4] + typenum[LOWDIE4];
	int alive3 = typenum[ALIVE3] + typenum[TIAO3];
	int alive2 = typenum[ALIVE2] + typenum[LOWALIVE2];
    if (alive2 >= 2) return 500;//双活2
    //if (die4 >= 2 || die4 && alive3) return 10000;//双死4 死4活3
    //if (alive3 >= 2) return 5000;//双活3
    //if (typenum[DIE3] && typenum[ALIVE3]) return 2000;//死3高级活3
    return 0;
}

int AIPlayer::dfs(int d, ChessPad &pad, int8_t pid) {
    if (d == depth) {
        int gg = g(pid, pad);
        // std::cerr << "IGOTIT" << gg << std::endl;
        return gg;
    }
    cpv vec;
    generate(pad, vec, pid);
    // std::cerr << "VECSIZZE:" << vec.size() << std::endl;
    int mxRes;
    if (pid == this->pid) mxRes = -1e9 - 7;
    else mxRes = 1e9 + 7;
    for (auto p : vec) {
        // std::cerr << (int)p.getPosX() << "place" << (int)p.getPosY() << std::endl;
        pad.place(p);
        int res = dfs(d + 1, pad, 3 - pid);
        pad.remove(p.getPid());
        if (pid == this->pid && res > mxRes) mxRes = res;
        else if (pid != this->pid && res < mxRes) mxRes = res;
    }
    return mxRes;
}