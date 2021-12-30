QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AIPlayer.cpp \
    ChessPad.cpp \
    ChessPiece.cpp \
    Game.cpp \
    GameWindow.cpp \
    MainWindow.cpp \
    Player.cpp \
    QtGame.cpp \
    QtGameClient.cpp \
    QtNetGame.cpp \
    QtPlayer.cpp \
    main.cpp \
    MyThread.cpp

HEADERS += \
    AIPlayer.h \
    ChessPad.h \
    ChessPiece.h \
    Game.h \
    GameWindow.h \
    MainWindow.h \
    Player.h \
    QtGame.h \
    QtGameClient.h \
    QtNetGame.h \
    QtPlayer.h \
    MyThread.h

FORMS += \
    GameWindow.ui \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_FILE += \
    icon.rc
