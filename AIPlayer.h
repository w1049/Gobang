#ifndef __AI_PLAYER_H__
#define __AI_PLAYER_H__

#include "Player.h"

class AIPlayer : public Player {
public:
    AIPlayer(int);
    ChessPiece getNextPiece(const ChessPad&);

private:
    // 落点范围
    int8_t r;
    // 搜索
    int dfs(ChessPiece&, int, ChessPad&, int, int = -INF, int = INF);
    // 搜索深度
    int depth;
};
namespace AI {
// 启发式评估函数，评估某一空位的优先程度
int evaluate(ChessPad&, const ChessPiece&);
// 生成落点，v 为保存走法所用 vector，pid 为现在将要下子的玩家
void generate(ChessPad& chessPad, cpv& v, int pid, int8_t r);
// 计算 pid 的原始局势分
int f(int, const ChessPad&, int[11]);
// pid 下完一步之后，计算 pid 的局势分
int g(int, const ChessPad&);
// 单一棋子形成多种旗形时，额外获得分数
int getExScore(int[]);
}  // namespace AI
#endif
