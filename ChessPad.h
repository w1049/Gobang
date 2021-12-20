#ifndef __CHESS_PAD_H__
#define __CHESS_PAD_H__

#include <vector>

#include "ChessPiece.h"

class ChessPad {
private:
    int8_t pad[15][15]; // ����
    // cpv piece[2]; // ����; ����˳��
    cpv list;
    int isBanned(const ChessPiece&) const;
    const static int8_t dx[4], dy[4];
    int8_t mode; // ����ģʽ 0����
public:
    ChessPad(int);
    ChessPad(const ChessPad&);
    int getType(const ChessPiece&, int direc, int = 0) const;
    void getLine(const ChessPiece&, int direc, int8_t[9]) const;
    int getType(int8_t[9]) const;
    int place(const ChessPiece&);
    // ������. �����ж��Ƿ�Ϸ�. ����ǰ���ȵ���check. ����Ҳ��ǿ�з���.(��Game������Ƿ���ã�����ֻ��ʵ�ַ���).
    int check(const ChessPiece&) const;
    // �ж��Ƿ���Է�������. ����ֵΪ0������Է��ã�1�����λ���������ӣ�2������Է���, �����֣��ݲ����ֽ���ԭ��.
    int judge(const ChessPiece&) const;
    // �ж���������Ӻ��Ƿ�ʤ��. ����ֵΪ0��ʾ��ʤ����1��ʾʤ����2��ʾƽ��.
    int p(int8_t, int8_t) const;
    const cpv& getPiece() const;
    void remove();
    int getMode();
};

#endif