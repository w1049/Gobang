#include "mainwindow.h"

#include <QThread>

#include "QtGame.h"
#include "gamewindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->Mode_0->setChecked(true);
    ui->Type_3->setChecked(true);
}

GameWindow *GW;

MainWindow::~MainWindow() {
    if (GW) delete GW;
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
