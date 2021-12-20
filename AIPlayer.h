#ifndef __AI_PLAYER_H__
#define __AI_PLAYER_H__

#include "ChessPad.h"
#include "Player.h"

class AIPlayer : public Player {
public:
    AIPlayer(int);
    ChessPiece getNextPos(const ChessPad&);
private:
    // 落点范围
    int8_t r;
    // 搜索
    int dfs(ChessPiece&, int, ChessPad&, int8_t, int = -INF, int = INF);
    // 搜索深度
    int depth;
};
namespace AI {
    /**
    * @brief 启发式评估函数，评估某一空位的优先程度
    * @param[in] 棋盘
    * @param[in] 空位
    * @return 在空位下子的分数
    */
    int evaluate(ChessPad&, const ChessPiece&);
    /**
    * @brief 生成落点
    * @param[in] chessPad 棋盘
    * @param[in] v 保存走法所用 vector
    * @param[in] pid 现在将要下子的玩家
    * @param[in] r 落点与已有棋子的距离
    */
    void generate(ChessPad& chessPad, cpv& v, int8_t pid, int8_t r);
    // 计算 pid 的原始局势分
    int f(int8_t, const ChessPad&, int[11]);
    // pid 下完一步之后，计算 pid 的局势分
    int g(int8_t, const ChessPad&);
    // 单一棋子形成多种旗形时，额外获得分数
    int getExScore(int[]);
}
#endif