#include "Player.h"

int Player::getType() { return type; }

Player::Player(int pid, int type) : pid(pid), type(type) {}

int Player::getPid() { return pid; }

int Player::command(const ChessPad&) { return 0; }

Player::~Player() {}
