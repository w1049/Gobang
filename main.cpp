#include <iostream>
#include "CmdGame.h"
#include "NetGame.h"
#include "NetPlayer.h"

#include <winsock.h>
#pragma comment (lib,"ws2_32")

using std::cin;
using std::cout;
using std::endl;
using std::string;

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
char recv_char[512];
void send_v(const SOCKET *sock, string str) {
    send(*sock, str.c_str(), str.size() + sizeof(char), 0);
}
void recv_v(const SOCKET* sock) {
    recv(*sock, recv_char, 512, 0);
}

void Server() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in sockaddr;
    sockaddr.sin_family = PF_INET;
    sockaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    sockaddr.sin_port = htons(9000);
    bind(s, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR));
    listen(s, 1);
    printf("listening on port[%d].\n", 9000);
    char* msg = new char[1000];
    SOCKADDR clientAddr;
    int size = sizeof(SOCKADDR);
    SOCKET client;
    client = accept(s, &clientAddr, &size);
    send_v(&client, "HELLO! Welcome to OUR GAME!\n");
    NetPlayer p(2, &client);
    NetGame g(&p, 0);
    g.start();
}

void Client() {

}
int main() {
    while (1) {
        cout << "1���˻�\n2������\n3������\n4������\n����������" << endl;
        char x = cin.get();
        cin.get();
        if (x == '3') {
            Server();
            continue;
        }
        else if (x == '4') {
            Client();
            continue;
        }
        cout << "0���޽���\n1������" << endl;
        char y = cin.get();
        cin.get();
        CmdGame g(x - '0', y - '0');
        g.start();
        cout << "���س����˳���Ϸ��������1����һ��" << endl;
        if (cin.get() == '1') continue;
        else break;
    }
    return 0;
}