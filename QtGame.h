#ifndef __QT_GAME_H__
#define __QT_GAME_H__

#include "Game.h"

class QtGame : public Game {
    friend class GameWindow;
    friend class MainWindow;

public:
    QtGame(int, bool);
    void start();
    void infoTips(int);
    void infoRecommend(const ChessPiece&);
    void infoGameOver(int pid);
    void infoPlace(const ChessPiece&);
    void infoRemove();
};

#endif
