#ifndef __AI_PLAYER_H__
#define __AI_PLAYER_H__

#include "ChessPad.h"
#include "Player.h"

class AIPlayer : public Player {
public:
    AIPlayer(int);
    ChessPiece getNextPos(const ChessPad&);
private:
    // ��㷶Χ
    int8_t r;
    // ����
    int dfs(ChessPiece&, int, ChessPad&, int8_t, int = -INF, int = INF);
    // �������
    int depth;
};
namespace AI {
    /**
    * @brief ����ʽ��������������ĳһ��λ�����ȳ̶�
    * @param[in] ����
    * @param[in] ��λ
    * @return �ڿ�λ���ӵķ���
    */
    int evaluate(ChessPad&, const ChessPiece&);
    /**
    * @brief �������
    * @param[in] chessPad ����
    * @param[in] v �����߷����� vector
    * @param[in] pid ���ڽ�Ҫ���ӵ����
    * @param[in] r ������������ӵľ���
    */
    void generate(ChessPad& chessPad, cpv& v, int8_t pid, int8_t r);
    // ���� pid ��ԭʼ���Ʒ�
    int f(int8_t, const ChessPad&, int[11]);
    // pid ����һ��֮�󣬼��� pid �ľ��Ʒ�
    int g(int8_t, const ChessPad&);
    // ��һ�����γɶ�������ʱ�������÷���
    int getExScore(int[]);
}
#endif