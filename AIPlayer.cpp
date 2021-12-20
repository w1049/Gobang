#include <cstring>
#include <algorithm>

#include "AIPlayer.h"
#include "ChessPiece.h"

enum ChessType { WIN5, ALIVE4, DIE4, LOWDIE4, ALIVE3, TIAO3, DIE3, ALIVE2, LOWALIVE2, DIE2, NOTHREAT };
const int sc[11] = { 100, 60, 60, 50, 60, 16, 12, 13, 5, 3, 0 };
const int ss[11] = { 100000, 10000, 1000, 1000, 100, 100, 10, 10, 1, 0 };
const int kk[10] = { 5, 4, 4, 4, 3, 3, 3, 2, 2, 2 };
using namespace AI;

AIPlayer::AIPlayer(int p) : Player(p, 1), depth(7), r(1) {}

typedef std::pair<int, ChessPiece> prc;
typedef std::vector<prc> vprc;

namespace AI {
    int gameOver;
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

// 上一次是 turn 下的，现在为 pid 选择落点
int AIPlayer::dfs(ChessPiece& maxp, int d, ChessPad& pad, int8_t pid, int alpha, int beta) {
    int score = -g(3 - pid, pad);
    if (depth == d) return score;
    if (AI::gameOver) return score;
    cpv vec;
    generate(pad, vec, pid, r);
    if (vec.empty()) return score;
    int best = -INF;
    for (auto p : vec) {
        pad.place(p);
        ChessPiece meiyong;
        score = -dfs(meiyong, d + 1, pad, 3 - pid, -beta, -alpha);
        pad.remove();
        if (score > best) {
            best = score;
            maxp = p;
        }
        alpha = std::max(best, alpha);
        if (score >= beta) break;
    }
    return best;
}

int AI::evaluate(ChessPad& pad, const ChessPiece &p) {
    int typ[11] = {}, ret = 0;
    pad.place(p);
    for (int i = 0; i < 4; i++)
        ++typ[pad.getType(p, i)];
    pad.remove();
    for (int i = 0; i < 11; i++)
        ret += typ[i] * ss[i];
    return ret;
}


void AI::generate(ChessPad& chessPad, cpv &v, int8_t pid, int8_t r) {
    int8_t pad[15][15] = {}, empty = 0;
    memset(pad, -1, sizeof(pad));
    v.clear();
    auto& plist = chessPad.getPiece();
    if (plist.empty()) pad[7][7] = 0;
    for (auto p : plist) {
        for (int8_t i = -r; i <= r; i++)
            for (int8_t j = -r; j <= r; j++) {
                int8_t x = p.getX() + i, y = p.getY() + j;
                if (x >= 0 && x < 15 && y >= 0 && y < 15 && pad[x][y] == -1) {
                    int ddd = chessPad.check(ChessPiece(1, x, y));
                    pad[x][y] = ddd;
                }
                        
            }
    }
    vprc vec;
    cpv win5, valive4, vdie4, vdouble3, valive3;
    cpv ualive4, udie4, udouble3, ualive3;
    ChessPiece p;
    for (int8_t x = 0; x < 15; x++)
        for (int8_t y = 0; y < 15; y++) {
            if (pad[x][y] == -1 || pad[x][y] == 1 || pad[x][y] == 2) continue;

            p.set(3 - pid, x, y);
            int hiScore;
            if (!pad[x][y] || pid == 1 || !chessPad.getMode()) {
                hiScore = evaluate(chessPad, p);
                p.setPid(pid);
                if (hiScore >= ss[WIN5]) win5.push_back(p);
                else if (hiScore > ss[ALIVE4]) ualive4.push_back(p);
                else if (hiScore > ss[DIE4]) udie4.push_back(p);
                else if (hiScore > 2 * ss[ALIVE3]) udouble3.push_back(p);
                else if (hiScore > ss[ALIVE3]) ualive3.push_back(p);
            }
            else hiScore = 0;

            p.set(pid, x, y);
            int myScore;
            if (!pad[x][y] || pid == 2 || !chessPad.getMode()) {
                myScore = evaluate(chessPad, p);
                if (myScore >= ss[WIN5]) win5.push_back(p);
                else if (myScore > ss[ALIVE4]) valive4.push_back(p);
                else if (myScore > ss[DIE4]) vdie4.push_back(p);
                else if (myScore > 2 * ss[ALIVE3]) vdouble3.push_back(p);
                else if (myScore > ss[ALIVE3]) valive3.push_back(p);
                else vec.push_back({ hiScore - myScore, p });
            }
        }

    if (!win5.empty()) v = win5;
    else if (!valive4.empty()) v = valive4;
    else if (!ualive4.empty() && vdie4.empty()) v = ualive4;
    else if (!ualive4.empty()) {
        v = ualive4;
        v.insert(v.end(), vdie4.begin(), vdie4.end());
        v.insert(v.end(), udie4.begin(), udie4.end());
    } else {
        v = vdouble3;
        v.insert(v.end(), udouble3.begin(), udouble3.end());
        v.insert(v.end(), vdie4.begin(), vdie4.end());
        v.insert(v.end(), udie4.begin(), udie4.end());
        v.insert(v.end(), valive3.begin(), valive3.end());
        v.insert(v.end(), ualive3.begin(), ualive3.end());
        if (!udouble3.empty() || !vdouble3.empty());
        else {
            sort(vec.begin(), vec.end());
            int n = vec.size() > 12 ? 12 : vec.size();
            for (int i = 0; i < n; i++)
                v.push_back(vec[i].second);
        }
    }
}

int AI::g(int8_t pid, const ChessPad& chessPad) {
    int typ1[11] = {}, typ2[11] = {}; // 棋形数组

    // 计算基础分
    int a = f(pid, chessPad, typ1) - f(3 - pid, chessPad, typ2);
    
    // 如果有5，记录一下
    if (typ1[WIN5] || typ2[WIN5]) gameOver = 1;
    else gameOver = 0;

    // 对方的冲4、活3
    int die4 = typ2[DIE4] + typ2[LOWDIE4];
    int alive3 = typ2[ALIVE3] + typ2[TIAO3];

    // 我方的冲4、活3
    int Die4 = typ1[DIE4] + typ1[LOWDIE4];
    int Alive3 = typ1[ALIVE3] + typ1[TIAO3];

    // 在原始局势分的基础上增加额外分

    if (typ1[WIN5]) a += 100000;
    // 我们连5 直接赢了

    else if (typ2[WIN5]) a -= 100000;
    // 对方连5 直接输了

    else if (typ2[ALIVE4]) a -= 80000;
    // 对方活4 马上输了

    else if (die4) a -= 70000;
    // 对方冲4 马上输了

    else if (typ1[ALIVE4] || Die4 >= 2) a += 80000;
    // 我们活4 且对方没有4 已经赢了

    else if (Die4 && Alive3) a += 70000;
    // 我们冲4活3 且对方没有4 已经赢了

    else if (!Die4 && alive3) a -= 60000;
    // 我们没有冲4 但对面有活3 对方下一步就要来活4 已经输了
    
    else if (Alive3 >= 2 && !alive3 && !typ2[DIE3]) a += 60000;
    // 我们两个活3 对面没4也没活3 也就是说这一步他们最多造个3出来（或者堵我们一个3）
    // 下一步我们就能活4 对面却不能活4

    return a;
}

int AI::f(int8_t pid, const ChessPad& chessPad, int types[11]) {
	auto &plist = chessPad.getPiece();
	int typenum[11] = {}, ret = 0, t;
//	int8_t rec[15][15][4] = {};
	for (auto p : plist) {
        if (p.getPid() != pid) continue;
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

int AI::getExScore(int typenum[]) {
	int die4 = typenum[DIE4] + typenum[LOWDIE4];
	int alive3 = typenum[ALIVE3] + typenum[TIAO3];
	int alive2 = typenum[ALIVE2] + typenum[LOWALIVE2];
    if (alive2 >= 2) return 15;//双活2
    //if (die4 >= 2 || die4 && alive3) return 10000;//双死4 死4活3
    //if (alive3 >= 2) return 5000;//双活3
    //if (typenum[DIE3] && typenum[ALIVE3]) return 2000;//死3高级活3
    return 0;
}