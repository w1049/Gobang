#include "AIPlayer.h"
#include "ChessPiece.h"

typedef std::pair<int, ChessPiece> prc;
typedef std::vector<prc> vprc;

namespace AI {
const int SCORE[11] = { 100, 60, 60, 50, 60, 16, 12, 13, 5, 3, 0 };
const int E_SCORE[11] = { 100000, 10000, 1000, 1000, 100, 100, 10, 10, 1, 0 };
const int NUM_OF_TYPE[10] = { 5, 4, 4, 4, 3, 3, 3, 2, 2, 2 };
const int CONSIDER_LIMIT = 12;
int gameOver;
}

using namespace AI;
using std::array;

AIPlayer::AIPlayer(int p) : Player(p, 0), r(1), depth(7) {}
ChessPiece AIPlayer::getNextPiece(const ChessPad& oriChessPad) {
    ChessPiece maxP;
    ChessPad chessPad(oriChessPad); // 复制一份
    dfs(maxP, 0, chessPad, pid); // 搜索出结果
    return maxP;
}

int AIPlayer::dfs(ChessPiece& maxP, int d, ChessPad& pad, int pid, int alpha, int beta) {
    int score = -g(3 - pid, pad); // 获得分数
    if (depth == d) return score; // 到达深度限制
    if (AI::gameOver) return score; // 游戏已经结束

    // 生成落点
    cpv vec;
    generate(pad, vec, pid, r);
    if (vec.empty()) return score; // 无路可走

    // 开始搜索
    for (auto p : vec) {
        pad.place(p); // 落子
        ChessPiece unused; // 懒得重写函数了，这个实际上是无用的
        // 向下搜索
        score = -dfs(unused, d + 1, pad, 3 - pid, -beta, -alpha);
        // 还原棋盘
        pad.remove();
        // 更新
        if (score > alpha) {
            alpha = score;
            maxP = p;
        }
        // 剪枝
        if (alpha >= beta) break;
    }

    return alpha;
}

int AI::evaluate(ChessPad& pad, const ChessPiece &p) {
    int typ[11] = {}, ret = 0;
    // pad.place(p);
    for (int i = 0; i < 4; i++)
        ++typ[pad.getType(p, i)];
    // pad.remove();
    for (int i = 0; i < 11; i++)
        ret += typ[i] * E_SCORE[i];
    return ret;
}


void AI::generate(ChessPad& chessPad, cpv &v, int pid, int8_t r) {
    // 临时记录棋盘情况
    // -1: 未检查; 其余值: 参考 ChessPad 类
    int8_t pad[15][15];
    memset(pad, -1, sizeof(pad));
    v.clear();

    auto& plist = chessPad.getPiecesList(); // 获取所有棋子
    if (plist.empty()) pad[7][7] = 0; // 若未下子，下最中间
    // 遍历每个棋子
    for (auto p : plist) {
    // 在每个棋子周围 r 的范围内寻找落子点
        for (int8_t i = -r; i <= r; i++)
            for (int8_t j = -r; j <= r; j++) {
                int8_t x = p.getX() + i, y = p.getY() + j;
                if (x >= 0 && x < 15 && y >= 0 && y < 15 && pad[x][y] == -1) {
                    pad[x][y] = chessPad.checkState(ChessPiece(1, x, y));
                    // 记录情况
                }

            }
    }

    cpv win5; // 可以连5的位置
    cpv valive4, vdie4, vdouble3, valive3; // 我方的落子点
    cpv ualive4, udie4, udouble3, ualive3; // 对方的落子点
    vprc vec; // 排序用数组
    ChessPiece p;
    for (int8_t x = 0; x < 15; x++)
        for (int8_t y = 0; y < 15; y++) {
            if (pad[x][y] == -1 || pad[x][y] == 1 || pad[x][y] == 2) continue;
            // 可下子 或 后手(无禁手)
            if (!pad[x][y] || pid == 2) {
                p.set(3 - pid, x, y); // 评估对方的分数
                int hiScore = evaluate(chessPad, p);
                p.setPid(pid); // 由于生成的是我方的走法，加入前先改为我方棋子
                if (hiScore >= E_SCORE[WIN5]) win5.push_back(p);
                else if (hiScore > E_SCORE[ALIVE4]) ualive4.push_back(p);
                else if (hiScore > E_SCORE[DIE4]) udie4.push_back(p);
                else if (hiScore > 2 * E_SCORE[ALIVE3]) udouble3.push_back(p);
                else if (hiScore > E_SCORE[ALIVE3]) ualive3.push_back(p);

                p.set(pid, x, y); // 评估我方的分数
                int myScore = evaluate(chessPad, p);
                if (myScore >= E_SCORE[WIN5]) win5.push_back(p);
                else if (myScore > E_SCORE[ALIVE4]) valive4.push_back(p);
                else if (myScore > E_SCORE[DIE4]) vdie4.push_back(p);
                else if (myScore > 2 * E_SCORE[ALIVE3]) vdouble3.push_back(p);
                else if (myScore > E_SCORE[ALIVE3]) valive3.push_back(p);
                // 否则为普通棋子，加入排序数组
                else vec.push_back({ hiScore - myScore, p });
            }
        }

    if (!win5.empty()) v = win5; // 如果可以连5，只考虑连5点
    else if (!valive4.empty()) v = valive4; // 我方可以活4
    else if (!ualive4.empty() && vdie4.empty()) v = ualive4; // 对方可以活4，我方不能冲4，就去堵他
    else if (!ualive4.empty()) { // 对方活4，我方可以冲4，考虑所有4点
        v = ualive4;
        v.insert(v.end(), vdie4.begin(), vdie4.end());
        v.insert(v.end(), udie4.begin(), udie4.end());
    } else { // 大家都没有活4，不会太快结束
        // 考虑所有3点
        v = vdouble3;
        v.insert(v.end(), udouble3.begin(), udouble3.end());
        v.insert(v.end(), vdie4.begin(), vdie4.end());
        v.insert(v.end(), udie4.begin(), udie4.end());
        v.insert(v.end(), valive3.begin(), valive3.end());
        v.insert(v.end(), ualive3.begin(), ualive3.end());
        // 如果没有3点，就考虑普通棋子
        if (udouble3.empty() && vdouble3.empty()) {
            // 排序，选择分数靠前的，分数太低的直接忽略
            sort(vec.begin(), vec.end());
            int n = vec.size() > CONSIDER_LIMIT ? CONSIDER_LIMIT : vec.size();
            for (int i = 0; i < n; i++)
                v.push_back(vec[i].second);
        }
    }
}

