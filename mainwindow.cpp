#include "mainwindow.h"

#include <QThread>

#include "MyThread.h"
#include "QtNetGame.h"
#include "gamewindow.h"
#include "ui_mainwindow.h"

GameWindow *GW;
namespace render {
extern QtGame *runningGame;
extern QtPlayer *currentPlayer;
extern QtPlayer *waitPlayer;
extern QMutex drawmutex;
extern ChessPad currentPad;  // only display
extern int points;
extern cpv banned;
extern cpv win5;
extern ChessPiece rcmd;
extern int winnerid;
extern MyThread *pro;
extern int currentPid;
}  // namespace render
using namespace render;
namespace GameServer {
QTcpSocket *clientConnection;
QDataStream in;
extern QtPlayer *remotePlayer;
QTcpSocket *tcpSocket;
extern bool infoBan, infoWin1, infoWin2, ai1, ai2;
extern int undo1, undo2;
}  // namespace GameServer

using namespace GameServer;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->Type_3->setChecked(true);
    ui->Type_4->setChecked(true);
    tcpServer = new QTcpServer(this);
    tcpSocket = new QTcpSocket(this);
    connect(tcpServer, &QTcpServer::newConnection, this,
            &MainWindow::connectClient);
    connect(tcpSocket, &QTcpSocket::connected, this,
            &MainWindow::connectServer);
    connect(tcpSocket, &QAbstractSocket::errorOccurred, this,
            &MainWindow::displayError);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_pushButton_clicked() {
    int type = 3;
    bool mode = ui->banCheckBox->isChecked();
    infoBan = ui->infoBanCheckBox->isChecked();
    if (ui->Type_1->isChecked()) type = 1;
    if (ui->Type_2->isChecked()) type = 2;
    if (ui->Type_3->isChecked()) type = 3;
    GW = new GameWindow(type, mode);
    GW->setAttribute(Qt::WA_DeleteOnClose);
    GW->show();
    this->close();
}

void MainWindow::connectClient() {
    clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, &QAbstractSocket::disconnected, clientConnection,
            &QObject::deleteLater);
    int type = 4, mode = ui->banCheckBox->isChecked();
    if (ui->Type_4->isChecked()) type = 4;
    if (ui->Type_5->isChecked()) type = 5;
    infoBan = ui->infoBanCheckBox->isChecked(),
    infoWin1 = ui->win1CheckBox->isChecked(),
    infoWin2 = ui->win2CheckBox->isChecked(),
    ai1 = ui->ai1CheckBox->isChecked(), ai2 = ui->ai2CheckBox->isChecked();
    undo1 = ui->undo1LineEdit->text().toInt(),
    undo2 = ui->undo2LineEdit->text().toInt();
    GW = new GameWindow(type, mode);
    connect(clientConnection, &QAbstractSocket::disconnected, GW,
            &GameWindow::disc);
    GW->setAttribute(Qt::WA_DeleteOnClose);
    connect(clientConnection, &QIODevice::readyRead, GW, &GameWindow::readData);
    in.setDevice(clientConnection);
    in.setVersion(QDataStream::Qt_4_0);
    GW->sendGameInfo();
    GW->show();
    this->close();
}

void MainWindow::connectServer() {
    GW = new GameWindow(6, 0);  // 实际上无所谓的数字
    GW->setAttribute(Qt::WA_DeleteOnClose);
    connect(tcpSocket, &QIODevice::readyRead, GW, &GameWindow::readDataClient);
    connect(tcpSocket, &QAbstractSocket::disconnected, GW, &GameWindow::disc);
    // this->close();
}

void MainWindow::on_pushButton_2_clicked() {
    if (tcpServer->isListening()) {
        tcpServer->close();
        ui->pushButton_2->setText("启动服务器");
        ui->pushButton->setEnabled(true);
        ui->pushButton_4->setEnabled(true);
        ui->hostLineEdit->setReadOnly(false);
        ui->portLineEdit->setReadOnly(false);
    } else {
        if (!tcpServer->listen(QHostAddress(ui->hostLineEdit->text()),
                               ui->portLineEdit->text().toInt())) {
            QMessageBox::critical(this, tr("Game Server"),
                                  tr("Unable to start the server: %1.")
                                      .arg(tcpServer->errorString()));
            return;
        }
        ui->pushButton_2->setText("终止服务器");
        ui->pushButton->setEnabled(false);
        ui->pushButton_4->setEnabled(false);
        ui->hostLineEdit->setReadOnly(true);
        ui->portLineEdit->setReadOnly(true);
        QMessageBox::information(this, "服务器", "成功开启服务器");
    }
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError) {
    tcpSocket->abort();  // 由于不是立即关闭，如果快速点击启动服务器会有bug
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->hostLineEdit->setEnabled(true);
    ui->portLineEdit->setEnabled(true);
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError: break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("客户端"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("客户端"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("客户端"),
                                 tr("The following error occurred: %1.")
                                     .arg(tcpSocket->errorString()));
    }
}

void MainWindow::on_pushButton_4_clicked() {
    tcpSocket->abort();
    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_4->setEnabled(false);
    ui->hostLineEdit->setEnabled(false);
    ui->portLineEdit->setEnabled(false);
    tcpSocket->connectToHost(ui->hostLineEdit->text(),
                             ui->portLineEdit->text().toInt());
}
// 清理资源：关闭时，掉线时
