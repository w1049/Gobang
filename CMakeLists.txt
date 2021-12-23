cmake_minimum_required(VERSION 3.16)

# set(CMAKE_PREFIX_PATH "F:/Qt/6.2.2/mingw_64")
# set(CMAKE_CXX_COMPILER "F:/Qt/Tools/mingw900_64/bin/g++.exe")

project(Gobang VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Release")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_FLAGS "-O3")

find_package(Qt6 COMPONENTS Widgets Network REQUIRED)

add_executable(Gobang
    icon.rc
    MainWindow.ui
    GameWindow.ui
    AIPlayer.cpp
    ChessPad.cpp
    ChessPiece.cpp
    CmdGame.cpp 
    CmdPlayer.cpp
    Game.cpp
    GameWindow.cpp
    MainWindow.cpp
    Player.cpp
    QtGame.cpp
    QtGameClient.cpp
    QtNetGame.cpp
    QtPlayer.cpp
    main.cpp
    MyThread.cpp
)

target_link_libraries(Gobang PRIVATE Qt6::Widgets Qt6::Network)
