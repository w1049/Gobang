#include "gamewindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QMutex>
#include <QThread>
#include <string>

#include "QtPlayer.h"
#include "QtGameClient.h"
#include "QtNetGame.h"
#include "mainwindow.h"
#include "mythread.h"
#include "qbrush.h"
#include "qevent.h"
#include "qpainter.h"
#include "qpen.h"
#include "ui_gamewindow.h"
namespace render {
QtGame *runningGame;
QtPlayer *currentPlayer;
QtPlayer *waitPlayer;
QMutex drawmutex;
ChessPad currentPad(0);  // only display
int points = 0;
cpv banned;
cpv win5;
ChessPiece rcmd;
int winnerid;
MyThread *pro;
int currentPid;
}  // namespace render
using namespace render;
namespace GameServer {
extern QTcpSocket *clientConnection;
QtPlayer *remotePlayer;
extern QDataStream in;
QByteArray sendBlock;
QMutex blockMutex;
QWaitCondition blockCond;
QtGameClient clientCtrl;
extern QTcpSocket *tcpSocket;
}  // namespace GameServer
using namespace GameServer;
void GameWindow::upd(int pid) {
    if (pid && pid != currentPid) {
        // 回合结束
        currentPid = pid;
        std::swap(currentPlayer, waitPlayer);
        if (currentPlayer) {
            ui->pushButton_2->setEnabled(true);
            ui->pushButton_3->setEnabled(true);
            ui->label_2->setText("请落子");
        } else {
            ui->pushButton_2->setEnabled(false);
            ui->pushButton_3->setEnabled(false);
            ui->label_2->setText("等待...");
        }
    }
    update();
}

// 模式选择
// 垃圾处理

void GameWindow::dealDone() {
    ui->pushButton_2->setDisabled(true);
    ui->pushButton_3->setDisabled(true);
    ui->label_2->setText("游戏结束！");
    delete runningGame;
    delete pro;
    currentPlayer = nullptr;
    waitPlayer = nullptr;
    QString str;
    if (winnerid == 3)
        str = "平局！";
    else if (runningGame->p[winnerid - 1]->getType() == 0)
        str = "电脑赢了！";
    else
        str = "玩家" + QString::number(winnerid) + "赢了! ";
    QMessageBox::information(this, "游戏结束", str, QMessageBox::Yes,
                             QMessageBox::Yes);
}

GameWindow::GameWindow(int type, bool mode, QWidget *parent)
    : QWidget(parent), ui(new Ui::GameWindow), type(type), mode(mode) {
    ui->setupUi(this);
    setMouseTracking(true);
    if (type < 6) {
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
        pro = new MyThread();
        connect(pro, &MyThread::isDone, this, &GameWindow::dealDone);
        connect(this, &GameWindow::destroyed, this, &GameWindow::stopThread);
    } else {
        ui->pushButton->setDisabled(true);
        switch (type) {
        case 6:
            currentPlayer = nullptr;
            waitPlayer = new QtPlayer(2);
            ui->pushButton_2->setEnabled(false);
            ui->pushButton_3->setEnabled(false);
            ui->label_2->setText("等待...");
            break;
        case 7:
            currentPlayer = new QtPlayer(1);
            waitPlayer = nullptr;
            ui->pushButton_2->setEnabled(true);
            ui->pushButton_3->setEnabled(true);
            ui->label_2->setText("请落子");
            break;
        }
        currentPid = 1;
    }
}

void GameWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);  // 画板
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen = painter.pen();  // 边框
    pen.setColor(QColor(0xCD, 0x78, 0x2D));
    pen.setWidth(5);
    painter.setPen(pen);

    QBrush brush;
    brush.setColor(QColor(0xFF, 0xD3, 0x9B));  // 填充
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);

    painter.drawRoundedRect(20, 40, 600, 600, 15, 15);

    pen.setColor(Qt::black);
    pen.setWidth(1);
    painter.setPen(pen);
    for (int i = 0; i < 15; i++) {
        painter.drawLine(40 + i * 40, 60, 40 + i * 40, 620);
        painter.drawLine(40, 60 + i * 40, 600, 60 + i * 40);
    }

    brush.setColor(Qt::black);
    painter.setBrush(brush);
    painter.drawEllipse(155, 175, 10, 10);
    painter.drawEllipse(475, 175, 10, 10);
    painter.drawEllipse(155, 495, 10, 10);
    painter.drawEllipse(475, 495, 10, 10);
    painter.drawEllipse(315, 335, 10, 10);

    if (currentPlayer && ~moveX && ~moveY)
        drawPiece(painter, pen, brush,
                  ChessPiece(currentPlayer->getPid(), moveX, moveY), 127);

    drawmutex.lock();
    ui->label->setText(QString::number(points));
    auto v = currentPad.getPiecesList();
    if (v.size()) {
        int px = v.back().getX() * 40 + 40;
        int py = v.back().getY() * 40 + 60;
        QRadialGradient radialGradient(px, py, 25, px, py);
        radialGradient.setColorAt(0.7, Qt::white);
        radialGradient.setColorAt(1.0, QColor(255, 255, 255, 0));
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(radialGradient));
        painter.drawEllipse(px - 26, py - 26, 52, 52);
        for (auto p : v) drawPiece(painter, pen, brush, p);
    }
    if (banned.size()) {
        for (auto p : banned) drawBanned(painter, p);
    }
    if (win5.size()) {
        for (auto p : win5) drawWarn(painter, p);
    }
    if (rcmd.getPid()) {
        int px = rcmd.getX() * 40 + 40;
        int py = rcmd.getY() * 40 + 60;
        pen.setColor(Qt::yellow);
        pen.setWidth(2);
        painter.setPen(pen);
        painter.drawLine(px - 10, py - 10, px - 5, py - 10);
        painter.drawLine(px + 10, py - 10, px + 5, py - 10);
        painter.drawLine(px - 10, py - 10, px - 10, py - 5);
        painter.drawLine(px - 10, py + 10, px - 10, py + 5);
        painter.drawLine(px + 10, py + 10, px + 5, py + 10);
        painter.drawLine(px - 10, py + 10, px - 5, py + 10);
        painter.drawLine(px + 10, py + 10, px + 10, py + 5);
        painter.drawLine(px + 10, py - 10, px + 10, py - 5);
    }
    drawmutex.unlock();
}

void GameWindow::drawPiece(QPainter &painter, QPen &pen, QBrush &brush,
                           const ChessPiece &p, int alpha) {
    int c = p.getPid() == 1 ? 0 : 255;
    brush.setColor(QColor(c, c, c, alpha));
    pen.setColor(QColor(0, 0, 0, alpha));
    painter.setBrush(brush);
    painter.setPen(pen);
    painter.drawEllipse(p.getX() * 40 + 24, p.getY() * 40 + 44, 32, 32);
}

void GameWindow::drawBanned(QPainter &painter, const ChessPiece &p) {
    QPen pen = painter.pen();
    pen.setColor(QColor(255, 0, 0));
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(p.getX() * 40 + 25, p.getY() * 40 + 45, 30, 30);
    painter.drawLine(p.getX() * 40 + 35, p.getY() * 40 + 55, p.getX() * 40 + 45,
                     p.getY() * 40 + 65);
    painter.drawLine(p.getX() * 40 + 35, p.getY() * 40 + 65, p.getX() * 40 + 45,
                     p.getY() * 40 + 55);
}

void GameWindow::drawWarn(QPainter &painter, const ChessPiece &p) {
    int px = p.getX() * 40 + 40;
    int py = p.getY() * 40 + 60;
    QRadialGradient radialGradient(px, py, 25, px, py);
    radialGradient.setColorAt(0, Qt::red);
    radialGradient.setColorAt(1.0, QColor(255, 0, 0, 0));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(radialGradient));
    painter.drawEllipse(px - 26, py - 26, 52, 52);
}

void GameWindow::mouseMoveEvent(QMouseEvent *event) {
    // qDebug() << event->position().y()  << " " << event->position().x();
    moveY = (event->position().y()) / 40;
    moveX = (event->position().x() + 20) / 40;
    --moveY, --moveX;
    if (moveX >= 0 && moveX < 15 && moveY >= 0 && moveY < 15)
        ;
    else
        moveX = moveY = -1;
    update();
}

QDataStream &operator<<(QDataStream &, const ChessPiece &);

void GameWindow::mouseReleaseEvent(QMouseEvent *) {
    if (!currentPlayer) return;
    if (runningGame) {
        currentPlayer->mutex.lock();
        currentPlayer->cmd = 0;
        currentPlayer->Tx = moveX;
        currentPlayer->Ty = moveY;
        currentPlayer->hasCmd.wakeAll();
        currentPlayer->mutex.unlock();
    } else {
        sendBlock.clear();
        QDataStream out(&sendBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_10);
        out << (quint16)0;
        out << CLICK << (int8_t)moveX << (int8_t)moveY;
        out.device()->seek(0);
        out << (quint16)(sendBlock.size() - sizeof(quint16));
        tcpSocket->write(sendBlock);
        tcpSocket->flush();
    }
}

void GameWindow::stopThread() {
    pro->quit();
    pro->wait();
    delete runningGame;
}

GameWindow::~GameWindow() {
    delete ui;
    // disconnect(pro, &MyThread::isDone, this, &GameWindow::dealDone);
    disconnect(this, &GameWindow::destroyed, this, &GameWindow::stopThread);
}

