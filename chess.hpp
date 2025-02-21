#ifndef CHESS_HPP_INCLUDED
#define CHESS_HPP_INCLUDED

#include <iostream>
#include <string>
#include <array>
using namespace std;

enum type_piece
{
    K,
    Q,
    N,
    B,
    R,
    P
}; // p : pawn, pour le reste cf notation algébrique

extern char nom_piece[6];
extern int val_piece[6];
const pair<int, int> king_moves[8] = {{-1, -1}, {-1, 0}, {-1, +1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
const pair<int, int> bishop_moves[4] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
const pair<int, int> rook_moves[4] = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}};
const pair<int, int> knight_moves[8] = {{-2, -1}, {-2, +1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};

// source des tables : https://www.chessprogramming.org/Simplified_Evaluation_Function
const int pawn_table[8][8] =
    {{0, 0, 0, 0, 0, 0, 0, 0},
     {50, 50, 50, 50, 50, 50, 50, 50},
     {10, 10, 20, 30, 30, 20, 10, 10},
     {5, 5, 10, 25, 25, 10, 5, 5},
     {0, 0, 0, 20, 20, 0, 0, 0},
     {5, -5, -10, 0, 0, -10, -5, 5},
     {5, 10, 10, -20, -20, 10, 10, 5},
     {0, 0, 0, 0, 0, 0, 0, 0}};

const int knight_table[8][8] =
    {{-50, -40, -30, -30, -30, -30, -40, -50},
     {-40, -20, 0, 0, 0, 0, -20, -40},
     {-30, 0, 10, 15, 15, 10, 0, -30},
     {-30, 5, 15, 20, 20, 15, 5, -30},
     {-30, 0, 15, 20, 20, 15, 0, -30},
     {-30, 5, 10, 15, 15, 10, 5, -30},
     {-40, -20, 0, 5, 5, 0, -20, -40},
     {-50, -40, -30, -30, -30, -30, -40, -50}};

const int bishop_table[8][8] =
    {{-20, -10, -10, -10, -10, -10, -10, -20},
     {-10, 0, 0, 0, 0, 0, 0, -10},
     {-10, 0, 5, 10, 10, 5, 0, -10},
     {-10, 5, 5, 10, 10, 5, 5, -10},
     {-10, 0, 10, 10, 10, 10, 0, -10},
     {-10, 10, 10, 10, 10, 10, 10, -10},
     {-10, 5, 0, 0, 0, 0, 5, -10},
     {-20, -10, -10, -10, -10, -10, -10, -20}};

const int rook_table[8][8] =
    {{0, 0, 0, 0, 0, 0, 0, 0},
     {5, 10, 10, 10, 10, 10, 10, 5},
     {-5, 0, 0, 0, 0, 0, 0, -5},
     {-5, 0, 0, 0, 0, 0, 0, -5},
     {-5, 0, 0, 0, 0, 0, 0, -5},
     {-5, 0, 0, 0, 0, 0, 0, -5},
     {-5, 0, 0, 0, 0, 0, 0, -5},
     {0, 0, 0, 5, 5, 0, 0, 0}};

const int queen_table[8][8] =
    {{20, -10, -10, -5, -5, -10, -10, -20},
     {-10, 0, 0, 0, 0, 0, 0, -10},
     {-10, 0, 5, 5, 5, 5, 0, -10},
     {-5, 0, 5, 5, 5, 5, 0, -5},
     {0, 0, 5, 5, 5, 5, 0, -5},
     {-10, 5, 5, 5, 5, 5, 0, -10},
     {-10, 0, 5, 0, 0, 0, 0, -10},
     {-20, -10, -10, -5, -5, -10, -10, -20}};

const int king_middle_game_table[8][8] =
    {{-30, -40, -40, -50, -50, -40, -40, -30},
     {-30, -40, -40, -50, -50, -40, -40, -30},
     {-30, -40, -40, -50, -50, -40, -40, -30},
     {-30, -40, -40, -50, -50, -40, -40, -30},
     {-20, -30, -30, -40, -40, -30, -30, -20},
     {-10, -20, -20, -20, -20, -20, -20, -10},
     {20, 20, 0, 0, 0, 0, 20, 20},
     {20, 30, 10, 0, 0, 10, 30, 20}};

const int king_end_game_table[8][8] =
    {{-50, -40, -30, -20, -20, -30, -40, -50},
     {-30, -20, -10, 0, 0, -10, -20, -30},
     {-30, -10, 20, 30, 30, 20, -10, -30},
     {-30, -10, 30, 40, 40, 30, -10, -30},
     {-30, -10, 30, 40, 40, 30, -10, -30},
     {-30, -10, 20, 30, 30, 20, -10, -30},
     {-30, -30, 0, 0, 0, 0, -30, -30},
     {-50, -30, -30, -30, -30, -30, -30, -50}};

//===========================================================================
//                          class Square
//===========================================================================
class square
{
public:
    int i;
    int j;
    square(int i, int j);
    bool operator==(const square &s);
    bool operator!=(const square &s);
    square &operator+=(const pair<int, int> &d);
    square();
};
bool is_in(const square &s);

//===========================================================================
//                          class Piece
//===========================================================================

class piece
{
public:
    const pair<int, int> *moves;
    type_piece type;
    bool can_jump;
    bool isWhite;
    bool iter; // indique si peut itérer ses déplacements (fou, dame, tour)
    int nb_dir;
    int table[8][8];
    piece(const pair<int, int> m[], const type_piece t, const bool can_jump, const bool isWhite, const bool iter, const int n, const int table[8][8]);
    piece(const piece p, bool isWhite);
    piece();
    piece(const type_piece& t, bool isWhite);
};

extern piece White_King;
extern piece White_Queen;
extern piece White_Knight;
extern piece White_rook;
extern piece White_Bishop;
extern piece White_Pawn;
extern piece pieces[6];

//===========================================================================
//                          class Coup
//===========================================================================
class coup
{
public:
    piece p;
    square dep;
    square arr;
    bool is_capture;
    bool is_special;
    type_piece* promotion;
    coup(const piece p, const square dep, const square arr, const bool is_capture, bool is_special = false, type_piece* promotion = nullptr);
    coup();
};

//===========================================================================
//                          class Echequier
//===========================================================================
class Echiquier
{
public:
    piece *board[8][8];
    square *en_passant; // nullptr si en passant impossible, pointe sur la case où l'on peut prendre en passant sinon
    bool wcpr;          // white can petit roque
    bool wcgr;          // white can grand roque
    bool bcpr;          // black can petit roque
    bool bcgr;          // black can gran roque

    Echiquier();                              // génère la représentation de la position de départ
    Echiquier &operator=(const Echiquier &e); // opérateur par copie
    void make_move(const coup &c);
    void unmake_move(const coup& c, square* en_passant,type_piece* capture, bool wcpr, bool wcgr, bool bcpr, bool bcgr);
    bool is_att(const square& s, bool byWhite);
    bool ischeck(bool white);       //true si on regarde si le roi blanc est en échec
    int nb_cases_ctrl(square& s);
};

ostream &operator<<(ostream &os, const piece &p);
ostream &operator<<(ostream &os, const Echiquier &e);
bool is_legal(const coup &c, const Echiquier &e);
float eval(const Echiquier &e);

//===========================================================================
//                       fonctions codage standard
//===========================================================================
piece char2p(const char c); // lettre to pièce (renvoie la pièce noire)
int col2int(const char c);  // c est une lettre entre a et h représentant une colonne. Renvoie le j correspondant
coup alg2coup(char alg[], bool is_white, const Echiquier& e);
coup lit_alg(const char *c_alg, bool is_white);

#endif
