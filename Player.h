#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED


#include "Board.h"

using namespace std;

class Player {
public:
    virtual ~Player() {}

    virtual string move() const = 0;
    virtual string name() const = 0;
};


#endif
