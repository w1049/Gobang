#include "mainwindow.h"

#include <QThread>

#include "QtNetGame.h"
#include "gamewindow.h"
#include "ui_mainwindow.h"
#include "mythread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->Mode_0->setChecked(true);
    ui->Type_3->setChecked(true);
}

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
}

using namespace GameServer;

MainWindow::~MainWindow() {
    if (GW) delete GW;
    if (tcpSocket) delete tcpSocket;
    if (tcpServer) delete tcpServer;
    delete ui;
}

void MainWindow::on_pushButton_clicked() {
    int type = 3;
    bool mode = 0;
    if (ui->Mode_0->isChecked()) mode = 0;
    if (ui->Mode_1->isChecked()) mode = 1;
    if (ui->Type_1->isChecked()) type = 1;
    if (ui->Type_2->isChecked()) type = 2;
    if (ui->Type_3->isChecked()) type = 3;
    GW = new GameWindow(type, mode);
    GW->setAttribute(Qt::WA_DeleteOnClose);
    GW->show();
    this->close();
}

void MainWindow::initServer() {
    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, 9000)) {
        QMessageBox::critical(this, tr("Game Server"),
                              tr("Unable to start the server: %1.")
                              .arg(tcpServer->errorString()));
        return;
    }
}

void MainWindow::connectClient() {
    clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, &QAbstractSocket::disconnected,
            clientConnection, &QObject::deleteLater);
    int type = 4, mode = 0;
    GW = new GameWindow(type, mode);
    GW->setAttribute(Qt::WA_DeleteOnClose);
    connect(clientConnection, &QIODevice::readyRead, GW, &GameWindow::readData);
    in.setDevice(clientConnection);
    in.setVersion(QDataStream::Qt_4_0);
    GW->show();
    this->close();
}

void MainWindow::connectServer() {
    GW = new GameWindow(6, 0);
    GW->setAttribute(Qt::WA_DeleteOnClose);
    connect(tcpSocket, &QIODevice::readyRead, GW, &GameWindow::readDataClient);
    GW->show();
    this->close();
}

void MainWindow::on_pushButton_2_clicked() {
    initServer();
    connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::connectClient);
    ui->pushButton_2->setDisabled(true);
}


void MainWindow::on_pushButton_3_clicked() {
    ui->pushButton_3->setDisabled(true);
    tcpSocket->abort();
    tcpSocket->connectToHost("127.0.0.1", 9000);
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }
    ui->pushButton_3->setEnabled(true);
}


void MainWindow::on_pushButton_4_clicked() {
    tcpSocket = new QTcpSocket(this);
    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);
    connect(tcpSocket, &QTcpSocket::connected, this, &MainWindow::connectServer);
    connect(tcpSocket, &QAbstractSocket::errorOccurred, this, &MainWindow::displayError);
}

