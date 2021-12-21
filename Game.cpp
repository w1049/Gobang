#include "Game.h"
#include "Player.h"
#include "AIPlayer.h"

int Game::step() {
    ChessPiece tmp = p[turn - 1]->getNextPos(*chessPad);
    chessPad->place(tmp);
    refreshPad(tmp);
    int code = chessPad->judge(tmp);
    if (code) {
        infoGameOver(turn);
        return 1;
    }
    turn = 3 - turn;
    return 0;
}

bool Game::undo() {
    if (chessPad->getPiece().empty()) {
        return 0;
    }
    ChessPiece p = chessPad->getPiece().back();
    turn = p.getPid();
    chessPad->remove();
    displayPad();
    return 1;
}

void Game::start() {
    displayPad();
    while (1) {
        int code = 0;
        if (p[turn - 1]->getType()) {
            info(turn);
            code = p[turn - 1]->command(*chessPad);
            if (code == 1) { // undo
                if (!undo()) continue;
                if (p[turn - 1]->getType() == 0 || p[turn - 1]->getType() == 2 || p[2 - turn]->getType() == 2) if (!undo()) continue;
            }
            else if (code == 2) { // ask
                AIPlayer ai = AIPlayer(turn);
                ChessPiece p = ai.getNextPos(*chessPad);
                reco(p);
            }
        }
        if (code) continue;
        code = step();
        if (code) break;
    }
}

    // virtual infoGameOver(int8_t pid) = 0; // ��ʾ��Ϸ������չʾʤ�߻�ƽ��
    // virtual infoPlaceFailed(ChessPiece, int8_t reason) = 0; // ��ʾ�޷���������
    // virtual displayPad() = 0; // ��ʾ����. ��һ����ʾ����ʱʹ��.
    // virtual refreshPad() = 0; // ˢ������. ���ı��������.


// ��Ϸ��������������Ϸ������󲿷��������. ����μ���Ϸ����.
// �ɵ�������Player��һ��ClassPad�еĺ���.
// �ɵ���������麯���������������.
// ��Ϸ������ֱ�� return. ���Ժ��ټӻ��壩