#include <iostream>
#include "CmdGame.h"

using std::cin;
using std::cout;
using std::endl;

int main() {
    CmdGame g;
    while (1) {
        g.gameInit();
        g.gameMain();
        cin.get();
        cout << "���س����˳���Ϸ��������1����һ��" << endl;
        if (cin.get() == '1') continue;
        else break;
    }
    return 0;
}