#include <iostream>
#include "CmdGame.h"

using std::cin;
using std::cout;
using std::endl;

/*void CmdGame::gameInit() {
    mode = 1;
    chessPad = new ChessPad(mode);
    if (x == '1') {
        p[0] = new CmdPlayer(1);
        p[1] = new AIPlayer(2);
    }
    else if (x == '2') {
        p[0] = new AIPlayer(1);
        p[1] = new CmdPlayer(2);
    }
    else {
        p[0] = new CmdPlayer(1);
        p[1] = new CmdPlayer(2);
    }
}*/


int main() {
    while (1) {
        cout << "1���˻�\n2������\n����������" << endl;
        char x = cin.get();
        cin.get();
        cout << "0���޽���\n1������" << endl;
        char y = cin.get();
        CmdGame g(x - '0', y - '0');
        g.start();
        cout << "���س����˳���Ϸ��������1����һ��" << endl;
        if (cin.get() == '1') continue;
        else break;
    }
    return 0;
}