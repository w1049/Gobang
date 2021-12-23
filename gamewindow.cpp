#include "gamewindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QMutex>
#include <QThread>
#include <string>

#include "MyThread.h"
#include "QtGameClient.h"
#include "QtNetGame.h"
#include "QtPlayer.h"
#include "mainwindow.h"
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
bool allowUndo = 1, allowAI = 1;
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
bool infoBan, infoWin1, infoWin2, ai1, ai2;
int undo1, undo2;
}  // namespace GameServer
using namespace GameServer;
void GameWindow::upd(int pid) {
    if (pid && pid != currentPid) {
        // 回合结束
        currentPid = pid;
        std::swap(currentPlayer, waitPlayer);
        if (currentPlayer) {
            setBtn(1);
            ui->label_2->setText("请落子");
        } else {
            setBtn(0);
            ui->label_2->setText("等待...");
        }
    }
    update();
}

// 模式选择
// 谁先 禁手开关 禁手显示 连5提示（1/2） 悔棋次数 允许AI
// type mode info infoWin1 infoWin2 undo1 undo2 ai1 ai2
// 垃圾处理

void GameWindow::setBtn(bool f) {
    ui->pushButton_2->setEnabled(f && allowUndo);
    ui->pushButton_3->setEnabled(f && allowAI);
}

