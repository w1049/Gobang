#include "GameWindow.h"

#include <QBrush>
#include <QEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QMutex>
#include <QPainter>
#include <QPen>

#include "MainWindow.h"
#include "MyThread.h"
#include "QtGameClient.h"
#include "QtNetGame.h"
#include "QtPlayer.h"
#include "ui_GameWindow.h"

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
MyThread *gameThread;
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

namespace paint {
    int prex, prey;
    double ratio;
}
using namespace paint;

extern MainWindow *MW;


GameWindow::GameWindow(int type, bool mode, QWidget *parent)
    : QWidget(parent), ui(new Ui::GameWindow), type(type), mode(mode) {
    ui->setupUi(this);
    // ui->gridLayout->removeItem(ui->horizontalSpacer);
    if (type < 6) init();
}

void GameWindow::resizeEvent(QResizeEvent *) {
    if (width() < height() && orientation == 0) { // 0 means l to r
        orientation = 1; // change to up to down
        ui->gridLayout->removeWidget(ui->statusLabel);
        ui->gridLayout->removeWidget(ui->scoresLabel);
        ui->gridLayout->removeWidget(ui->startButton);
        ui->gridLayout->removeWidget(ui->undoButton);
        ui->gridLayout->removeWidget(ui->aiButton);
        ui->gridLayout->removeItem(ui->aiLayout);
        ui->gridLayout->removeItem(ui->verticalSpacer);
        ui->gridLayout->addWidget(ui->statusLabel, 0, 0, 1, 1);
        ui->gridLayout->addWidget(ui->scoresLabel, 0, 1, 1, 1);
        ui->gridLayout->addWidget(ui->startButton, 0, 2, 1, 1);
        ui->gridLayout->addWidget(ui->undoButton, 0, 3, 1, 1);
        ui->gridLayout->addWidget(ui->aiButton, 0, 4, 1, 1);
        ui->gridLayout->addLayout(ui->aiLayout, 0, 5, 1, 1);

        ui->mainLayout->removeItem(ui->gridLayout);
        ui->mainLayout->addLayout(ui->gridLayout, 1, 0, 1, 1);
    } else if (width() > height() && orientation == 1) { // 1 means up to down
        orientation = 0;
        ui->gridLayout->removeWidget(ui->statusLabel);
        ui->gridLayout->removeWidget(ui->scoresLabel);
        ui->gridLayout->removeWidget(ui->startButton);
        ui->gridLayout->removeWidget(ui->undoButton);
        ui->gridLayout->removeWidget(ui->aiButton);
        ui->gridLayout->removeItem(ui->aiLayout);
        ui->gridLayout->addItem(ui->verticalSpacer, 6, 0, 1, 1);
        ui->gridLayout->addWidget(ui->statusLabel, 0, 0, 1, 1);
        ui->gridLayout->addWidget(ui->scoresLabel, 1, 0, 1, 1);
        ui->gridLayout->addWidget(ui->startButton, 2, 0, 1, 1);
        ui->gridLayout->addWidget(ui->undoButton, 3, 0, 1, 1);
        ui->gridLayout->addWidget(ui->aiButton, 4, 0, 1, 1);
        ui->gridLayout->addLayout(ui->aiLayout, 5, 0, 1, 1);

        ui->mainLayout->removeItem(ui->gridLayout);
        ui->mainLayout->addLayout(ui->gridLayout, 0, 1, 1, 1);
    }
}

void GameWindow::init() {
    if (type < 6) {
        setButtonEnabled(0);
        gameThread = new MyThread();
        connect(gameThread, &MyThread::isDone, this, &GameWindow::dealDone);
        connect(this, &GameWindow::destroyed, this, &GameWindow::stopThread);
    } else {
        ui->startButton->setDisabled(true);
        allowAI = ai2;
        allowUndo = undo2;
        switch (type) {
        case 6:  // 客机白
            currentPid = 1;
            currentPlayer = nullptr;
            waitPlayer = new QtPlayer(2);
            setButtonEnabled(0);
            ui->statusLabel->setText("等待...");
            break;
        case 7:  // 客机黑
            currentPid = 2;
            currentPlayer = nullptr;
            waitPlayer = new QtPlayer(1);
            setButtonEnabled(0);
            ui->statusLabel->setText("等待...");
            break;
        }
        this->show();
        MW->close();
    }
}

void GameWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);  // 画板
    painter.setRenderHint(QPainter::Antialiasing, true);
    prex = ui->padLabel->x(), prey = ui->padLabel->y();
    ratio = qMin(ui->padLabel->width(), ui->padLabel->height()) / 600.0;
    painter.translate(prex, prey);
    painter.scale(ratio, ratio);
    QPen pen = painter.pen();  // 边框
    pen.setColor(QColor(0xCD, 0x78, 0x2D));
    pen.setWidth(5);
    painter.setPen(pen);

    QBrush brush;
    brush.setColor(QColor(0xFF, 0xD3, 0x9B));  // 填充
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);

    painter.drawRoundedRect(0, 0, 600, 600, 15, 15);

    pen.setColor(Qt::black);
    pen.setWidth(1);
    painter.setPen(pen);
    for (int i = 0; i < 15; i++) {
        painter.drawLine(20 + i * 40, 20, 20 + i * 40, 580);
        painter.drawLine(20, 20 + i * 40, 580, 20 + i * 40);
    }  // 棋盘线

    brush.setColor(Qt::black);
    painter.setBrush(brush);  // 黑点
    painter.drawEllipse(135, 135, 10, 10);
    painter.drawEllipse(455, 135, 10, 10);
    painter.drawEllipse(135, 455, 10, 10);
    painter.drawEllipse(455, 455, 10, 10);
    painter.drawEllipse(295, 295, 10, 10);

    if (currentPlayer && ~moveX && ~moveY)
        drawPiece(painter, pen, brush,
                  ChessPiece(currentPlayer->getPid(), moveX, moveY), 127);

    drawmutex.lock();
    ui->scoresLabel->setText(QString::number(points));
    auto v = currentPad.getPiecesList();
    if (v.size()) {  // 最后棋子的光环
        int px = v.back().getX() * 40 + 20;
        int py = v.back().getY() * 40 + 20;
        QRadialGradient radialGradient(px, py, 25, px, py);
        radialGradient.setColorAt(0.7, Qt::white);
        radialGradient.setColorAt(1.0, QColor(255, 255, 255, 0));
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(radialGradient));
        painter.drawEllipse(px - 26, py - 26, 52, 52);
        for (auto p : v) drawPiece(painter, pen, brush, p);
    }
    if (banned.size()) {  // 禁手
        for (auto p : banned) drawBanned(painter, p);
    }
    if (win5.size()) {  // 警告
        for (auto p : win5) drawWarn(painter, p);
    }
    if (rcmd.getPid()) {  // 推荐棋子
        int px = rcmd.getX() * 40 + 20;
        int py = rcmd.getY() * 40 + 20;
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
    painter.drawEllipse(p.getX() * 40 + 4, p.getY() * 40 + 4, 32, 32);
}

void GameWindow::drawBanned(QPainter &painter, const ChessPiece &p) {
    QPen pen = painter.pen();
    pen.setColor(QColor(255, 0, 0));
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(p.getX() * 40 + 5, p.getY() * 40 + 5, 30, 30);
    painter.drawLine(p.getX() * 40 + 15, p.getY() * 40 + 15, p.getX() * 40 + 25,
                     p.getY() * 40 + 25);
    painter.drawLine(p.getX() * 40 + 15, p.getY() * 40 + 25, p.getX() * 40 + 25,
                     p.getY() * 40 + 15);
}

void GameWindow::drawWarn(QPainter &painter, const ChessPiece &p) {
    int px = p.getX() * 40 + 20;
    int py = p.getY() * 40 + 20;
    QRadialGradient radialGradient(px, py, 25, px, py);
    radialGradient.setColorAt(0, Qt::red);
    radialGradient.setColorAt(1.0, QColor(255, 0, 0, 0));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(radialGradient));
    painter.drawEllipse(px - 26, py - 26, 52, 52);
}

