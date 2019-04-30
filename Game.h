#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED


#include "Board.h"
#include "Player.h"
#include "Game_host.h"

#include <iostream>
#include <string>
#include <cctype>
#include <cassert>
#include <algorithm>

using namespace std;

enum class End_state {
    P1_WIN, P2_WIN, DRAW, QUIT
};

class Game {
private:
    Game_host *_host;
    Board *_board;
    Player *_first;
    Player *_second;
    bool _skip_no_moves;  //Determines whether to wait for user input when no moves are available

    bool _quit = false;
    Piece _active_player = Piece::P1;

    Player* get_player(Piece piece) const;
    void next_turn();
    void print_score();

public:
    Game(Game_host *host, Board *board, Player *first, Player *second, bool skip_no_moves);

    End_state play();
    //Plays the game while only printing the board once the game is finished,
    //Used for testing the computer player
    End_state play_silent();
    void quit();  //Ends the current game in progress early
};

//Private methods

Player* Game::get_player(Piece piece) const {
    assert(piece != Piece::EMPTY);
    if(piece == Piece::P1) return _first;
    else return _second;
}

void Game::next_turn() {
    _active_player = get_opponent(_active_player);
}

void Game::print_score() {
    cout << "Pieces:" << endl;
    cout << _first->name() << ": " << _board->count_pieces(Piece::P1) << "  |  ";
    cout << _second->name() << ": " << _board->count_pieces(Piece::P2) << endl << endl;
}


//Public mehods

Game::Game(Game_host *host, Board *board, Player *first, Player *second, bool skip_no_moves=false):
    _host(host),
    _board(board),
    _first(first),
    _second(second),
    _skip_no_moves(skip_no_moves)
{}


End_state Game::play() {
    _board->reset();

    while(!_board->game_over() && !_quit) {
        cout << endl;
        cout << _board->board_string(_active_player) << endl << endl;
        print_score();
        Player *player = get_player(_active_player);

        if(_board->can_move(_active_player)) {
            cout << "Go " << player->name() << ": " << endl;

            string command = player->move();

            for(char &c: command) c = toupper(c);

            bool is_move = false;
            int row = 0;
            int col = 0;

            if(command.length() == 2) {
                if(isdigit(command[0]) && isalpha(command[1])) {
                    row = command[0] - '1';
                    col = command[1] - 'A';
                    is_move = true;
                } else if(isalpha(command[0]) && isdigit(command[1])) {
                    row = command[1] - '1';
                    col = command[0] - 'A';
                    is_move = true;
                }
            }

            if(is_move) {
                if(row >= 0 && row <= 7 && col >= 0 && col <= 7) {
                    cout << "Played: " << command << endl;

                    Position pos(row, col);
                    if(_board->is_legal(_active_player, pos)) {
                        int flipped = _board->play(_active_player, pos);
                        cout << "Flipped: " << flipped << endl;
                        next_turn();

                    } else {
                        cout << "Illegal move, please try again" << endl;
                    }
                } else {
                    cout << "Location out of range, please try again" << endl;
                }
            } else {
                _host->handle_command(command);
            }
        } else {
            cout << "No legal moves for " << player->name() << endl;
            if(_skip_no_moves) {
                next_turn();
            } else {
                cout << "Hit enter to continue" << endl;

                string command = player->move();
                if(command == "") {
                    next_turn();
                } else {
                    for(char &c: command) c = toupper(c);
                    _host->handle_command(command);       
                }
            }
        }
    }

    if(_quit) {
        _quit = false;
        return End_state::QUIT;
    } else {
        cout << _board->board_string() << endl;
        print_score();
        cout << "Game over" << endl;
        int first_score = _board->count_pieces(Piece::P1);
        int second_score = _board->count_pieces(Piece::P2);
        if(first_score > second_score) {
            cout << _first->name() << " wins!" << endl;
            return End_state::P1_WIN;
        } else if(first_score < second_score) {
            cout << _second->name() << " wins!" << endl;
            return End_state::P2_WIN;
        } else {
            cout << "Draw!" << endl;
            return End_state::DRAW;
        }
    }
}

End_state Game::play_silent() {
    _board->reset();

    while(!_board->game_over()) {
        if(_board->can_move(_active_player)) {
            Player *player = get_player(_active_player);
            string command = player->move();

            for(char &c: command) c = toupper(c);

            if(command.length() == 2 && isalpha(command[0]) && isdigit(command[1])) {
                int col = command[0] - 'A';
                int row = command[1] - '1';

                if(row >= 0 && row <= 7 && col >= 0 && col <= 7) {
                    Position pos(row, col);
                    if(_board->is_legal(_active_player, pos)) {
                        _board->play(_active_player, pos);
                        next_turn();
                    } else {
                        cmpt::error("Illegal move by computer: " + command);
                    }
                } else {
                    cmpt::error("Illegal placement by computer: " + command);
                }
            } else {
                cmpt::error("Invalid command by computer: " + command);
            }
        } else {
            next_turn();
        }
    }

    cout << _board->board_string() << endl;
    print_score();
    int first_score = _board->count_pieces(Piece::P1);
    int second_score = _board->count_pieces(Piece::P2);
    if(first_score > second_score) {
        cout << _first->name() << " wins!" << endl << endl;
        return End_state::P1_WIN;
    } else if(first_score < second_score) {
        cout << _second->name() << " wins!" << endl << endl;
        return End_state::P2_WIN;
    } else {
        cout << "Draw!" << endl << endl;
        return End_state::DRAW;
    }
}

void Game::quit() {
    _quit = true;
}

#endif
