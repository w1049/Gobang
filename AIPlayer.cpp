#include <cstring>
#include <iostream>
#include <algorithm>
#include "AIPlayer.h"
#include "ChessPiece.h"
using std::ostream;

enum ChessType { WIN5, ALIVE4, DIE4, LOWDIE4, ALIVE3, TIAO3, DIE3, ALIVE2, LOWALIVE2, DIE2, NOTHREAT };
const int sc[11] = { 100, 60, 60, 50, 60, 16, 12, 13, 5, 3, 0 };
const int ss[11] = { 100000, 10000, 1000, 1000, 100, 100, 10, 10, 1, 0 };
const int kk[10] = { 5, 4, 4, 4, 3, 3, 3, 2, 2, 2 };

AIPlayer::AIPlayer(int8_t p = 0) {
    pid = p;
    type = 1;
    depth = 7;
}

typedef std::pair<int, ChessPiece> prc;
typedef std::vector<prc> vprc;

int AIPlayer::evaluate(ChessPad& pad, ChessPiece p) {
    int typ[11] = {}, ret = 0;
    pad.place(p);
    for (int i = 0; i < 4; i++)
        ++typ[pad.getType(p, i)];
    pad.remove(p.getPid());
    for (int i = 0; i < 11; i++)
        ret += typ[i] * ss[i];
    return ret;
}

// 上一次是 3 - pid 下的，现在为 pid 选择落点
void AIPlayer::generate(ChessPad& chessPad, cpv &v, int8_t pid) {
    int8_t pad[15][15] = {}, empty = 0;
    v.clear();
    for (int k = 0; k <= 1; k++) {
        auto& plist = chessPad.getPiece(k);
        if (plist.empty()) ++empty;
        for (auto p : plist) {
            for (int8_t i = -1; i <= 1; i++)
                for (int8_t j = -1; j <= 1; j++) {
                    int8_t x = p.getPosX() + i, y = p.getPosY() + j;
                    if (!chessPad.check(ChessPiece(pid, x, y)) && !pad[x][y])
                        pad[x][y] = 1;
                }
        }
    }
    if (empty == 2) pad[7][7] = 1;
    vprc vec;
    ChessPiece p;
    for (int8_t x = 0; x < 15; x++)
        for (int8_t y = 0; y < 15; y++) {
            p.set(pid, x, y);
            if (pad[x][y]) vec.push_back({ -evaluate(chessPad, p), p });
        }
    sort(vec.begin(), vec.end());
    int n = vec.size();// > 20 ? 20 : vec.size();
    for (int i = 0; i < n; i++)
        v.push_back(vec[i].second);
}


int types[11];
ostream& operator<<(ostream& o, ChessPiece& p) {
    o << (int)p.getPid() << "-" << char(p.getPosY() + 'A') << (int)(p.getPosX() + 1);
    return o;
}
ChessPiece AIPlayer::getNextPos(const ChessPad& oriChessPad) {
    ChessPiece maxP;
    // int maxVal = -1E9-8, tmp;
    ChessPad chessPad(oriChessPad);
    // cpv vec;
    // generate(chessPad, vec, pid);
    // std::cerr << "=================================\n";
    dfs(maxP, 0, chessPad, pid);
    /*for (auto p : vec) {
        chessPad.place(p);
        //tmp = g(pid, chessPad); // + 14 - abs(p.getPosX() - 7) - abs(p.getPosY() - 7);
        tmp = -dfs(0, chessPad, 3 - pid);
        chessPad.remove(p.getPid());
        std::cerr << p << ":" << tmp << std::endl;
        if (tmp > maxVal) {
            maxVal = tmp;
            maxP = p;
        }
    }*/
    return maxP;
}

int winwin;

