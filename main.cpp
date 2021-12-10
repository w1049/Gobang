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
        cout << "按回车键退出游戏，或输入1再来一局" << endl;
        if (cin.get() == '1') continue;
        else break;
    }
    return 0;
}