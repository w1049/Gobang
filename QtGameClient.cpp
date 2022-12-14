#include "QtGameClient.h"

#include "AIPlayer.h"
#include "GameWindow.h"
#include "QtPlayer.h"

namespace render {
extern QMutex drawmutex;
extern ChessPad currentPad;
extern cpv banned, win5;
extern ChessPiece rcmd;
extern int winnerid;
extern int points;
extern bool allowUndo;
}  // namespace render

using namespace render;
using namespace GameServer;

class GameWindow;
extern GameWindow* GW;
QDataStream& operator<<(QDataStream& o, const ChessPiece& p);
QDataStream& operator>>(QDataStream& o, ChessPiece& p);

void QtGameClient::run(int8_t c, QDataStream& in) {
    switch (c) {
    case REMOVE: infoRemove(in); break;
    case RECOMMEND: infoRecommend(in); break;
    case TIPS: infoTips(in); break;
    case GAMEOVER: infoGameOver(in); break;
    case PLACE: infoPlace(in); break;
    case STOPUNDO: allowUndo = 0; break;
    case START: GW->upd(1); break;
    }
}

void QtGameClient::infoRemove(QDataStream& in) {
    int8_t turn;
    in >> turn;
    // drawmutex.lock();
    currentPad.remove();
    banned.clear();
    win5.clear();
    rcmd.set(0, -1, -1);
    GW->upd(turn);  // 提示改变回合
    // drawmutex.unlock();
}

void QtGameClient::infoRecommend(QDataStream& in) {
    ChessPiece p;
    in >> p;
    // drawmutex.lock();
    rcmd = p;
    GW->setAIButtonEnabled(1);
    GW->upd();
    // drawmutex.unlock();
}

void QtGameClient::infoTips(QDataStream& in) {
    uint8_t s1, s2;
    ChessPiece p;
    // drawmutex.lock();
    in >> points;
    banned.clear();
    win5.clear();
    // read vectors and push
    in >> s1;
    for (uint8_t i = 0; i < s1; i++) in >> p, banned.push_back(p);
    in >> s2;
    for (uint8_t i = 0; i < s2; i++) in >> p, win5.push_back(p);
    GW->upd();
    // drawmutex.unlock();
}

void QtGameClient::infoGameOver(QDataStream& in) {
    int8_t id;
    in >> id;
    winnerid = id;
}

void QtGameClient::infoPlace(QDataStream& in) {
    ChessPiece p;
    int8_t turn;
    in >> p >> turn;
    // drawmutex.lock();
    currentPad.place(p);
    banned.clear();
    win5.clear();
    rcmd.set(0, -1, -1);
    GW->upd(3 - turn);  // 提示改变回合
    // drawmutex.unlock();
}
