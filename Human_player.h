#ifndef HUMAN_PLAYER_H_INCLUDED
#define HUMAN_PLAYER_H_INCLUDED

#include "Board.h"
#include "Player.h"
#include <iostream>
#include <string>
#include <cctype>

using namespace std;

class Human_player : public Player {
private:
    Piece _piece;
    string _name;

public:
    Human_player(Piece piece, const string& name="Human"):
    _piece(piece),
    _name(name)
    {}

    string move() const;
    string name() const;

    void set_piece(Piece piece);
};

string Human_player::move() const {
    string out;
    getline(cin, out);
    return out;
}

string Human_player::name() const {
    return _name;
}


#endif
