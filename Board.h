#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED


#include "cmpt_error.h"
#include <cstdlib>
#include <string>
#include <vector>
#include <cassert>

using namespace std;

const static string RESET = "\033[0m";
const static string BLINK = "\033[5m";
const static string BLINK_OFF = "\033[25m";

struct Position {
    unsigned char row = -1;
    unsigned char col = -1;

    Position() {}

    Position(unsigned char row, unsigned char col):
    row(row),
    col(col)
    {}

    Position(const Position &other, const int (&offset)[2]):
    Position(other.row, other.col)
    {
        row += offset[0];
        col += offset[1];
    }
};

string to_string(Position pos) {
    string out;
    out += 'A' + pos.col;
    out += '1' + pos.row;

    return out;
}

//These are the possible states that can be stored on the board,
//and they are also used to keep track of which player controls
//which pieces.
enum class Piece : char {
    EMPTY, P1, P2
};

inline Piece get_piece(bool first_player) {
    if(first_player) return Piece::P1;
    else return Piece::P2;
}

inline Piece get_opponent(Piece player) {
    switch(player) {
        case Piece::P1: return Piece::P2;
        case Piece::P2: return Piece::P1;
        case Piece::EMPTY: return Piece::EMPTY;
        //Default case is unreachable
        default: assert(false); return Piece::EMPTY;
    }
}

typedef vector<vector<Piece>> Board_vec;

class Board {
private:
    //Game state:
    Board_vec _board;

    //Display settings:
    bool _large_board = false;
    string _BG_COLOR;
    string _BOARD_COLOR;
    string _EMPTY_COLOR;
    string _P1_COLOR;
    string _P2_COLOR;

    //Small Board Drawing Instructions:
    const string _S_EMPTY = "·";
    const string _S_POSSIBLE = "•";
    const string _S_Piece = "■";
    const string _S_COL_LABELS = 
    "  A B C D E F G H  ";
    const string _BOARD_TEMPLATE[10] = {
    "╔═════════════════╗",
    "║ # # # # # # # # ║",
    "║ # # # # # # # # ║",
    "║ # # # # # # # # ║",
    "║ # # # # # # # # ║",
    "║ # # # # # # # # ║",
    "║ # # # # # # # # ║",
    "║ # # # # # # # # ║",
    "║ # # # # # # # # ║",
    "╚═════════════════╝"
    };

    //Large Board Drawing Instructions:
    const string _L_COL_LABELS = "   A  B  C  D  E  F  G  H   ";
    const string _BOARD_TOP =    "╔══════════════════════════╗";
    const string _BOARD_BOTTOM = "╚══════════════════════════╝";

    const string _L_POSSIBLE[2] = {
        " _ ",
        "   "
    };
    const string _L_EMPTY[2] = {
        "┌─┐",
        "└─┘"
    };
    const string _L_Piece[2] = {
        "╔═╗",
        "╚═╝"
    };


    //Display helper methods
    int color_code(vector<int> rgb) const;
    string foreground(int grayscale) const;
    string foreground(vector<int> rgb) const;
    string background(int grayscale) const;
    string background(vector<int> rgb) const;

    //Board display methods
    string get_board_small(Piece active_player) const;
    string get_board_large(Piece active_player) const;


    //Note: the definitions for the following two functions are located directly after the definitions
    //for count_move and play respectively, as they are essentially a recursive sub-part of these other
    //functions but should not be public.
    static int count_move_line(const Board_vec &board, Piece piece, Position pos, const int (&offset)[2]);
    static int flip_line(Board_vec &board, Piece piece, Position pos, const int (&offset)[2]);

public:
    const static int BOARD_PALETTES = 3;  //The number of colour palettes available

    Board();
    Board(bool large_board, int palette);

    Board_vec get_board_vec() const;  //Returns a copy of the underlying Board_vec

    string board_string() const;  //Gets the display string for the board without showing legal moves
    string board_string(Piece piece) const;  //Gets the display string showing legal moves for the given player

    void set_size(bool set_large);  //Allows the user to set the size of the board
    void set_palette(int palette);  //Allows the user to choose the board's colour palette

