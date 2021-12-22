#ifndef __QT_GAME_CLIENT_H__
#define __QT_GAME_CLIENT_H__

#include "Game.h"
#include <QDataStream>
#include <QtCore>

class QtGameClient : public QObject{
    Q_OBJECT
    friend class GameWindow;
    friend class MainWindow;

public:
    void run(int8_t, QDataStream&);
    void infoTips(QDataStream&);
    void infoRecommend(QDataStream&);
    void infoGameOver(QDataStream&);
    void infoPlace(QDataStream&);
    void infoRemove(QDataStream&);



};

#endif
