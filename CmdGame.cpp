#include <iostream>
#include <iomanip>

#include "CmdGame.h"
#include "CmdPlayer.h"
#include "AIPlayer.h"

using std::cin;
using std::cout;
using std::endl;

void CmdGame::gameInit() {
    chessPad = new ChessPad;
    cout << "1���˻�\n2������\n����������" << endl;
    char x = cin.get();
    if (x == '1') {
        p[0] = new CmdPlayer(1);
        p[1] = new AIPlayer(2);
    } else if (x == '2') {
        p[0] = new AIPlayer(1);
        p[1] = new CmdPlayer(2);
    } else {
        p[0] = new CmdPlayer(1);
        p[1] = new CmdPlayer(2);
    }
    mode = 0;
}

void CmdGame::infoGameOver(uint8_t pid) {
    cout << (pid == 1 ? "��" : "��");
    if (p[pid - 1]->getType() == 0) {
        cout << "���" << (int)pid << "Ӯ��! " << endl;
    } else {
        cout << "����Ӯ�ˣ�" << endl;
    }
    
} // ��ʾ��Ϸ������չʾʤ�߻�ƽ��

const char* toS(uint8_t x) {
    if (x == 0) return "++ ";
    else if (x == 1) return "�� ";
    else return "�� ";
}

void CmdGame::displayPad() {
    system("cls");
    cout << "   A  B  C  D  E  F  G  H  I  J  K  L  M  N  O" << endl;
    for (uint8_t i = 0; i < 15; i++) {
        cout << std::setw(2) << (int)(i+1) << " ";
        for (uint8_t j = 0; j < 15; j++)
            cout << toS(chessPad->p(i, j));
        cout << endl;
    }
}
void CmdGame::refreshPad(ChessPiece p) {
    //system("cls");
    cout << "   A  B  C  D  E  F  G  H  I  J  K  L  M  N  O" << endl;
    for (uint8_t i = 0; i < 15; i++) {
        cout << std::setw(2) << (int)(i + 1) << " ";
        for (uint8_t j = 0; j < 15; j++) {
            if (i == p.getPosX() && j == p.getPosY())
                cout << "\b(" << toS(chessPad->p(i, j)) << "\b)";
            else cout << toS(chessPad->p(i, j));
        }
        cout << endl;
    }
}

CmdGame::~CmdGame() {
    delete chessPad;
    delete p[0];
    delete p[1];
}