    void reset();

    //Non-modifying functions for evaluating board state
    //All of these functions call static functions of the same name with the current Board_vec
    //as the first argument
    //Their names and return types should make them self explanatory
    bool is_legal(Piece active_player, Position pos) const;
    int count_legal_positions(Piece active_player) const;
    bool can_move(Piece piece) const;
    bool game_over() const;
    int count_pieces(Piece piece) const;

    int play(Piece piece, Position pos);  //The return value is the number of pieces flipped

    //These functions are all static to allow the computer player to use them to evaluate
    //future board states, without having to make a copy of the entire board object
    static vector<Position> get_legal_positions(const Board_vec &board, Piece piece);
    static int count_legal_positions(const Board_vec &board, Piece piece);
    static bool can_move(const Board_vec &board, Piece piece);
    //Counts the number of pieces that would be flipped if a given move were to be played
    static int count_move(const Board_vec &board, Piece piece, Position pos);

    static int play(Board_vec &board, Piece piece, Position pos);
    static int count_pieces(const Board_vec &board, Piece piece);

    static bool game_over(const Board_vec &board);
    static Piece get_winner(const Board_vec &board);
};

//Private Methods:

int Board::color_code(vector<int> rgb) const {
    int r = rgb[0];
    int g = rgb[1];
    int b = rgb[2];

    if((r < 0) || (r > 5) ||
        (g < 0) || (g > 5) ||
        (b < 0) || (b > 5))
        cmpt::error("RGB value out of range");

    return 16 + (36 * r) + (6 * g) + b;
}

string Board::foreground(int grayscale) const {
    if((grayscale < 0) || (grayscale > 23))
        cmpt::error("Grayscale value out of range");

    string graycode = to_string(grayscale + 232);
    return "\033[38;5;" + graycode + "m";
}

string Board::foreground(vector<int> rgb) const {
    return "\033[38;5;" + to_string(color_code(rgb)) + "m";
}

string Board::background(int grayscale) const {
    if((grayscale < 0) || (grayscale > 23))
        cmpt::error("Grayscale value out of range");

    string graycode = to_string(grayscale + 232);
    return "\033[48;5;" + graycode + "m";
}

string Board::background(vector<int> rgb) const {
    return "\033[48;5;" + to_string(color_code(rgb)) + "m";
}

string Board::get_board_small(Piece active_player) const {
    string out;
    string next_line = " " + _S_COL_LABELS;
    out += next_line + "\n";

    next_line = " " + _BG_COLOR + _BOARD_COLOR + _BOARD_TEMPLATE[0] + RESET;
    out += next_line + "\n";

    for(int row = 0; row < 8; row++) {
        string row_label(1, '1' + row);

        next_line = row_label + _BG_COLOR + _BOARD_COLOR +
            _BOARD_TEMPLATE[row + 1] + RESET + row_label;

        for(int col = 0; col < 8; col++) {
            int Piece_location = next_line.find("#");
            next_line.erase(Piece_location, 1);

            string tile = "";

            if(is_legal(active_player, Position(row, col))) {
                if(active_player == Piece::P1) {
                    tile = _P1_COLOR + BLINK + _S_POSSIBLE + BLINK_OFF;
                } else {
                    tile = _P2_COLOR + BLINK + _S_POSSIBLE + BLINK_OFF;
                }
            } else {
                switch(_board[row][col]) {
                    case Piece::EMPTY:
                        tile = _EMPTY_COLOR + _S_EMPTY;
                        break;
                    case Piece::P1:
                        tile = _P1_COLOR + _S_Piece;
                        break;
                    case Piece::P2:
                        tile = _P2_COLOR + _S_Piece;
                        break;
                    default:
                        cmpt::error("Invalid board status value");
                }
            }

            next_line.insert(Piece_location, tile + _BOARD_COLOR);
        }

        out += next_line + "\n";
    }

    next_line = " " + _BG_COLOR + _BOARD_COLOR + _BOARD_TEMPLATE[9] + RESET;
    out += next_line + "\n";

    next_line = " " + _S_COL_LABELS;
    out += next_line;

    return out;
}

