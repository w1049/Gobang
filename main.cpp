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
        cout << "1：人机\n2：机人\n其他：人人" << endl;
        char x = cin.get();
        cin.get();
        cout << "0：无禁手\n1：禁手" << endl;
        char y = cin.get();
        CmdGame g(x - '0', y - '0');
        g.start();
        cout << "按回车键退出游戏，或输入1再来一局" << endl;
        if (cin.get() == '1') continue;
        else break;
    }
    return 0;
}