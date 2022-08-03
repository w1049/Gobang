## 五子棋 Gobang

#### Android 和 Qt5 放到分支里了

#### 分工情况

A：完成了基本规则与胜负判定部分。

B：完成了命令行界面与输入/输出等交互功能。

C：初步实现了棋盘的图形界面。

D：对代码进行了模块化，并完成了其余部分。

#### 项目开发环境
系统为 Windows 10，使用 QtCreator 6.0.0 与 Qt6.2.2 ，编译环境为 MinGW-W64 11.2.0 。根据构建内容、构建方式不同，可能需要不同的编译环境。

#### 项目面向环境
理论上全平台，已测试 Windows 10 x64，Ubuntu 20.04 LTS，Android 11。

#### 项目结构

##### 核心部分
```
ChessPiece.cpp
ChessPad.cpp
Player.cpp
AIPlayer.cpp
Game.cpp
```
##### 命令行界面
```
CmdGame.cpp
CmdPlayer.cpp
CmdMain.cpp
```
##### 图形界面
```
GameWindow.cpp
MainWindow.cpp
QtGame.cpp
QtNetGame.cpp
QtGameClient.cpp
QtPlayer.cpp
MyThread.cpp
main.cpp
```

#### 构建指南

##### 命令行界面

命令行界面功能不全，仅为前期测试时使用。

使用 MinGW 编译，见运行目录下的 CompileForCmd.bat 。

或使用 Visual Studio 2022 打开目录下的 cmdtest.sln。

如果要为 Linux 构建，注意修改 CmdMain 中的 `system("cls");` 语句，并且将三个 Cmd 开头的文件转为 UTF-8 编码。

##### 图形界面

可在 https://www.qt.io/download 注册账号后安装 QtCreator，安装过程中注意选择 Qt6.2.2 。安装完成后使用 QtCreator 打开 Gobang.pro 文件即可。在 IDE 中可配置编译器与平台。

Qt 程序不能静态连接 Qt 库，可以打开 Qt 6.2.2 (MinGW 9.0.0 64-bit) 后在有 Gobang.exe 的目录下运行 windeployqt Gobang.exe 来复制运行库文件。

如果在 Ubuntu 20.04 LTS 下使用 Qt5 构建（apt源似乎没有Qt6），可以做如下更改：（已在Qt5文件夹中完成）

 - 将 GameWindow.cpp 文件第248、249行的`event->position().x()`与`event->position().y()`改为`event->x()`与`event->y()`。

 - 将 MainWindow.cpp 文件第48、49行的`connect`语句改为：
```cpp
    connect(
        tcpSocket,
        static_cast<void (QAbstractSocket::*)(QAbstractSocket::SocketError)>(
            &QAbstractSocket::error),
        this,
        static_cast<void (MainWindow::*)(QAbstractSocket::SocketError)>(
            &MainWindow::displayError));
```

 - 将  CMakeList.txt 中的 Qt6 全部改为 Qt5

然后，进入源码文件夹，在终端运行以下命令：
```
sudo apt install qt5-default build-essential qtbase5-dev qttools5-dev cmake
mkdir build && cd build
cmake ..
make
```

即可使用`./Golang`打开程序。

#### 备注

icon.png icon_4x.png 以及 android-sources 文件夹是安卓的图标资源，如果需要使用请取消 .pro 文件中最后几行的注释。

构建部分没有非常详细，如果遇到问题可参考 Qt 官方文档。

图形界面部分注释比较少，因为最后有点赶，写得可能有些乱，注释来不及详细写。