string Board::get_board_large(Piece active_player) const {
    string out;

    string next_line = " " + _L_COL_LABELS;
    out += next_line + "\n";

    next_line = " " + _BG_COLOR + _BOARD_COLOR + _BOARD_TOP + RESET;
    out += next_line + "\n";

    for(int row = 0; row < 8; row++) {
        for(int line = 0; line < 2; line++) {
            string row_label = string(1, '1' + row);
            string left = " ";
            string right = " ";
            if(line == 0) {
                left = row_label;
            } else {
                right = row_label;
            }

            next_line = left + _BG_COLOR + _BOARD_COLOR + "║ ";

            for(int col = 0; col < 8; col++) {
                if(is_legal(active_player, Position(row, col))) {
                    if(active_player == Piece::P1) {
                        next_line += BLINK + _P1_COLOR + 
                            _L_POSSIBLE[line] + BLINK_OFF;
                    } else {
                        next_line += BLINK + _P2_COLOR + 
                            _L_POSSIBLE[line] + BLINK_OFF;
                    }
                } else {
                    switch(_board[row][col]) {
                        case Piece::EMPTY:
                            next_line += _EMPTY_COLOR + _L_EMPTY[line];
                            break;
                        case Piece::P1:
                            next_line += _P1_COLOR + _L_Piece[line];
                            break;
                        case Piece::P2:
                            next_line += _P2_COLOR + _L_Piece[line];
                            break;
                        default:
                            cmpt::error("Invalid board status value");
                    }
                }
            }

            next_line += _BOARD_COLOR + " ║" + RESET + right;

            out += next_line + "\n";
        }
    }

    next_line = " " + _BG_COLOR + _BOARD_COLOR + _BOARD_BOTTOM + RESET;
    out += next_line + "\n";

    next_line = " " + _L_COL_LABELS;
    out += next_line;

    return out;
}


//Public Methods

Board::Board(): Board(false, 0)
{}

Board::Board(bool large_board, int palette): _large_board{large_board} {
    set_palette(palette);

    _board = Board_vec(8, vector<Piece>(8, Piece::EMPTY));

    reset();
}


Board_vec Board::get_board_vec() const {
    return _board;
}

string Board::board_string() const {
    return board_string(Piece::EMPTY);
}

string Board::board_string(Piece piece) const {
    if(_large_board)
        return get_board_large(piece);
    else
        return get_board_small(piece);
}


void Board::set_size(bool set_large) {
    _large_board = set_large;
}

void Board::set_palette(int palette) {
    switch(palette) {
        case 0:
            _BG_COLOR = background({1,2,0});
            _BOARD_COLOR = foreground(6);
            _EMPTY_COLOR = foreground({1, 3, 0});
            _P1_COLOR = foreground(0);
            _P2_COLOR = foreground(23);
            break;

        case 1:
            _BG_COLOR = background(2);
            _BOARD_COLOR = foreground(23);
            _EMPTY_COLOR = foreground({1, 0, 0});
            _P1_COLOR = foreground({1,1,5});
            _P2_COLOR = foreground({1,5,1});
            break;

        case 2:
            _BG_COLOR = "";
            _BOARD_COLOR = foreground(23);
            _EMPTY_COLOR = foreground(6);
            _P1_COLOR = foreground({1, 1, 5});
            _P2_COLOR = foreground({1, 5, 1});
            break;

        default:
            cmpt::error("Color palette does not exist");
    }
}

bool Board::is_legal(Piece active_player, Position pos) const {
    if(active_player == Piece::EMPTY) return false;
    else return count_move(_board, active_player, pos) > 0;
}

int Board::count_legal_positions(Piece active_player) const {
    return count_legal_positions(_board, active_player);
}

bool Board::can_move(Piece piece) const {
    return can_move(_board, piece);
}

bool Board::game_over() const {
    return game_over(_board);
}

int Board::count_pieces(Piece piece) const {
    return count_pieces(_board, piece);
}


int Board::play(Piece piece, Position pos) {
    return play(_board, piece, pos);
}