int AI::g(int pid, const ChessPad& chessPad) {
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

    // 我们连5 直接赢了
    if (typ1[WIN5]) a += 100000;
    // 对方连5 直接输了
    else if (typ2[WIN5]) a -= 100000;
    // 对方活4 马上输了
    else if (typ2[ALIVE4]) a -= 80000;
    // 对方冲4 马上输了
    else if (die4) a -= 70000;
    // 我们活4 且对方没有4 已经赢了
    else if (typ1[ALIVE4] || Die4 >= 2) a += 80000;
    // 我们冲4活3 且对方没有4 已经赢了
    else if (Die4 && Alive3) a += 70000;
    // 我们没有冲4 但对面有活3 对方下一步就要来活4 已经输了
    else if (!Die4 && alive3) a -= 60000;
    // 我们两个活3 对面没4也没活3 也就是说这一步他们最多造个3出来（或者堵我们一个3）
    // 下一步我们就能活4 对面却不能活4
    else if (Alive3 >= 2 && !alive3 && !typ2[DIE3]) a += 60000;

    return a;
}

int AI::f(int pid, const ChessPad& chessPad, int types[11]) {
    auto &plist = chessPad.getPiecesList();
    int single_type[11] = {}, ret = 0, t;
    // 遍历棋子
    for (auto p : plist) {
        // 只关注己方棋子
        if (p.getPid() != pid) continue;
        memset(single_type, 0, sizeof(single_type));
        // 对每个方向计算旗形
        for (int i = 0; i < 4; i++) {
            t = chessPad.getType(p, i);
            ++single_type[t], ++types[t];
        }
        // 单个棋子形成多旗形可以加分
        ret += getExScore(single_type);
    }
    // 每个旗形会被重复计算，这里简单的除以数量，可以考虑优化
    for (int i = 0; i < 10; i++) types[i] /= NUM_OF_TYPE[i];
    // 计算总分
    for (int i = 0; i < 11; i++) ret += types[i] * SCORE[i];
    return ret;
}

int AI::getExScore(int typenum[]) {
    // int die4 = typenum[DIE4] + typenum[LOWDIE4];
    // int alive3 = typenum[ALIVE3] + typenum[TIAO3];
    int alive2 = typenum[ALIVE2] + typenum[LOWALIVE2];
    if (alive2 >= 2) return 15; // 双活2
    return 0;
}
