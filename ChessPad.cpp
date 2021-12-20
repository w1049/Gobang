#include <vector>
#include <cstring>
#include <iostream>
#include "ChessPad.h"

enum ChessType { WIN5, ALIVE4, DIE4, LOWDIE4, ALIVE3, TIAO3, DIE3, ALIVE2, LOWALIVE2, DIE2, NOTHREAT };
const int8_t ChessPad::dx[4] = { 0, 1, 1, 1 }, ChessPad::dy[4] = { 1, 0, 1, -1 };

// ������. �����ж��Ƿ�Ϸ�. ����ǰ���ȵ���check. ����Ҳ��ǿ�з���.(��Game������Ƿ���ã�����ֻ��ʵ�ַ���).
int ChessPad::place(const ChessPiece &p) {
    pad[p.getX()][p.getY()] = p.getPid();
    //pid=1����,pid=2����.
    // piece[p.getPid()-1].push_back(p);
    list.push_back(p);
    return 0;
}

// �Ƿ����
int ChessPad::isBanned(const ChessPiece &p) const {
    bool ong = 0;
    if (p.getPid() == 2) return 0;
    // �жϽ��ֲ�����
    int typ[11] = {};
    for (int i = 0, t; i < 4; i++) {
        t = getType(p, i, 1);
        if (t == INF) ong = 1; // ����
        else ++typ[t];
    }
    if (ong && !typ[WIN5]) return 3;
    if (typ[ALIVE3] + typ[TIAO3] >= 2) return 4; // 33
    if (typ[DIE4] + typ[LOWDIE4] >= 2) return 5; // 44
    return 0;
}

// �ж��Ƿ���Է�������. ����ֵΪ0������Է��ã�1�����λ���������ӣ�2������磬3 4 5������Է��ã�������.
int ChessPad::check(const ChessPiece &p) const {
    int8_t x = p.getX(), y = p.getY();
    if (x < 0 || x >= 15 || y < 0 || y >= 15) return 2;
    if (pad[x][y]) return 1;
    if (mode) return isBanned(p);
    return 0;
}

// �ж���������Ӻ��Ƿ�ʤ��.�����������£�����ֵΪ0��ʾ��ʤ����1��ʾʤ����2��ʾƽ��.
int ChessPad::judge(const ChessPiece &p) const {
    for (int i = 0; i < 4; i++)
        if (getType(p, i) == WIN5) return 1;
    return 0;
}

int ChessPad::p(int8_t i, int8_t j) const {
    return pad[i][j];
}

const cpv& ChessPad::getPiece() const {
    // if (i >= 0 && i < 2) return piece[i];
    return list;
}

void ChessPad::remove() {
    // ChessPiece p = piece[pid - 1].back();
    ChessPiece p = list.back();
    // piece[pid - 1].pop_back();
    list.pop_back();
    pad[p.getX()][p.getY()] = 0;
}

ChessPad::ChessPad(int mode): mode(mode) {
    memset(pad, 0, sizeof(pad));
    // piece[0].clear();
    // piece[1].clear();
    list.clear();
}

ChessPad::ChessPad(const ChessPad& p) {
    // piece[0] = p.piece[0];
    // piece[1] = p.piece[1];
    list = p.list;
    memcpy(pad, p.pad, sizeof(pad));
    mode = p.mode;
    // std::cerr << "COPY\n";
}
int ChessPad::getMode() {
    return mode;
}
int ChessPad::getType(const ChessPiece &p, int direc, int judgeLong) const {
    // if (rec[p.getX()][p.getY()][direc]) return rec[p.getX()][p.getY()][direc];
    int8_t line[9] = {};
    getLine(p, direc, line);
    int re = getType(line);
    if (judgeLong && !line[4]) return INF;
//    std::cerr << re << " ";
//    for (int i = 0; i < 9; i++) std::cerr << (int)line[i] << ",";
//    std::cerr << std::endl;
    return re;
}

