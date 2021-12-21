#include <iostream>
#include <iomanip>
#include <sstream>

#include "NetGame.h"
#include "NetPlayer.h"
#include "AIPlayer.h"
#include "CmdPlayer.h"

using std::cin;
using std::cout;
using std::endl;

extern char recv_char[256];
void send_v(const SOCKET* sock, std::string str);
void recv_v(const SOCKET* sock);

NetGame::NetGame(NetPlayer* rep, int mode) {
    p[0] = new CmdPlayer(1);
    p[1] = rep;
    chessPad = new ChessPad(mode);
}

void NetGame::reco(const ChessPiece& p) {
    std::stringstream ss("");
    ss << "Recommand: " << char(p.getY() + 'A') << (int)p.getX() + 1 << endl;
    Player* player = this->p[p.getPid() - 1];
    if (player->getType() == 2) {
        send_v(dynamic_cast<NetPlayer*>(player)->getSock(), ss.str());
    }
    else cout << ss.str();
}

void NetGame::info(int pid) {
    std::stringstream ss("");
    ss << "��ǰ���ƣ�" << -AI::g(3 - turn, *chessPad) << endl;
    ChessPiece p;
    cpv banned, win5;
    for (int8_t x = 0; x < 15; x++)
        for (int8_t y = 0; y < 15; y++) {
            int code = chessPad->check(p.set(pid, x, y));
            if (code == 3 || code == 4 || code == 5) banned.push_back(p);
        }
    for (int8_t x = 0; x < 15; x++)
        for (int8_t y = 0; y < 15; y++) {
            if(!chessPad->check(p.set(3 - pid, x, y)) &&
                chessPad->judge(p)) win5.push_back(p);
        }
    if (!banned.empty()) {
        ss << "���棺���ֵ� ";
        for (auto p : banned)
            ss << char(p.getY() + 'A') << (int)p.getX() + 1 << " ";
        ss << endl;
    }
    if (!win5.empty()) {
        ss << "���棺����� ";
        for (auto p : win5)
            ss << char(p.getY() + 'A') << (int)p.getX() + 1 << " ";
        ss << endl;
    }
    Player* player = this->p[pid - 1];
    if (player->getType() == 2) {
        send_v(dynamic_cast<NetPlayer*>(player)->getSock(), ss.str());
    }
    else cout << ss.str();
}

void NetGame::infoGameOver(int pid) {
    std::stringstream ss("");
    ss << (pid == 1 ? "��" : "��");
    if (p[pid - 1]->getType() == 0) {
        ss << "����Ӯ�ˣ�" << endl;
    } else {
        ss << "���" << (int)pid << "Ӯ��! " << endl;
    }
    cout << ss.str();
    Player* player = this->p[0];
    if (player->getType() == 2) {
        send_v(dynamic_cast<NetPlayer*>(player)->getSock(), ss.str());
    }
    player = this->p[1];
    if (player->getType() == 2) {
        send_v(dynamic_cast<NetPlayer*>(player)->getSock(), ss.str());
    }
    
} // ��ʾ��Ϸ������չʾʤ�߻�ƽ��

const char* toSS(int8_t x) {
    if (x == 0) return "++";
    else if (x == 1) return "��";
    else return "��";
}

void NetGame::displayPad() {
    std::stringstream ss("");
    system("cls");
    ss << "   A  B  C  D  E  F  G  H  I  J  K  L  M  N  O" << endl;
    for (int8_t i = 0; i < 15; i++) {
        ss << std::setw(2) << (int)(i+1) << " ";
        for (int8_t j = 0; j < 15; j++)
            ss << toSS(chessPad->p(i, j)) << " ";
        ss << endl;
    }
    cout << ss.str();
    Player* player = this->p[0];
    if (player->getType() == 2) {
        send_v(dynamic_cast<NetPlayer*>(player)->getSock(), ss.str());
    }
    player = this->p[1];
    if (player->getType() == 2) {
        send_v(dynamic_cast<NetPlayer*>(player)->getSock(), ss.str());
    }
}
void NetGame::refreshPad(const ChessPiece &p) {
    std::stringstream ss("");
    system("cls");
    ss << "   A  B  C  D  E  F  G  H  I  J  K  L  M  N  O" << endl;
    for (int8_t i = 0; i < 15; i++) {
        ss << std::setw(2) << (int)(i + 1) << " ";
        for (int8_t j = 0; j < 15; j++) {
            if (i == p.getX() && j == p.getY())
                ss << "\b(" << toSS(chessPad->p(i, j)) << ")";
            else ss << toSS(chessPad->p(i, j)) << " ";
        }
        ss << endl;
    }
    cout << ss.str();
    Player* player = this->p[0];
    if (player->getType() == 2) {
        send_v(dynamic_cast<NetPlayer*>(player)->getSock(), ss.str());
    }
    player = this->p[1];
    if (player->getType() == 2) {
        send_v(dynamic_cast<NetPlayer*>(player)->getSock(), ss.str());
    }
}

NetGame::~NetGame() {
    delete p[0];
    delete p[1];
    delete chessPad;
}