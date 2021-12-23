#include <QApplication>

#include "MainWindow.h"
#include "QtGame.h"

MainWindow *MW;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    MW = &w;
    w.show();
    return a.exec();
}