void GameWindow::mouseMoveEvent(QMouseEvent *event) {
    // qDebug() << event->position().y()  << " " << event->position().x();
    moveX = (event->position().x() - prex) / (40 * ratio);
    moveY = (event->position().y() - prey) / (40 * ratio);
    // --moveX, --moveY;
    if (!(moveX >= 0 && moveX < 15 && moveY >= 0 && moveY < 15))
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

void GameWindow::upd(int pid) {
    if (pid && pid != currentPid) {
        // 回合结束
        currentPid = pid;
        std::swap(currentPlayer, waitPlayer);
        if (currentPlayer) {
            setButtonEnabled(1);
            ui->statusLabel->setText("请落子");
        } else {
            setButtonEnabled(0);
            ui->statusLabel->setText("等待...");
        }
    }
    update();
}

void GameWindow::setButtonEnabled(bool f) {
    ui->undoButton->setEnabled(f && allowUndo);
    ui->aiButton->setEnabled(f && allowAI);
}

void GameWindow::dealDone() {
    setButtonEnabled(0);
    ui->statusLabel->setText("游戏结束！");
    QString str;
    if (winnerid == 3)
        str = "平局！";
    else if (runningGame->p[winnerid - 1]->getType() == 0)
        str = "电脑赢了！";
    else
        str = "玩家" + QString::number(winnerid) + "赢了! ";
    QMessageBox::information(this, "游戏结束", str, QMessageBox::Yes,
                             QMessageBox::Yes);
    delete runningGame;
    delete gameThread;
    currentPlayer = nullptr;
    waitPlayer = nullptr;
}

void GameWindow::stopThread() {
    gameThread->quit();
    gameThread->wait();
    delete runningGame;
}

void GameWindow::on_startButton_clicked() {
    switch (type) {
    case 1:
        runningGame = new QtGame(1, mode);
        currentPlayer = dynamic_cast<QtPlayer *>(runningGame->p[0]);
        waitPlayer = nullptr;
        setButtonEnabled(1);
        ui->statusLabel->setText("请落子");
        break;
    case 2:
        runningGame = new QtGame(2, mode);
        currentPlayer = nullptr;
        waitPlayer = dynamic_cast<QtPlayer *>(runningGame->p[1]);
        setButtonEnabled(0);
        ui->statusLabel->setText("等待...");
        break;
    case 3:
        runningGame = new QtGame(3, mode);
        currentPlayer = dynamic_cast<QtPlayer *>(runningGame->p[0]);
        waitPlayer = dynamic_cast<QtPlayer *>(runningGame->p[1]);
        setButtonEnabled(1);
        ui->statusLabel->setText("请落子");
        break;
    case 4:  // 主机黑
        allowAI = ai1;
        allowUndo = undo1;
        runningGame = new QtNetGame(1, mode);
        currentPlayer = dynamic_cast<QtPlayer *>(runningGame->p[0]);
        waitPlayer = nullptr;
        remotePlayer = dynamic_cast<QtPlayer *>(runningGame->p[1]);
        setButtonEnabled(1);
        ui->statusLabel->setText("请落子");
        break;
    case 5:  // 主机白
        allowAI = ai1;
        allowUndo = undo1;
        runningGame = new QtNetGame(2, mode);
        currentPlayer = nullptr;
        waitPlayer = dynamic_cast<QtPlayer *>(runningGame->p[1]);
        remotePlayer = dynamic_cast<QtPlayer *>(runningGame->p[0]);
        setButtonEnabled(0);
        ui->statusLabel->setText("等待...");
        sendBlock.clear();
        QDataStream out(&sendBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (quint16)0;
        out << START;
        out.device()->seek(0);
        out << (quint16)(sendBlock.size() - sizeof(quint16));
        sendData();
        break;
    }
    currentPid = 1;
    gameThread->start();
    ui->startButton->setDisabled(true);
}

void GameWindow::on_undoButton_clicked() {
    if (!currentPlayer) return;
    if (runningGame) {
        currentPlayer->mutex.lock();
        currentPlayer->cmd = 1;
        currentPlayer->hasCmd.wakeAll();
        currentPlayer->mutex.unlock();
        if (remotePlayer && --undo1 == 0)
            allowUndo = 0, ui->undoButton->setEnabled(0);
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
        if (--undo2 == 0) allowUndo = 0, ui->undoButton->setEnabled(0);
    }
}

void GameWindow::on_aiButton_clicked() {
    if (!currentPlayer) return;
    if (runningGame) {
        currentPlayer->mutex.lock();
        currentPlayer->cmd = 2;
        currentPlayer->depth = ui->depthCombo->currentText().toInt() * 2 - 1;
        currentPlayer->hasCmd.wakeAll();
        currentPlayer->mutex.unlock();
    } else {
        sendBlock.clear();
        QDataStream out(&sendBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (quint16)0;
        out << COMMAND << 2 << ui->depthCombo->currentText().toInt() * 2 - 1;
        out.device()->seek(0);
        out << (quint16)(sendBlock.size() - sizeof(quint16));
        tcpSocket->write(sendBlock);
        tcpSocket->waitForBytesWritten();
        tcpSocket->flush();
        setAIButtonEnabled(0);
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
        if (remotePlayer->cmd == 2) in >> remotePlayer->depth;
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
    // qDebug() << sendBlock.toHex();
}

void GameWindow::readDataClient() {
    if (blockSize == 0) {
        if (tcpSocket->bytesAvailable() < (int)sizeof(quint16)) return;
        in >> blockSize;
    }
    if (tcpSocket->bytesAvailable() < blockSize) return;
    // qDebug() << tcpSocket->bytesAvailable() << "vs" << blockSize;
    int8_t c;
    in >> c;
    // qDebug() << c;
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
    } else {
        clientCtrl.run(c, in);
        if (c == GAMEOVER) {
            setButtonEnabled(0);
            QString str;
            if (winnerid == 3)
                str = "平局！";
            else
                str = "玩家" + QString::number(winnerid) + "赢了! ";
            ui->statusLabel->setText("游戏结束！");
            QMessageBox::information(this, "游戏结束", str, QMessageBox::Yes,
                                     QMessageBox::Yes);
            if (currentPlayer) delete currentPlayer;
            if (waitPlayer) delete waitPlayer;
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
    // qDebug() << sendBlock.toHex();
}

void GameWindow::setAIButtonEnabled(bool f) {
    // qDebug() << "set" << f;
    ui->aiButton->setEnabled(f);
}

void GameWindow::exitDisconnected() {
    QMessageBox::critical(this, "连接失败", "无法连接，即将退出");
    close();
}

GameWindow::~GameWindow() {
    delete ui;
    // disconnect(gameThread, &MyThread::isDone, this, &GameWindow::dealDone);
    disconnect(this, &GameWindow::destroyed, this, &GameWindow::stopThread);
}