void ChessPad::getLine(const ChessPiece &p, int direc, int8_t line[9]) const {
    int8_t dx = ChessPad::dx[direc], dy = ChessPad::dy[direc];
    int8_t x = p.getX(), y = p.getY();
    int8_t pid = p.getPid(), cnt = 1;
    line[4] = pid;
    x += dx, y += dy;
    for (int8_t c = 5; c < 9; c++)
        if (x >= 0 && x < 15 && y >= 0 && y < 15)
           line[c] = pad[x][y], x += dx, y += dy;
        else line[c] = 3 - pid;
    x = p.getX(), y = p.getY();
    x -= dx, y -= dy;
    for (int8_t c = 3; c >= 0; c--)
        if (x >= 0 && x < 15 && y >= 0 && y < 15)
            line[c] = pad[x][y], x -= dx, y -= dy;
        else line[c] = 3 - pid;
}

// �����жϲο���https://www.cnblogs.com/songdechiu/p/5768999.html

int ChessPad::getType(int8_t line[9]) const {
    int pid = line[4];
    int hid = 3 - pid;
    int l, r;     //��ʼ�������߶Ͽ���λ��
    int cnt = 1;  //�������ж��ٸ�����ʼ��
    for (int i = 1; i <= 4; i++)
        if (line[4 - i] == pid) cnt++;
        else {
            l = 4 - i;  //����Ͽ�λ��
            break;
        }
    for (int i = 1; i <= 4; i++)
        if (line[4 + i] == pid) cnt++;
        else {
            r = 4 + i;  //����Ͽ�λ��
            break;
        }
    if (cnt > 5) {
        line[4] = 0; // ����
        return WIN5;
    }
    if (cnt == 5) return WIN5;
    int cl = line[l], cr = line[r];  // cl := (cl == hid)
    if (cnt == 4) {                       //������4��
        if (!cl && !cr)                   //���߶Ͽ�λ�þ���
            return ALIVE4;                //����
        else if (cl == hid && cr == hid)  //���߶Ͽ�λ�þ��ǿ�
            return NOTHREAT;              //û����в
        else if (!cl || !cr)              //���߶Ͽ�λ��ֻ��һ����
            return DIE4;                  //����
    }
    if (cnt == 3) {        //������3��
        if (!cl && !cr) {  //���߶Ͽ�λ�þ���
            if (line[l - 1] == hid && line[r + 1] == hid)  //��Ϊ��������
                return DIE3;
            else if (line[l - 1] == pid || line[r + 1] == pid)  //ֻҪһ��Ϊ�Լ�������
                return LOWDIE4;
            else if (!line[l - 1] || !line[r + 1])  //ֻҪ��һ����
                return ALIVE3;
        }
        else if (cl == hid && cr == hid) {  //���߶Ͽ�λ�þ��ǿ�
            return NOTHREAT;                  //û����в
        }
        else if (!cl || !cr) {         //���߶Ͽ�λ��ֻ��һ����
            if (cl == hid) {             //��߱��Է���ס
                if (line[r + 1] == hid)  //�ұ�Ҳ���Է���ס
                    return NOTHREAT;
                if (!line[r + 1])  //�ұ߾���
                    return DIE3;
                if (line[r + 1] == pid) return LOWDIE4;
            }
            if (cr == hid) {             //�ұ߱��Է���ס
                if (line[l - 1] == hid)  //���Ҳ���Է���ס
                    return NOTHREAT;
                if (!line[l - 1])  //��߾���
                    return DIE3;
                if (line[l - 1] == pid)  //��߻����Լ�������
                    return LOWDIE4;
            }
        }
    }
    if (cnt == 2) {        //������2��
        if (!cl && !cr) {  //���߶Ͽ�λ�þ���
            if (!line[r + 1] && line[r + 2] == pid || !line[l - 1] && line[l - 2] == pid)
                return DIE3;  //��3
            else if (!line[l - 1] && !line[r + 1])
                return ALIVE2;  //��2
            if (line[r + 1] == pid && line[r + 2] == hid || line[l - 1] == pid && line[l - 2] == hid)
                return DIE3;  //��3
            if (line[r + 1] == pid && line[r + 2] == pid || line[l - 1] == pid && line[l - 2] == pid)
                return LOWDIE4;  //��4
            if (line[r + 1] == pid && !line[r + 2] || line[l - 1] == pid && !line[l - 2])
                return TIAO3;  //����3
                               //����������±߷���NOTHREAT
            if (!line[l - 1] && line[r + 1] == hid || line[l - 1] == hid && !line[r + 1])
                return LOWALIVE2;
        }
        else if (cl == hid && cr == hid) {  //���߶Ͽ�λ�þ��ǿ�
            return NOTHREAT;
        }
        else if (!cl || !cr) {  //���߶Ͽ�λ��ֻ��һ����
            if (cl == hid) {      //��߱��Է���ס
                if (line[r + 1] == hid || line[r + 2] == hid) {  //ֻҪ�жԷ���һ������
                    return NOTHREAT;       //û����в
                }
                else if (!line[r + 1] && !line[r + 2]) {  //����
                    return DIE2;                            //��2
                }
                else if (line[r + 1] == pid && line[r + 2] == pid) {  //��Ϊ�Լ�������
                    return LOWDIE4;               //��4
                }
                else if (line[r + 1] == pid || line[r + 2] == pid) {  //ֻ��һ���Լ�������
                    return DIE3;                  //��3
                }
            }
            if (cr == hid) {  //�ұ߱��Է���ס
                if (line[l - 1] == hid || line[l - 2] == hid) {  //ֻҪ�жԷ���һ������
                    return NOTHREAT;       //û����в
                }
                else if (!line[l - 1] && !line[l - 2]) {  //����
                    return DIE2;                            //��2
                }
                else if (line[l - 1] == pid && line[l - 2] == pid) {  //��Ϊ�Լ�������
                    return LOWDIE4;               //��4
                }
                else if (line[l - 1] == pid || line[l - 2] == pid) {  //ֻ��һ���Լ�������
                    return DIE3;                  //��3
                }
            }
        }
    }
    if (cnt == 1) {  //������1��
        if (!cl && line[l - 1] == pid && line[l - 2] == pid && line[l - 3] == pid)
            return LOWDIE4;
        if (!cr && line[r + 1] == pid && line[r + 2] == pid && line[r + 3] == pid)
            return LOWDIE4;
        if (!cl && line[l - 1] == pid && line[l - 2] == pid && !line[l - 3] && !cr)
            return TIAO3;
        if (!cr && line[r + 1] == pid && line[r + 2] == pid && !line[r + 3] && !cl)
            return TIAO3;
        if (!cl && line[l - 1] == pid && line[l - 2] == pid && line[l - 3] == hid && !cr)
            return DIE3;
        if (!cr && line[r + 1] == pid && line[r + 2] == pid && line[r + 3] == hid && !cl)
            return DIE3;
        if (!cl && !line[l - 1] && line[l - 2] == pid && line[l - 3] == pid)
            return DIE3;
        if (!cr && !line[r + 1] && line[r + 2] == pid && line[r + 3] == pid)
            return DIE3;
        if (!cl && line[l - 1] == pid && !line[l - 2] && line[l - 3] == pid)
            return DIE3;
        if (!cr && line[r + 1] == pid && !line[r + 2] && line[r + 3] == pid)
            return DIE3;
        if (!cl && line[l - 1] == pid && !line[l - 2] && !line[l - 3] && !cr)
            return LOWALIVE2;
        if (!cr && line[r + 1] == pid && !line[r + 2] && !line[r + 3] && !cl)
            return LOWALIVE2;
        if (!cl && !line[l - 1] && line[l - 2] == pid && !line[l - 3] && !cr)
            return LOWALIVE2;
        if (!cr && !line[r + 1] && line[r + 2] == pid && !line[r + 3] && !cl)
            return LOWALIVE2;
        //�������±߷���û����в
    }
    return NOTHREAT;  //����û����в
}