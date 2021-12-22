#include <QApplication>

#include "QtGame.h"
#include "mainwindow.h"

MainWindow *MW;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    MW = &w;
    w.show();
    return a.exec();
}
