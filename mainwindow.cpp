#include "MainWindow.h"

#include "GameWindow.h"
#include "MyThread.h"
#include "QtNetGame.h"
#include "ui_MainWindow.h"

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
extern MyThread *gameThread;
extern int currentPid;
extern int aiDepth;
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

GameWindow *GW;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    tcpServer = new QTcpServer(this);
    tcpSocket = new QTcpSocket(this);
    connect(tcpServer, &QTcpServer::newConnection, this,
            &MainWindow::connectClient);
    connect(tcpSocket, &QTcpSocket::connected, this,
            &MainWindow::connectServer);
    connect(tcpSocket, &QAbstractSocket::errorOccurred, this,
            &MainWindow::displayError);
}

void MainWindow::resizeEvent(QResizeEvent *) {
    if (width() < height() && orientation == 0) { // 0 means l to r
        orientation = 1; // change to up to down

        ui->mainLayout->removeItem(ui->localLayout);
        ui->mainLayout->removeItem(ui->op1Layout);
        ui->mainLayout->removeItem(ui->op2Layout);
        ui->mainLayout->removeItem(ui->horizontalSpacer);
        ui->mainLayout->removeItem(ui->serverLayout);
        ui->mainLayout->removeItem(ui->verticalSpacer);

        ui->serverLayout->removeWidget(ui->hostLineEdit);
        ui->serverLayout->removeWidget(ui->portLineEdit);
        ui->serverLayout->removeWidget(ui->serverButton);
        ui->serverLayout->removeWidget(ui->clientButton);
        ui->serverLayout->addWidget(ui->hostLineEdit, 0, 0, 1, 3);
        ui->serverLayout->addWidget(ui->portLineEdit, 1, 0, 1, 1);
        ui->serverLayout->addWidget(ui->serverButton, 1, 1, 1, 1);
        ui->serverLayout->addWidget(ui->clientButton, 1, 2, 1, 1);
        ui->serverLayout->setColumnStretch(0, 1);

        ui->mainLayout->addLayout(ui->localLayout, 0, 0, 2, 1);
        ui->mainLayout->addLayout(ui->op1Layout, 2, 0, 1, 1);
        ui->mainLayout->addLayout(ui->op2Layout, 3, 0, 1, 1);
        ui->mainLayout->addItem(ui->horizontalSpacer, 1, 1, 1, 1);
        ui->mainLayout->addLayout(ui->serverLayout, 4, 0, 1, 2);
        ui->mainLayout->addItem(ui->verticalSpacer, 5, 0, 1, 1);
    } else if (width() > height() && orientation == 1) { // 1 means up to down
        orientation = 0;

        ui->mainLayout->removeItem(ui->localLayout);
        ui->mainLayout->removeItem(ui->op1Layout);
        ui->mainLayout->removeItem(ui->op2Layout);
        ui->mainLayout->removeItem(ui->horizontalSpacer);
        ui->mainLayout->removeItem(ui->serverLayout);
        ui->mainLayout->removeItem(ui->verticalSpacer);

        ui->serverLayout->removeWidget(ui->hostLineEdit);
        ui->serverLayout->removeWidget(ui->portLineEdit);
        ui->serverLayout->removeWidget(ui->serverButton);
        ui->serverLayout->removeWidget(ui->clientButton);
        ui->serverLayout->addWidget(ui->hostLineEdit, 0, 0, 1, 1);
        ui->serverLayout->addWidget(ui->portLineEdit, 0, 1, 1, 1);
        ui->serverLayout->addWidget(ui->serverButton, 0, 2, 1, 1);
        ui->serverLayout->addWidget(ui->clientButton, 0, 3, 1, 1);
        ui->serverLayout->setColumnStretch(0, 3);

        ui->mainLayout->addLayout(ui->localLayout, 0, 0, 2, 1);
        ui->mainLayout->addLayout(ui->op1Layout, 0, 1, 1, 1);
        ui->mainLayout->addLayout(ui->op2Layout, 1, 1, 1, 1);
        ui->mainLayout->addItem(ui->horizontalSpacer, 1, 2, 1, 1);
        ui->mainLayout->addLayout(ui->serverLayout, 2, 0, 1, 2);
        ui->mainLayout->addItem(ui->verticalSpacer, 3, 0, 1, 1);
    }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_localButton_clicked() {
    int type = 3;
    bool mode = ui->banCheckBox->isChecked();
    infoBan = ui->infoBanCheckBox->isChecked();
    if (ui->Type_1->isChecked()) type = 1;
    if (ui->Type_2->isChecked()) type = 2;
    if (ui->Type_3->isChecked()) type = 3;
    aiDepth = ui->depthCombo->currentText().toInt() * 2 - 1;
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
            &GameWindow::exitDisconnected);
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
    connect(tcpSocket, &QAbstractSocket::disconnected, GW,
            &GameWindow::exitDisconnected);
    // this->close();
}

void MainWindow::on_serverButton_clicked() {
    if (tcpServer->isListening()) {
        tcpServer->close();
        ui->serverButton->setText("启动服务器");
        ui->localButton->setEnabled(true);
        ui->clientButton->setEnabled(true);
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
        ui->serverButton->setText("终止服务器");
        ui->localButton->setEnabled(false);
        ui->clientButton->setEnabled(false);
        ui->hostLineEdit->setReadOnly(true);
        ui->portLineEdit->setReadOnly(true);
        QMessageBox::information(this, "服务器", "成功开启服务器");
    }
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError) {
    tcpSocket->abort();  // 由于不是立即关闭，如果快速点击启动服务器会有bug
    ui->localButton->setEnabled(true);
    ui->serverButton->setEnabled(true);
    ui->clientButton->setEnabled(true);
    ui->hostLineEdit->setEnabled(true);
    ui->portLineEdit->setEnabled(true);
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError: break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, "客户端",
                                 "服务器未找到。请检查地址与端口。");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(
            this, "客户端",
            "连接被拒绝。确保服务器已开启，并检查地址与端口。 ");
        break;
    default:
        QMessageBox::information(
            this, "客户端", tr("发生错误: %1.").arg(tcpSocket->errorString()));
    }
}

void MainWindow::on_clientButton_clicked() {
    tcpSocket->abort();
    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);
    ui->localButton->setEnabled(false);
    ui->serverButton->setEnabled(false);
    ui->clientButton->setEnabled(false);
    ui->hostLineEdit->setEnabled(false);
    ui->portLineEdit->setEnabled(false);
    tcpSocket->connectToHost(ui->hostLineEdit->text(),
                             ui->portLineEdit->text().toInt());
}