void GameWindow::on_pushButton_clicked() {
    switch (type) {
    case 1:
        runningGame = new QtGame(1, mode);
        currentPlayer = dynamic_cast<QtPlayer *>(runningGame->p[0]);
        waitPlayer = nullptr;
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        ui->label_2->setText("请落子");
        break;
    case 2:
        runningGame = new QtGame(2, mode);
        currentPlayer = nullptr;
        waitPlayer = dynamic_cast<QtPlayer *>(runningGame->p[1]);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
        ui->label_2->setText("等待...");
        break;
    case 4:
        runningGame = new QtNetGame(1, mode);
        currentPlayer = dynamic_cast<QtPlayer *>(runningGame->p[0]);
        waitPlayer = nullptr;
        remotePlayer = dynamic_cast<QtPlayer *>(runningGame->p[1]);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        ui->label_2->setText("请落子");
        break;
    case 5:
        runningGame = new QtNetGame(2, mode);
        currentPlayer = nullptr;
        waitPlayer = dynamic_cast<QtPlayer *>(runningGame->p[1]);
        remotePlayer = dynamic_cast<QtPlayer *>(runningGame->p[0]);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
        ui->label_2->setText("等待...");
        break;
    case 3:
        runningGame = new QtGame(3, mode);
        currentPlayer = dynamic_cast<QtPlayer *>(runningGame->p[0]);
        waitPlayer = dynamic_cast<QtPlayer *>(runningGame->p[1]);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        ui->label_2->setText("请落子");
        break;
    }
    currentPid = 1;
    pro->start();
    ui->pushButton->setDisabled(true);
}

void GameWindow::on_pushButton_2_clicked() {
    if (!currentPlayer) return;
    if (runningGame) {
        currentPlayer->mutex.lock();
        currentPlayer->cmd = 1;
        currentPlayer->hasCmd.wakeAll();
        currentPlayer->mutex.unlock();
    } else {
        sendBlock.clear();
        QDataStream out(&sendBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_10);
        out << (quint16)0;
        out << COMMAND << 1;
        out.device()->seek(0);
        out << (quint16)(sendBlock.size() - sizeof(quint16));
        tcpSocket->write(sendBlock);
        tcpSocket->flush();
    }
}

void GameWindow::on_pushButton_3_clicked() {
    if (!currentPlayer) return;
    if (runningGame) {
        currentPlayer->mutex.lock();
        currentPlayer->cmd = 2;
        currentPlayer->hasCmd.wakeAll();
        currentPlayer->mutex.unlock();
    } else {
        sendBlock.clear();
        QDataStream out(&sendBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_10);
        out << (quint16)0;
        out << COMMAND << 2;
        out.device()->seek(0);
        out << (quint16)(sendBlock.size() - sizeof(quint16));
        tcpSocket->write(sendBlock);
        tcpSocket->flush();
    }
}

qint16 blockSize = 0;
void GameWindow::readData() {
    if (blockSize == 0) {
        if (clientConnection->bytesAvailable() < (int)sizeof(quint16)) return;
        in >> blockSize;
    }
    if (clientConnection->bytesAvailable() < blockSize) return;
    int8_t c;
    in >> c;
    remotePlayer->mutex.lock();
    if (c == COMMAND) {
        in >> remotePlayer->cmd;
        remotePlayer->hasCmd.wakeAll();
    } else if (c == CLICK) {
        remotePlayer->cmd = 0;
        in >> remotePlayer->Tx;
        in >> remotePlayer->Ty;
        remotePlayer->hasCmd.wakeAll();
    }
    blockSize = 0;
    remotePlayer->mutex.unlock();
}

void GameWindow::sendData() {
    blockMutex.lock();
    clientConnection->write(sendBlock);
    clientConnection->flush();
    blockCond.wakeAll();
    blockMutex.unlock();
}

void GameWindow::readDataClient() {
    if (blockSize == 0) {
        if (tcpSocket->bytesAvailable() < (int)sizeof(quint16)) return;
        in >> blockSize;
    }
    if (tcpSocket->bytesAvailable() < blockSize) return;

    int8_t c;
    in >> c;
    // qDebug() << (int)c;
    clientCtrl.run(c, in);
    if (c == GAMEOVER) {
        ui->pushButton_2->setDisabled(true);
        ui->pushButton_3->setDisabled(true);
        if (currentPlayer) delete currentPlayer;
        if (waitPlayer) delete waitPlayer;
        QString str;
        if (winnerid == 3)
            str = "平局！";
        else
            str = "玩家" + QString::number(winnerid) + "赢了! ";
        ui->label_2->setText("游戏结束！");
        QMessageBox::information(this, "游戏结束", str, QMessageBox::Yes,
                                 QMessageBox::Yes);
    }
    blockSize = 0;
}
