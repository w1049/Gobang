#include <iostream>
#include <iomanip>

#include "CmdGame.h"
#include "CmdPlayer.h"
#include "AIPlayer.h"

using std::cin;
using std::cout;
using std::endl;

CmdGame::CmdGame(int type, int mode) {
    switch (type) {
    case 1:
        p[0] = new CmdPlayer(1);
        p[1] = new AIPlayer(2);
        break;
    case 2:
        p[0] = new AIPlayer(1);
        p[1] = new CmdPlayer(2);
        break;
    default:
        p[0] = new CmdPlayer(1);
        p[1] = new CmdPlayer(2);
        break;
    }
    chessPad = new ChessPad(mode);
}

void CmdGame::start() {
    displayPad();
    char c;
    while (1) {
        cin >> c;
        if (c == 'u') { // û����⣨����һֱ���壡��
            chessPad->remove(3 - turn);
            displayPad();
            turn = 3 - turn;
        }
        else {
            int code = step();
            if (code) break;
        }
    }
}

void CmdGame::infoGameOver(int pid) {
    cout << (pid == 1 ? "��" : "��");
    if (p[pid - 1]->getType() == 0) {
        cout << "���" << (int)pid << "Ӯ��! " << endl;
    } else {
        cout << "����Ӯ�ˣ�" << endl;
    }
    
} // ��ʾ��Ϸ������չʾʤ�߻�ƽ��

const char* toS(int8_t x) {
    if (x == 0) return "++ ";
    else if (x == 1) return "�� ";
    else return "�� ";
}

void CmdGame::displayPad() {
    system("cls");
    cout << "   A  B  C  D  E  F  G  H  I  J  K  L  M  N  O" << endl;
    for (int8_t i = 0; i < 15; i++) {
        cout << std::setw(2) << (int)(i+1) << " ";
        for (int8_t j = 0; j < 15; j++)
            cout << toS(chessPad->p(i, j));
        cout << endl;
    }
}
void CmdGame::refreshPad(const ChessPiece &p) {
    system("cls");
    cout << "   A  B  C  D  E  F  G  H  I  J  K  L  M  N  O" << endl;
    for (int8_t i = 0; i < 15; i++) {
        cout << std::setw(2) << (int)(i + 1) << " ";
        for (int8_t j = 0; j < 15; j++) {
            if (i == p.getX() && j == p.getY())
                cout << "\b(" << toS(chessPad->p(i, j)) << "\b)";
            else cout << toS(chessPad->p(i, j));
        }
        cout << endl;
    }
}

CmdGame::~CmdGame() {
    delete p[0];
    delete p[1];
    delete chessPad;
}