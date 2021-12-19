#include "Game.h"
#include "Player.h"

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
    
    // virtual infoGameOver(int8_t pid) = 0; // ��ʾ��Ϸ������չʾʤ�߻�ƽ��
    // virtual infoPlaceFailed(ChessPiece, int8_t reason) = 0; // ��ʾ�޷���������
    // virtual displayPad() = 0; // ��ʾ����. ��һ����ʾ����ʱʹ��.
    // virtual refreshPad() = 0; // ˢ������. ���ı��������.


// ��Ϸ��������������Ϸ������󲿷��������. ����μ���Ϸ����.
// �ɵ�������Player��һ��ClassPad�еĺ���.
// �ɵ���������麯���������������.
// ��Ϸ������ֱ�� return. ���Ժ��ټӻ��壩