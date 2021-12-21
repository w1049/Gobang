#include <iostream>
#include <string>
#include <sstream>

#include "NetPlayer.h"
#include "ChessPiece.h"

using std::endl;
using std::stringstream;

extern char recv_char[256];
void send_v(const SOCKET* sock, std::string str);
void recv_v(const SOCKET* sock);

NetPlayer::NetPlayer(int p, SOCKET* sock): Player(p, 2), Tx(0), Ty(0), sock(sock) {}

ChessPiece NetPlayer::getNextPos(const ChessPad& pad) {
    return ChessPiece(pid, Tx, Ty);
}

void NetPlayer::infoFailed(const ChessPiece &p, int8_t reason) {
    stringstream s;
    s << "由于原因" << (int)reason << ", 你不能在" << char(p.getY() + 'A') << (int)p.getX() + 1 << "下棋." << endl;
    send_v(sock, s.str());
}
// ask
// undo
// H8
int NetPlayer::command(const ChessPad& pad) {
    ChessPiece p;
    int8_t reason;
    while (1) {
        std::stringstream sss;
        sss << "轮到" << (pid == 1 ? "●" : "○") << "方下棋。请输入坐标，形式为“A8”（不含引号）: " << endl;
        send_v(sock, sss.str());
        char cy;  int x;
        recv_v(sock);
        std::string str(recv_char);
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

const SOCKET* NetPlayer::getSock() {
    return sock;
}
