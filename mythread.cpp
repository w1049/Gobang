#include "MyThread.h"

#include "QtGame.h"
namespace render {
extern QtGame* runningGame;
}

void MyThread::run() {
    render::runningGame->start();
    emit isDone();  //发送完成信号
}