void GameWindow::dealDone() {
    setBtn(0);
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
    if (type < 6) init();
}
void GameWindow::init() {
    if (type < 6) {
        setBtn(0);
        pro = new MyThread();
        connect(pro, &MyThread::isDone, this, &GameWindow::dealDone);
        connect(this, &GameWindow::destroyed, this, &GameWindow::stopThread);
    } else {
        ui->pushButton->setDisabled(true);
        currentPid = 1;
        allowAI = ai2;
        allowUndo = undo2;
        switch (type) {
        case 6:  // 客机白
            currentPlayer = nullptr;
            waitPlayer = new QtPlayer(2);
            setBtn(0);
            ui->label_2->setText("等待...");
            break;
        case 7:  // 客机黑
            currentPlayer = new QtPlayer(1);
            waitPlayer = nullptr;
            setBtn(1);
            ui->label_2->setText("请落子");
            break;
        }
        this->show();
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
        out.setVersion(QDataStream::Qt_4_0);
        out << (quint16)0;
        out << CLICK << (int8_t)moveX << (int8_t)moveY;
        out.device()->seek(0);
        out << (quint16)(sendBlock.size() - sizeof(quint16));
        tcpSocket->write(sendBlock);
        tcpSocket->waitForBytesWritten();
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
        setBtn(1);
        ui->label_2->setText("请落子");
        break;
    case 2:
        runningGame = new QtGame(2, mode);
        currentPlayer = nullptr;
        waitPlayer = dynamic_cast<QtPlayer *>(runningGame->p[1]);
        setBtn(0);
        ui->label_2->setText("等待...");
        break;
    case 4:  // 主机黑
        allowAI = ai1;
        allowUndo = undo1;
        runningGame = new QtNetGame(1, mode);
        currentPlayer = dynamic_cast<QtPlayer *>(runningGame->p[0]);
        waitPlayer = nullptr;
        remotePlayer = dynamic_cast<QtPlayer *>(runningGame->p[1]);
        setBtn(1);
        ui->label_2->setText("请落子");
        break;
    case 5:  // 主机白
        allowAI = ai1;
        allowUndo = undo1;
        runningGame = new QtNetGame(2, mode);
        currentPlayer = nullptr;
        waitPlayer = dynamic_cast<QtPlayer *>(runningGame->p[1]);
        remotePlayer = dynamic_cast<QtPlayer *>(runningGame->p[0]);
        setBtn(0);
        ui->label_2->setText("等待...");
        break;
    case 3:
        runningGame = new QtGame(3, mode);
        currentPlayer = dynamic_cast<QtPlayer *>(runningGame->p[0]);
        waitPlayer = dynamic_cast<QtPlayer *>(runningGame->p[1]);
        setBtn(1);
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
        if (remotePlayer && --undo1 == 0)
            allowUndo = 0, ui->pushButton_2->setEnabled(0);
    } else {
        sendBlock.clear();
        QDataStream out(&sendBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (quint16)0;
        out << COMMAND << 1;
        out.device()->seek(0);
        out << (quint16)(sendBlock.size() - sizeof(quint16));
        tcpSocket->write(sendBlock);
        tcpSocket->waitForBytesWritten();
        tcpSocket->flush();
        if (--undo2 == 0) allowUndo = 0, ui->pushButton_2->setEnabled(0);
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
        out.setVersion(QDataStream::Qt_4_0);
        out << (quint16)0;
        out << COMMAND << 2;
        out.device()->seek(0);
        out << (quint16)(sendBlock.size() - sizeof(quint16));
        tcpSocket->write(sendBlock);
        tcpSocket->waitForBytesWritten();
        tcpSocket->flush();
        enableAI(0);
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
    clientConnection->waitForBytesWritten();
    clientConnection->flush();
    blockCond.wakeAll();
    blockMutex.unlock();
    qDebug() << sendBlock.toHex();
}

void GameWindow::readDataClient() {
    if (blockSize == 0) {
        if (tcpSocket->bytesAvailable() < (int)sizeof(quint16)) return;
        in >> blockSize;
    }
    if (tcpSocket->bytesAvailable() < blockSize) return;
    qDebug() << tcpSocket->bytesAvailable() << "vs" << blockSize;
    int8_t c;
    in >> c;
    qDebug() << c;
    if (c == GAMEINFO) {
        qDebug() << "INFO!";
        in >> type >> mode >> infoBan >> infoWin1 >> infoWin2 >> ai1 >> ai2;
        in >> undo1 >> undo2;
        type += 2;
        QString str;
        str = tr("主机棋子 - ") + (type == 6 ? tr("黑\n") : tr("白\n"));
        str += tr("禁手 - ") + (mode ? tr("开\n") : tr("关\n"));
        str += tr("禁手提示 - ") + (infoBan ? tr("开\n") : tr("关\n"));
        str += tr("连五警告 - 主机") +
               (infoWin1 ? tr("开 客机") : tr("关 客机")) +
               (infoWin2 ? tr("开\n") : tr("关\n"));
        str += tr("AI 辅助 - 主机") + (ai1 ? tr("开 客机") : tr("关 客机")) +
               (ai2 ? tr("开\n") : tr("关\n"));
        str += tr("允许悔棋次数 - 主机") + QString::number(undo1) +
               tr(" 客机") + QString::number(undo2);
        QMessageBox::information(this, "服务器信息", str, QMessageBox::Yes,
                                 QMessageBox::Yes);
        init();
    } else if (c == STOPUNDO) {
        allowUndo = 0;
    } else {
        clientCtrl.run(c, in);
        if (c == GAMEOVER) {
            setBtn(0);
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
    }
    blockSize = 0;
    if (tcpSocket->bytesAvailable()) readDataClient();
}

void GameWindow::sendGameInfo() {
    sendBlock.clear();
    QDataStream out(&sendBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << GAMEINFO;
    out << type << mode << infoBan << infoWin1 << infoWin2 << ai1 << ai2;
    out << undo1 << undo2;
    out.device()->seek(0);
    out << (quint16)(sendBlock.size() - sizeof(quint16));
    clientConnection->write(sendBlock);
    clientConnection->waitForBytesWritten();
    clientConnection->flush();
    qDebug() << sendBlock.toHex();
}

void GameWindow::enableAI(bool f) {
    qDebug() << "set" << f;
    ui->pushButton_3->setEnabled(f);
}

void GameWindow::disc() {
    QMessageBox::critical(this, "连接失败", "无法连接，即将退出");
    close();
}
