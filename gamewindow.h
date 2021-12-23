#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>

#include "ChessPad.h"
#include "QtGame.h"
#include "QtPlayer.h"

namespace Ui {
class GameWindow;
}

class GameWindow : public QWidget {
    Q_OBJECT

public:
    explicit GameWindow(int type, bool mode, QWidget *parent = nullptr);
    ~GameWindow();
    void upd(int = 0);
    void dealDone();
    void sendGameInfo();
    void setAIButtonEnabled(bool);

protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void on_startButton_clicked();
    void on_undoButton_clicked();
    void on_aiButton_clicked();

public slots:
    void readData();
    void sendData();
    void readDataClient();
    void exitDisconnected();

private:
    void init();
    Ui::GameWindow *ui;
    int moveX = -1;
    int moveY = -1;
    int type, mode;
    void drawPiece(QPainter &, QPen &, QBrush &, const ChessPiece &, int = 255);
    void drawBanned(QPainter &, const ChessPiece &);
    void drawWarn(QPainter &, const ChessPiece &);
    void stopThread();
    void setButtonEnabled(bool);
};

#endif  // GAMEWINDOW_H
