cmake_minimum_required(VERSION 3.5)

project (Gobang)

#set(CMAKE_INCLUDE_CURRENT_DIR ON)

#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Network Core Gui Widgets REQUIRED)

set(SOURCES
    AIPlayer.cpp
    ChessPad.cpp
    ChessPiece.cpp
    CmdGame.cpp
    CmdPlayer.cpp
    Game.cpp
    Player.cpp
    QtGame.cpp
    QtGameClient.cpp
    QtNetGame.cpp
    QtPlayer.cpp
    gamewindow.cpp
    main.cpp
    mainwindow.cpp
    MyThread.cpp
)

set(HEADERS
    AIPlayer.h
    ChessPad.h
    ChessPiece.h
    CmdGame.h
    CmdPlayer.h
    Game.h
    Player.h
    QtGame.h
    QtGameClient.h
    QtNetGame.h
    QtPlayer.h
    gamewindow.h
    mainwindow.h
    MyThread.h
)

set(FORMS
    gamewindow.ui
    mainwindow.ui
)
    qt_add_executable(Gobang
        MANUAL_FINALIZATION
        ${SOURCES}
    )
QT5_WRAP_CPP(HEADERS_MOC ${HEADERS})

QT5_WRAP_UI(FORMS_HEADERS ${FORMS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

#target_link_libraries(Gobang PRIVATE Qt6::Widgets Qt6::Gui Qt6::Network)

add_executable(Gobang ${SOURCES} ${HEADERS_MOC} ${FORMS_HEADERS})

#qt6_use_modules(Gobang Core Network Gui Widgets)

#qt_finalize_executable(Gobang)
