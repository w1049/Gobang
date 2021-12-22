#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>

class MyThread : public QThread {
    Q_OBJECT

protected:
    void run();

signals:
    void isDone();  // 完成信号
};

#endif  // MYTHREAD_H