void Board::reset() {
    for(auto &row: _board)
        for(auto &element: row)
            element = Piece::EMPTY;

    _board[3][4] = Piece::P1;
    _board[4][3] = Piece::P1;
    _board[3][3] = Piece::P2;
    _board[4][4] = Piece::P2;
}


//Static functions for use by computer player

vector<Position> Board::get_legal_positions(const Board_vec &board, Piece piece) {
    vector<Position> out;

    for(unsigned char row = 0; row < 8; row++) {
        for(unsigned char col = 0; col < 8; col++) {
            Position pos(row, col);
            if(count_move(board, piece, pos)) {
                out.push_back(pos);
            }
        }
    }

    return out;
}

int Board::count_legal_positions(const Board_vec &board, Piece piece) {
    unsigned char total = 0;

    for(unsigned char row = 0; row < 8; row++)
        for(unsigned char col = 0; col < 8; col++)
            if(count_move(board, piece, Position(row, col)))
                total++;

    return total;
}

bool Board::can_move(const Board_vec &board, Piece piece) {
    return count_legal_positions(board, piece) > 0;
}

int Board::count_move(const Board_vec &board, Piece piece, Position pos) {
    assert(piece != Piece::EMPTY);

    if(board[pos.row][pos.col] != Piece::EMPTY)
        return 0;

    int total = 0;
    for(int row_off = -1; row_off <= 1; row_off++) {
        for(int col_off = -1; col_off <= 1; col_off++) {
            if(!(row_off == 0 && col_off == 0)) {
                int line_flips = count_move_line(board, piece, pos, {row_off, col_off});
                if(line_flips > 0) {
                    total += line_flips;
                }
            }
        }
    }

    return total;
}

//This funciton is actually private, but is kept below count move as it is only every called
//through that function
int Board::count_move_line(const Board_vec &board, Piece piece, Position pos, const int (&offset)[2]) {
    Position next(pos, offset);

    if(next.row > 7 || next.col > 7)
        return -1;

    if(board[next.row][next.col] == Piece::EMPTY)
        return -1;

    if(board[next.row][next.col] == piece)
        return 0;

    int next_flips = count_move_line(board, piece, next, offset);
    if(next_flips == -1)
        return -1;

    return next_flips + 1;
}


int Board::play(Board_vec &board, Piece piece, Position pos) {
    assert(piece != Piece::EMPTY);

    if(board[pos.row][pos.col] != Piece::EMPTY)
        return 0;

    board[pos.row][pos.col] = piece;

    int total = 0;
    for(int row_off = -1; row_off <= 1; row_off++) {
        for(int col_off = -1; col_off <= 1; col_off++) {
            if(!(row_off == 0 && col_off == 0)) {
                int line_flips = flip_line(board, piece, pos, {row_off, col_off});
                if(line_flips > 0) {
                    total += line_flips;
                }
            }
        }
    }

    return total;
}

//This function is also actually private, but kept here because it is only ever called by play
int Board::flip_line(Board_vec &board, Piece piece, Position pos, const int (&offset)[2]) {
    Position next(pos, offset);

    if(next.row > 7 || next.col > 7)
        return -1;

    if(board[next.row][next.col] == Piece::EMPTY)
        return -1;

    if(board[next.row][next.col] == piece)
        return 0;

    int next_flips = flip_line(board, piece, next, offset);
    if(next_flips == -1)
        return -1;

    board[next.row][next.col] = piece;

    return next_flips + 1;
}

int Board::count_pieces(const Board_vec &board, Piece piece) {
    int count = 0;
    for(auto row: board)
        for(auto element: row)
            if(element == piece)
                count++;

    return count;
}

bool Board::game_over(const Board_vec &board) {
    return !can_move(board, Piece::P1) && !can_move(board, Piece::P2);
}

Piece Board::get_winner(const Board_vec &board) {
    int p1_pieces = count_pieces(board, Piece::P1);
    int p2_pieces = count_pieces(board, Piece::P2);
    if(p1_pieces > p2_pieces) {
        return Piece::P1;
    } else if (p1_pieces < p2_pieces) {
        return Piece::P2;
    } else {
        return Piece::EMPTY;
    }
}


#endif
