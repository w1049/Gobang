#include "Game.h"
#include "Player.h"
#include "AIPlayer.h"

bool Game::step(bool lose) {
    ChessPiece tmp = p[turn - 1]->getNextPiece(*chessPad);
    chessPad->place(tmp);
    infoPlace(tmp);
    if (chessPad->getPiecesList().size() == 225) {
        infoGameOver(3);
        return 1;
    }
    int code = chessPad->judgeWinner(tmp);
    if (code) {
        infoGameOver(turn);
        return 1;
    }
    if (lose) {
        infoGameOver(3 - turn);
        return 1;
    }
    turn = 3 - turn;
    return 0;
}

bool Game::undo() {
    if (chessPad->getPiecesList().empty()) {
        return 0;
    }
    ChessPiece p = chessPad->getPiecesList().back();
    turn = p.getPid();
    chessPad->remove();
    infoRemove();
    return 1;
}

void Game::infoRemove() {
    infoPlace(ChessPiece(0, -1, -1));
}

void Game::start() {
    infoPlace(ChessPiece(0, -1, -1));
    while (1) {
        int code = 0;
        if (p[turn - 1]->getType()) {
            infoTips(turn);
            code = p[turn - 1]->command(*chessPad);
            if (code == 1) { // undo
                if (!undo()) continue;
                if (p[turn - 1]->getType() == 0 || p[turn - 1]->getType() == 2 || p[2 - turn]->getType() == 2) if (!undo()) continue;
                continue;
            }
            else if (code == 2) { // ask
                AIPlayer ai(turn);
                ChessPiece p = ai.getNextPiece(*chessPad);
                infoRecommend(p);
                continue;
            }
        }
        if (step(code == 3)) break;
    }
}

Game::~Game() {
    delete p[0];
    delete p[1];
    delete chessPad;
}
