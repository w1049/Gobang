#include <iostream>
#include <string>
#include <sstream>

#include "CmdPlayer.h"
#include "ChessPiece.h"

using std::cin;
using std::cout;
using std::endl;

CmdPlayer::CmdPlayer(int p): Player(p, 1), Tx(0), Ty(0) {}

ChessPiece CmdPlayer::getNextPos(const ChessPad& pad) {
    return ChessPiece(pid, Tx, Ty);
}

void CmdPlayer::infoFailed(const ChessPiece &p, int8_t reason) {
    cout << "����ԭ��" << (int)reason << ", �㲻����" << char(p.getY() + 'A') << (int)p.getX() + 1 << "����." << endl;
}
// ask
// undo
// H8
int CmdPlayer::command(const ChessPad& pad) {
    ChessPiece p;
    int8_t reason;
    while (1) {
        cout << "�ֵ�" << (pid == 1 ? "��" : "��") << "�����塣���������꣬��ʽΪ��A8�����������ţ�: " << endl;
        char cy;  int x;
        std::string str;
        cin.clear(); cin.sync();
        std::getline(cin, str);
        if (str == "undo") return 1;
        else if (str == "ask") return 2;
        std::stringstream ss(str);
        ss >> cy;
        if ('a' <= cy && cy <= 'z') cy -= 'a';
        else if ('A' <= cy && cy <= 'Z') cy -= 'A';
        else continue;
        if (!isdigit(str[1])) continue;
        ss >> x;
        if (ss.fail() || x < 1 || x > 15 || cy < 0 || cy >= 15) continue;
        if (reason = pad.check(p.set(pid, x - 1, cy))) {
            infoFailed(p, reason);
        }
        else {
            Tx = p.getX(), Ty = p.getY();
            break;
        }
    }
    return 0;
}