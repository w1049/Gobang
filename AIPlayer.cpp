#include <cstring>
#include <iostream>
#include <cmath>
#include "AIPlayer.h"
#include "ChessPiece.h"

enum ChessType { WIN5, ALIVE4, DIE4, LOWDIE4, ALIVE3, TIAO3, DIE3, ALIVE2, LOWALIVE2, DIE2, NOTHREAT };

AIPlayer::AIPlayer(uint8_t p = 0) {
    pid = p;
    type = 1;
    memset(pad, 0, sizeof(pad));
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
    if (chessPad->getPiece(pid - 1).size() == 0 && chessPad->getPiece(2 - pid).size() == 0) return ChessPiece(pid, 7, 7);
    ChessPiece maxP(pid, 0, 0);
    int maxVal = -1E9-7, tmp;
    getVec(chessPad);
	chessPad->writePad(pad);
    for (auto p : vec) {
        pad[p.getPosX()][p.getPosY()] = pid;
        int a, b;
        tmp = g(pid, chessPad, a, b);
        std::cerr << (int)p.getPosX() << "," << (int)p.getPosY() << ":" << tmp << "|" << a << "vs" << b << std::endl;
        if (tmp > maxVal) {
            maxVal = tmp;
            maxP = p;
        } else if (tmp == maxVal) {
            int a = abs(p.getPosX() - 7) + abs(p.getPosY() - 7);
            int b = abs(maxP.getPosX() - 7) + abs(maxP.getPosY() - 7);
            if (b < a) {
                maxVal = tmp;
                maxP = p;
            }
        }
        pad[p.getPosX()][p.getPosY()] = 0;
    }
    return maxP;
}

const int sc[11] = { 100000, 10000, 500, 400, 100, 90, 5, 10, 9, 2, 0 };

int AIPlayer::g(uint8_t pid, const ChessPad* chessPad, int &a, int &b) {
    int typ1[11] = {}, typ2[11] = {};
    a = f(pid, chessPad, typ1), b = f(3 - pid, chessPad, typ2);
    int ret = a - b;
    int die4 = typ2[DIE4] + typ2[LOWDIE4];
    int alive3 = typ2[ALIVE3] + typ2[TIAO3];
    if (typ2[ALIVE4] || die4 >= 2 || die4 && alive3 || alive3 >= 2) {
        die4 = typ1[DIE4] + typ1[LOWDIE4];
        alive3 = typ1[ALIVE3] + typ1[TIAO3];
        if (die4 >= 2 || die4 && alive3) ret -= 10000;// ˫��4 ��4��3
        if (alive3 >= 2) ret -= 5000;// ˫��3
        ret -= typ1[ALIVE4] * sc[ALIVE4];
    }
    return ret;
}

int AIPlayer::f(uint8_t pid, const ChessPad* chessPad, int types[11]) {
	auto& myp = chessPad->getPiece(pid - 1);
	int typenum[11] = {}, ret = 0, t;
	uint8_t rec[15][15][4] = {};
	memset(rec, 0, sizeof(rec));
	for (auto p : myp) {
		memset(typenum, 0, sizeof(typenum));
		for (int i = 0; i < 4; i++) {
            t = getType(p, i);
			++typenum[t], ++types[t];
		}
		ret += getExScore(typenum);
	}
    int die4 = types[DIE4] + types[LOWDIE4];
    int alive3 = types[ALIVE3] + types[TIAO3];
    if (die4 >= 2 || die4 && alive3) ret += 10000;// ˫��4 ��4��3
    if (alive3 >= 2) ret += 5000;// ˫��3
    // ��Ϊ��ɱ
    for (int i = 0; i < 11; i++) ret += types[i] * sc[i];
    return ret;
}

int AIPlayer::getType(ChessPiece p, int8_t direc) {
	getLine(p, direc);
	if (rec[p.getPosX()][p.getPosY()][direc]) return rec[p.getPosX()][p.getPosY()][direc];
	else return getType();
}

void AIPlayer::getLine(ChessPiece p, int8_t direc) {
    switch (direc) {
    case 0: getLine(p, 1, 0); break;
    case 1: getLine(p, 0, 1); break;
    case 2: getLine(p, 1, 1); break;
    case 3: getLine(p, 1, -1); break;
    }
}

void AIPlayer::getLine(ChessPiece p, int8_t dx, int8_t dy) {
    int8_t x = p.getPosX(), y = p.getPosY();
    uint8_t pid = p.getPid(), cnt = 1;
    line[4] = pad[x][y];
    x += dx, y += dy;
    for (int8_t c = 5; c < 9; c++)
        if (x >= 0 && x < 15 && y >= 0 && y < 15)
            line[c] = pad[x][y], x += dx, y += dy;
        else line[c] = 3 - pid;
    x = p.getPosX(), y = p.getPosY();
    x -= dx, y -= dy;
    for (int8_t c = 3; c >= 0; c--)
        if (x >= 0 && x < 15 && y >= 0 && y < 15)
            line[c] = pad[x][y], x -= dx, y -= dy;
        else line[c] = 3 - pid;
}

// �����жϲο���https://www.cnblogs.com/songdechiu/p/5768999.html

int AIPlayer::getType() {
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
    if (cnt > 4) return WIN5;
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

int AIPlayer::getExScore(int typenum[]) {
	int die4 = typenum[DIE4] + typenum[LOWDIE4];
	int alive3 = typenum[ALIVE3] + typenum[TIAO3];
	int alive2 = typenum[ALIVE2] + typenum[LOWALIVE2];
    if (alive2 >= 2) return 50;//˫��2
    //if (die4 >= 2 || die4 && alive3) return 10000;//˫��4 ��4��3
    //if (alive3 >= 2) return 5000;//˫��3
    //if (typenum[DIE3] && typenum[ALIVE3]) return 2000;//��3�߼���3
    return 0;
}