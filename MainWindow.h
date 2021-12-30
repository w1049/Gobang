#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtCore>
#include <QtNetwork>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_localButton_clicked();
    void on_serverButton_clicked();
    void on_clientButton_clicked();

protected:
    void resizeEvent(QResizeEvent *);

private:
    int orientation = 0;
    Ui::MainWindow *ui;
    void connectClient();
    void connectServer();
    QTcpServer *tcpServer = nullptr;
    void displayError(QAbstractSocket::SocketError socketError);
};
#endif  // MAINWINDOW_H
