#ifndef REVERSI_H_INCLUDED
#define REVERSI_H_INCLUDED


//This class acts as a wrapper class for the Game class which does all of the
//actual move handling. It allows commands that are not moves to be passed from the
//game class to this classto be processed.


#include "Board.h"
#include "Player.h"
#include "Human_player.h"
#include "Computer_player.h"
#include "Game.h"
#include "Game_host.h"

#include <iostream>
#include <string>
#include <utility>

using namespace std;

//The following two values determine how many moves ahead the computer player searches
//when deciding its move. Higher values will lead to smarter play, but will make the
//computer think for longer between moves.

//BOT_SEARCH_DEPTH determines how many moves ahead the bot looks when evaluating board state
//normally. Once there are few spots left, it becomes faster and smarter to search all the way to
//the end of the game and check for winning move combinations than to evaluate the board normally,
//as the evaluation process is more performance heavy than simply checking for the winner of a
//finished game. Once the number of empty spaces on the board is less than or equal to
//BOT_END_SEARCH_DEPTH, the bot will switch to searching all the way to the end of the game.

//INSTRUCTIONS:
//When running the program normally on native linux (possibly not a VM), the following two values
//can be set as high as 10 and 15 respectively, and the computer will think for at most a few
//seconds between moves.
//
//When running the program in valgrind or in a VM, the values should be lowered to account for the
//reduced processing speed. Values of 7 and 11 respectively are sufficient for native 

const static int BOT_SEARCH_DEPTH = 7;  //Can be set to 10 on native linux when not using valgrind
const static int BOT_END_SEARCH_DEPTH = 11;  //Can be set to 15 in the same conditions as above

//If this flag is set to true, the computer will wait for the user
//to hit enter before it plays its move. If it is set to false, it will
//play as soon as it is done processing its move.
const static bool BOT_WAIT = true;

class Reversi : public Game_host {
private:
    Board _board;
    Player *_first = nullptr;
    Player *_second = nullptr;
    Game *_game = nullptr;

    bool _exit = false;

public:
    Reversi(bool default_display = true);

    ~Reversi();

    void run();  //Starts the interface
    void play();

    void choose_size();
    void choose_palette();
    void choose_players();

    void handle_command(string s);
    void list_commands() const;
};

Reversi::Reversi(bool default_display) {
    if(!default_display) {
        choose_size();
        choose_palette();
    }
}

Reversi::~Reversi() {
    if(_first) delete _first;
    if(_second) delete _second;
    assert(!_game);
}


void Reversi::run() {
    if(!_first || !_second)
        choose_players();

    cout << "Enter command \"PLAY\" to begin game, or type \"HELP\" for a command list" << endl;
    cout << "Note: all commands are case-insensitive," << endl;
    cout << "but are capitalized in instructions to stand out" << endl << endl;
    while(!_exit) {
        string input;
        getline(cin, input);
        for(char &c: input) c = toupper(c);
        handle_command(input);
    }

    _exit = false;
}

void Reversi::play() {
    _game = new Game(this, &_board, _first, _second);
    _game->play();
    delete _game;
    _game = nullptr;

    if(!_exit) {
        cout << "Enter command \"PLAY\" to begin new game, or type \"HELP\" for a command list" << endl;
    }
}


void Reversi::choose_size() {
    string selection;
    bool selected = false;
    while(!selected) {
        cout << "Select board size:" << endl << endl;

        _board.set_size(false);
        cout << "1)" << endl;
        cout << _board.board_string() << endl << endl;

        _board.set_size(true);
        cout << "2)" << endl;
        cout << _board.board_string() << endl << endl;

        getline(cin, selection);

        if(selection == "1") {
            cout << "Small board selected" << endl << endl;
            _board.set_size(false);
            selected = true;
        } else if(selection == "2") {
            cout << "Large board selected" << endl << endl;
            selected = true;
        } else {
            cout << "Invalid selection, please type \"1\" or \"2\"" << endl << endl;
        }
    }
}