// pid 下完了一步，下一步由对方下了，pid 的胜算是多少？
int AIPlayer::g(int8_t pid, const ChessPad& chessPad) {
    int typ1[11] = {}, typ2[11] = {};
    int a = f(pid, chessPad, typ1) - f(3 - pid, chessPad, typ2);
    if (typ1[WIN5] || typ2[WIN5]) winwin = 1;
    else winwin = 0;
    int die4 = typ2[DIE4] + typ2[LOWDIE4];
    int alive3 = typ2[ALIVE3] + typ2[TIAO3];
    int Die4 = typ1[DIE4] + typ1[LOWDIE4];
    int Alive3 = typ1[ALIVE3] + typ1[TIAO3];
    if (typ1[WIN5]) {
        // 我们连5 赢麻了
        a += 100000;
    }
    else if (typ2[WIN5]) {
        // 对方连5 输麻了
        a -= 100000;
    }
    else if (typ2[ALIVE4]) {
        // 对方活4 已经输了
        a -= 80000;
    }
    else if (die4) {
        // 对方冲4 已经输了
        a -= 70000;
    }
    else if (typ1[ALIVE4] || Die4 >= 2) {
        // 我们活4 且对方没有4 已经赢了
        a += 80000;
    }
    else if (Die4 && Alive3) {
        // 我们冲4活3 且对方没有4 已经赢了
        a += 70000;
    }
    else if (!Die4 && alive3) {
        // 我们没有冲4 但对面有活3 对方下一步就要来活4 已经输了
        a -= 60000;
    }
    else if (Alive3 >= 2 && !alive3 && !typ2[DIE3]) {
        // 我们两个活3 对面没4也没活3 也就是说这一步他们最多造个3出来（或者堵我们一个3）
        // 下一步我们就能活4 对面却不能活4
        a += 60000;
    }
    return a;
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
    for (int i = 0; i < 10; i++) types[i] /= kk[i]; //std::cerr << types[i] << " ";
    // std::cerr << "\n";
    /*int die4 = types[DIE4] + types[LOWDIE4];
    int alive3 = types[ALIVE3] + types[TIAO3];
    if (die4 >= 8 || die4 && alive3) ret += 10000;// 双死4 死4活3
    if (alive3 >= 6) ret += 5000;// 双活3
    // 均为必杀*/
    for (int i = 0; i < 11; i++) ret += types[i] * sc[i];
    return ret;
}

int AIPlayer::getExScore(int typenum[]) {
	int die4 = typenum[DIE4] + typenum[LOWDIE4];
	int alive3 = typenum[ALIVE3] + typenum[TIAO3];
	int alive2 = typenum[ALIVE2] + typenum[LOWALIVE2];
    if (alive2 >= 2) return 15;//双活2
    //if (die4 >= 2 || die4 && alive3) return 10000;//双死4 死4活3
    //if (alive3 >= 2) return 5000;//双活3
    //if (typenum[DIE3] && typenum[ALIVE3]) return 2000;//死3高级活3
    return 0;
}
const int inf = 1e9 + 7;
// 上一次是 turn 下的，现在为 pid 选择落点
int AIPlayer::dfs(ChessPiece &maxp, int d, ChessPad &pad, int8_t pid, int alpha, int beta) {
    int score = -g(3 - pid, pad);
    if (depth == d) return score;
    if (winwin) return score;
    cpv vec;
    generate(pad, vec, pid);
    if (vec.empty()) return score;
    // std::cerr << "VECSIZZE:" << vec.size() << std::endl;
    int best = -inf;
    for (auto p : vec) {
        // std::cerr << (int)p.getPosX() << "place" << (int)p.getPosY() << std::endl;
        pad.place(p);
        ChessPiece meiyong;
        score = -dfs(meiyong, d + 1, pad, 3 - pid, -beta, -alpha);
        pad.remove(p.getPid());
        if (score > best) {
            best = score;
            maxp = p;
        }
        alpha = std::max(best, alpha);
        if (score >= beta) break;
    }
    // for (int i = 0; i < d; i++) std::cerr << '\t';
    // std::cerr << maxp << " " << best << std::endl;
    return best;
}