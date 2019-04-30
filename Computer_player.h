#ifndef COMPUTER_PLAYER_H_INCLUDED
#define COMPUTER_PLAYER_H_INCLUDED


#include "Board.h"
#include "Player.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <utility>
#include <climits>

using namespace std;

//Represents a possible move, the board state that the move
//results in, and a relative value for the move that is used
//for various purposes.
struct Possibility {
    Position pos;
    Board_vec board;
    int value;

    Possibility() {}

    Possibility(int value):
    value(value)
    {}

    Possibility(Position pos, Board_vec board):
    Possibility(pos, board, INT_MIN)
    {}

    Possibility(Position pos, Board_vec board, int value):
    pos(pos),
    board(board),
    value(value)
    {}


};

class Computer_player : public Player {
private:
    Piece _piece;
    //The computer player must store a pointer to the game board in order to be able to choose a position to play
    //based on the current board state
    const Board *_board;
    int _max_depth;
    int _end_game_depth;
    bool _wait;

    mutable bool _search_to_end = false;

    string _name;

    //This 
    const int WEIGHTS[4][4] = {
        { 99,  -8,   8,   6},
        { -8, -24,  -4,  -3},
        {  8,  -4,   7,   4},
        {  6,  -3,   4,   0}
    };

    Possibility search(const Board_vec &board_state, Piece piece, int beta=INT_MAX, int alpha=INT_MIN, int depth=1) const;
    inline int evaluate(const Board_vec &board_vec, Piece piece) const;

public:
    Computer_player(Piece piece, const Board *board, int max_depth = 7, int end_game_depth = 12, bool wait=true, string name="Robo"):
    _piece(piece),
    _board(board),
    _max_depth(max_depth),
    _end_game_depth(end_game_depth),
    _wait(wait),
    _name(name)
    {}

    string move() const;
    string name() const;
};

string Computer_player::move() const {
    if(!_board->can_move(_piece))
        return "";

    if(_board->count_pieces(Piece::EMPTY) <= _end_game_depth)
        _search_to_end = true;

    Possibility poss = search(_board->get_board_vec(), _piece);

    _search_to_end = false;

    string out = to_string(poss.pos);

    if(_wait) {
        cout << "(Ready... hit enter)";
        string trash;
        getline(cin, trash);
    }

    return out;
}



string Computer_player::name() const {
    return _name;
}


//This function uses the negamax algorithm to decide on a move based on a computed value for a given board state.
//It uses alpha-beta pruning to eliminate many search patchs and dramatically reduce the search time.
//It speeds up search time further by evaluating moves that will result in the fewest possible moves for the opponent
//first in the early stages of the search. This biases the search towards moves that restrict the opponents possible
//moves, which are generally better moves, and it also reduces search time significantly by evaluating paths with
//a higher branching factor later when they can often be eliminated quickly through alpha-beta pruning.
Possibility Computer_player::search(const Board_vec &board_state, Piece piece, int beta, int alpha, int depth) const {

    if(_search_to_end) {
        if(Board::game_over(board_state)) {
            return evaluate(board_state, piece);
        }
    } else {
        if(depth == _max_depth) {
            return evaluate(board_state, piece);
        }
    }

    if(!Board::can_move(board_state, piece)) {
        return -1 * search(board_state, get_opponent(piece), -alpha, -beta, depth + 1).value;
    }

    vector<Position> possible_positions = Board::get_legal_positions(board_state, piece);
    vector<Possibility> possibilities;

    for(Position pos: possible_positions) {
        Board_vec board_next = board_state;
        Board::play(board_next, piece, pos);
        possibilities.push_back(Possibility{pos, board_next, 0});
    }

    if(depth <= _max_depth / 2) {
        for(Possibility &poss: possibilities)
            poss.value = Board::count_legal_positions(poss.board, piece);

        sort(possibilities.begin(), possibilities.end(),
        [](Possibility a, Possibility b)
        {return a.value < b.value;}
        );
    }

    Possibility max_poss(INT_MIN);

    for(Possibility poss: possibilities) {
        poss.value = -1 * search(poss.board, get_opponent(piece), -alpha, -beta, depth + 1).value;

        if(poss.value > max_poss.value) {
            max_poss = poss;

            alpha = max(alpha, poss.value);
            if(alpha >= beta) {
                break;
            }
        }
    }

    return max_poss;
}

int Computer_player::evaluate (const Board_vec &board_vec, Piece piece) const {

    //End state boards are evaluated differently than intermediate state boards.
    //Once the game is over, the board is valued very highly (effectively positive
    //infinity) if the active player has one the game, and very poorly if they have
    //lost. Among both winning and losing boards, configurations where the active
    //player has the most pieces are valued the highest, to win by as much as possible
    //or lose by as little as possible.
    if(Board::game_over(board_vec)) {
        Piece winner = Board::get_winner(board_vec);
        if(winner == Piece::EMPTY) {
            return INT_MIN / 4;
        } else {
            int active_pieces = Board::count_pieces(board_vec, piece);
            int opponent_pieces = Board::count_pieces(board_vec, get_opponent(piece));
            int modifier = active_pieces - opponent_pieces;
            if(winner == piece)
                return INT_MAX / 2 + modifier;
            if(winner == get_opponent(piece))
                return INT_MIN / 2 + modifier;
        }
    }

    //When the game is not over, the current state of the board is evaluated
    //using the position value grid (a constant member variable). Corner pieces
    //are valued very highly, and pieces adjacent to corners are generally avoided
    //unless the computer player predicts that it van
    int active_weight = 0;
    int opponent_weight = 0;

    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 8; col++) {
            int weight_row;
            int weight_col;

            //The following ternary conditions adjust the row
            //and column values to effectively mirror the position
            //value grid across the horizontal and vertical axes,
            //allowing the position weights of a full 8x8 board to
            //be stored in a 4x4 array.
            row < 4? weight_row = row : weight_row = 7 - row;
            col < 4? weight_col = col : weight_col = 7 - col;

            int weight = WEIGHTS[weight_row][weight_col];

            //Some pieces in the position value grid have negative
            //values because they can potentially grant access to the
            //corners. Once a corner has been claimed, occupying these
            //spaces no longer poses any risk. To compensate for this,
            //once a corner has been claimed, all negative values in the
            //corner's quadrant are raised to a small positive value of 1.
            bool corner_empty = false;
            if(row < 4 && col < 4) {
                corner_empty = board_vec[0][0] == Piece::EMPTY;
            } else if (row < 4 && col >= 4) {
                corner_empty = board_vec[0][7] == Piece::EMPTY;
            } else if (row >= 4 && col < 4) {
                corner_empty = board_vec[7][0] == Piece::EMPTY;
            } else {
                corner_empty = board_vec[7][7] == Piece::EMPTY;
            }

            if(!corner_empty) {
                weight = max(1, weight);
            }

            if(board_vec[row][col] == piece) {
                active_weight += weight;
            }
            if(board_vec[row][col] == get_opponent(piece)) {
                opponent_weight += weight;
            }
        }
    }

    return active_weight - opponent_weight;
}


#endif
