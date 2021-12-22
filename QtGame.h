#ifndef __QT_GAME_H__
#define __QT_GAME_H__

#include "Game.h"
#include <QtCore>

class QtGame : public QObject, public Game {
    Q_OBJECT
    friend class GameWindow;
    friend class MainWindow;

public:
    QtGame() = default;
    QtGame(int, bool);
    void start();
    void infoTips(int);
    void infoRecommend(const ChessPiece&);
    void infoGameOver(int pid);
    void infoPlace(const ChessPiece&);
    void infoRemove();

signals:
    void upd(int x);
};

#endif
