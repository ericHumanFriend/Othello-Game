/////////////////////////////////////////////////////////////////////////
//s
// Student Info
// ------------
//
// Name : Eric Klayne Joseph Kozak
// St.# : 301357464
// Email: ekkozak@sfu.ca
//
//
// Statement of Originality
// ------------------------
//
// All the code and comments below are my own original work. For any non-
// original work, I have provided citations in the comments with enough
// detail so that someone can see the exact source and extent of the
// borrowed work.
//
// In addition, I have not shared this work with anyone else, and I have
// not seen solutions from other students, tutors, websites, books,
// etc.
//
/////////////////////////////////////////////////////////////////////////

#include "Reversi.h"

int main() {
    //The game does not begin immediately when starting the program. Follow the instructions printed
    //to the command line for how to play.

    //Please expand your terminal enough that you don't have to scroll constantly

    //Setting this flag to true (or removing it) will start the game with default display options
    //instead of asking you to choose them before the game.
    Reversi reversi(false);
    reversi.run();
}