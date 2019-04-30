#ifndef GAME_HOST_H_INCLUDED
#define GAME_HOST_H_INCLUDED

//This class is used as an interface for the Reversi class and other testing classes
//that act as hosts for the Game class.

class Game_host {
public:
    virtual ~Game_host() {}

    virtual void handle_command(string s) = 0;
};


#endif
