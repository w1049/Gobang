#include <iostream>

#include "CmdGame.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;

int main() {
    while (1) {
        cout << "1���˻�\n2������\n����������" << endl;
        char x = cin.get();
        cin.get();
        cout << "0���޽���\n1������" << endl;
        char y = cin.get();
        cin.get();
        CmdGame g(x - '0', y - '0');
        g.start(1);
        cout << "���س����˳���Ϸ��������1����һ��" << endl;
        if (cin.get() == '1')
            continue;
        else
            break;
    }
    return 0;
}