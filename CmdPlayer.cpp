#include <iostream>

#include "CmdPlayer.h"
#include "ChessPiece.h"

using std::cin;
using std::cout;
using std::endl;

CmdPlayer::CmdPlayer(uint8_t p = 0) {
    pid = p;
}

ChessPiece CmdPlayer::getNextPos(const ChessPad* pad) {
    ChessPiece p;
    uint8_t reason;
    while (1) {
        cout << "�ֵ�" << (pid == 1 ? "��" : "��") << "�����塣���������꣬��ʽΪ��A8��\n���������ţ�����ʽ������Ҳ��֪���ᷢ��ʲô��: " << endl;
        char cy;
        int x;
        cin >> cy >> x;
        if (reason = pad->check(p = ChessPiece(pid, x-1, cy-'A'))) {
            infoFailed(p, reason);
        }
        else return p;
    }
}

void CmdPlayer::infoFailed(ChessPiece p, uint8_t reason) {
    cout << "����ԭ��" << (int)reason << ", �㲻����(" << (int)p.getPosX() << "," << (int)p.getPosY() << ")����." << endl;
}