void Reversi::choose_palette() {
    string selection;
    bool selected = false;
    while(!selected) {
        cout << "Select color palette:" << endl << endl;

        for(int i = 1; i <= Board::BOARD_PALETTES; i++) {
            cout << to_string(i) << ")" << endl;
            _board.set_palette(i - 1);
            cout << _board.board_string() << endl << endl;
        }

        getline(cin, selection);

        bool is_number = true;
        for(char c: selection)
            if(!isdigit(c))
                is_number = false;

        if(is_number && stoi(selection) >= 1 && stoi(selection) <= Board::BOARD_PALETTES) {
            cout << "Palette " << selection << " selected" << endl << endl;
            _board.set_palette(stoi(selection) - 1);
            selected = true;
        } else {
            cout << "Invalid selection, please enter a number from 1 to ";
            cout << Board::BOARD_PALETTES << endl << endl;
        }
    }
}

void Reversi::choose_players() {
    if(_first) delete _first;
    if(_second) delete _second;

    string selection;
    bool selected = false;
    while(!selected) {
        cout << "Select play mode:" << endl;
        cout << "1) Human vs Human" << endl;
        cout << "2) Human vs Computer" << endl;

        getline(cin, selection);

        if(selection == "1" || selection == "2") {
            selected = true;
        } else {
            cout << "Invalid selection, please type \"1\" or \"2\"" << endl << endl;
        }
    }

    if(selection == "1") {
        string name;
        cout << "Enter Player 1 name: ";
        getline(cin, name);
        _first = new Human_player(Piece::P1, name);

        cout << "Enter Player 2 name: ";
        getline(cin, name);
        _second = new Human_player(Piece::P2, name);
        cout << endl;
    }

    if(selection == "2") {
        selected = false;

        string name;
        cout << "Enter your name: ";
        getline(cin, name);

        while(!selected) {
            cout << "Would you like to play first or second?" << endl;
            cout << "1) First" << endl;
            cout << "2) Second" << endl;

            getline(cin, selection);

            if(selection == "1" || selection == "2") {
                selected = true;
            } else {
                cout << "Invalid selection, please type \"1\" or \"2\"" << endl << endl;
            }
        }

        if(selection == "1") {
            cout << "Playing first" << endl << endl;
            _first = new Human_player(Piece::P1, name);
            _second = new Computer_player(Piece::P2, &_board, BOT_SEARCH_DEPTH, BOT_END_SEARCH_DEPTH, BOT_WAIT);
        }

        if(selection == "2") {
            cout << "Playing second." << endl << endl;
            _first = new Computer_player(Piece::P1, &_board, BOT_SEARCH_DEPTH, BOT_END_SEARCH_DEPTH, BOT_WAIT);
            _second = new Human_player(Piece::P2, name);
        }
    }
}

void Reversi::handle_command(string s) {


    if(s == "HELP") {
        list_commands();
    } else if(s == "SIZE") {
        choose_size();
    } else if(s == "PALETTE") {
        choose_palette();
    } else if(s == "EXIT") {
        if(_game) {
            _game->quit();
        }
        _exit = true;
    } else if(s == "QUIT") {
        if(_game) {
            _game->quit();
        } else {
            cout << "There is no game to quit" << endl;
        }
    } else if(s == "PLAYERS") {
        if(!_game) {
            choose_players();
        } else {
            cout << "Cannot change players while in game, please quit first" << endl;
        }
    } else if(s == "PLAY") {
        if(!_game) {
            play();
        } else {
            cout << "Already in game" << endl;
        }
    } else {
        cout << "Invalid input. Type \"HELP\" for a list of commands" << endl;
    }
}

void Reversi::list_commands() const {
    string out;
    out += "HELP: Show this list\n";
    out += "EXIT: End program\n";
    out += "SIZE: Select board size\n";
    out += "PALETTE: Select color palette\n";
    out += "\n";
    out += "IN GAME COMMANDS - \n";
    out += "QUIT: Quit current game\n";
    out += "Instructions:\n";
    out += "-Type the row and column of a position to place a piece there\n";
    out += "-It does not matter whether you put the row or the column first\n";
    out += "\n";
    out += "OUT OF GAME COMMANDS - \n";
    out += "PLAYERS: Change players\n";
    out += "PLAY: Start new game\n";

    cout << endl << out << endl;

    if(_game) {
        cout << "Hit enter to continue..." << endl;
        string trash;
        getline(cin, trash);
    }
}


#endif
