#ifndef __QT_NET_GAME_H__
#define __QT_NET_GAME_H__

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>

#include "QtGame.h"

class QtNetGame : public QtGame {
    Q_OBJECT
    friend class GameWindow;
    friend class MainWindow;

public:
    QtNetGame(int, bool);

private:
    bool canUndo(int);
    void infoTips(int);
    void infoRecommend(const ChessPiece&);
    void infoGameOver(int pid);
    void infoPlace(const ChessPiece&);
    void infoRemove();

signals:
    void sendData();
};

#endif
