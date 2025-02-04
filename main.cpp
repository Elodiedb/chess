 #include <iostream>
 #include <string>
 //===========================================================================
using namespace std;
#include <array>

int nb_pos = 0;

enum type_piece {K, Q, N, B, R, P}; //p : pawn, pour le reste cf notation algébrique

char nom_piece[6] = {'K', 'Q', 'N', 'B', 'R', 'P'};
int val_piece[6] = {500, 9, 3, 3, 5, 1};
const pair<int, int> king_moves[8] = {{-1, -1},{-1, 0}, {-1, +1}, {0, -1}, {0,1}, {1, -1},{1,0}, {1,1}};
const pair<int, int> bishop_moves[4] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
const pair<int, int> rook_moves[4] = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}};
const pair<int, int> knight_moves[8] = {{-2, -1}, {-2, +1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};

//source des tables : https://www.chessprogramming.org/Simplified_Evaluation_Function
const int pawn_table[8][8] =
{ {0,  0,  0,  0,  0,  0,  0,  0},
{50, 50, 50, 50, 50, 50, 50, 50},
{10, 10, 20, 30, 30, 20, 10, 10},
 {5,  5, 10, 25, 25, 10,  5,  5},
 {0,  0,  0, 20, 20,  0,  0,  0},
 {5, -5,-10,  0,  0,-10, -5,  5},
 {5, 10, 10,-20,-20, 10, 10,  5},
{0,  0,  0,  0,  0,  0,  0,  0}};

const int knight_table[8][8] =
{{-50,-40,-30,-30,-30,-30,-40,-50},
{-40,-20,  0,  0,  0,  0,-20,-40},
{-30,  0, 10, 15, 15, 10,  0,-30},
{-30,  5, 15, 20, 20, 15,  5,-30},
{-30,  0, 15, 20, 20, 15,  0,-30},
{-30,  5, 10, 15, 15, 10,  5,-30},
{-40,-20,  0,  5,  5,  0,-20,-40},
{-50,-40,-30,-30,-30,-30,-40,-50}};

const int bishop_table[8][8] =
{{-20,-10,-10,-10,-10,-10,-10,-20},
{-10,  0,  0,  0,  0,  0,  0,-10},
{-10,  0,  5, 10, 10,  5,  0,-10},
{-10,  5,  5, 10, 10,  5,  5,-10},
{-10,  0, 10, 10, 10, 10,  0,-10},
{-10, 10, 10, 10, 10, 10, 10,-10},
{-10,  5,  0,  0,  0,  0,  5,-10},
{-20,-10,-10,-10,-10,-10,-10,-20}};

const int rook_table[8][8] =
{{0,  0,  0,  0,  0,  0,  0,  0},
  {5, 10, 10, 10, 10, 10, 10,  5},
 {-5,  0,  0,  0,  0,  0,  0, -5},
 {-5,  0,  0,  0,  0,  0,  0, -5},
 {-5,  0,  0,  0,  0,  0,  0, -5},
 {-5,  0,  0,  0,  0,  0,  0, -5},
 {-5,  0,  0,  0,  0,  0,  0, -5},
  {0,  0,  0,  5,  5,  0,  0,  0}};


const int queen_table[8][8] =
{{20,-10,-10, -5, -5,-10,-10,-20},
{-10,  0,  0,  0,  0,  0,  0,-10},
{-10,  0,  5,  5,  5,  5,  0,-10},
 {-5,  0,  5,  5,  5,  5,  0, -5},
  {0,  0,  5,  5,  5,  5,  0, -5},
{-10,  5,  5,  5,  5,  5,  0,-10},
{-10,  0,  5,  0,  0,  0,  0,-10},
{-20,-10,-10, -5, -5,-10,-10,-20}};

const int king_middle_game_table[8][8] =
{{-30,-40,-40,-50,-50,-40,-40,-30},
{-30,-40,-40,-50,-50,-40,-40,-30},
{-30,-40,-40,-50,-50,-40,-40,-30},
{-30,-40,-40,-50,-50,-40,-40,-30},
{-20,-30,-30,-40,-40,-30,-30,-20},
{-10,-20,-20,-20,-20,-20,-20,-10},
 {20, 20,  0,  0,  0,  0, 20, 20},
 {20, 30, 10,  0,  0, 10, 30, 20}};

const int king_end_game_table[8][8] =
{{-50,-40,-30,-20,-20,-30,-40,-50},
{-30,-20,-10,  0,  0,-10,-20,-30},
{-30,-10, 20, 30, 30, 20,-10,-30},
{-30,-10, 30, 40, 40, 30,-10,-30},
{-30,-10, 30, 40, 40, 30,-10,-30},
{-30,-10, 20, 30, 30, 20,-10,-30},
{-30,-30,  0,  0,  0,  0,-30,-30},
{-50,-30,-30,-30,-30,-30,-30,-50}};

class square
{
    public:
    int i;
    int j;
    square (int i, int j);
    bool operator == (const square & s);
    bool operator != (const square& s);
    square& operator += (const pair<int, int>& d);
    square ();
};

square :: square()
{
    this->i = 0;
    this->j = 0;
}
square& square :: operator += (const pair<int, int>& d)
{
    this->i += d.first;
    this->j += d.second;
    return (*this);
}

bool is_in(const square & s)
{
    return (s.i < 8 && s.i >= 0 && s.j < 8 && s.j >= 0);
}




//implémentation des pieces


class piece
{
public:
    const pair<int, int>* moves;
    type_piece type;
    bool can_jump;
    bool isWhite;
    bool iter;       //indique si peut itérer ses déplacements (fou, dame, tour)
    int nb_dir;
    int table[8][8];
    piece(const pair<int, int> m[], const type_piece t, const bool can_jump,const bool isWhite, const bool iter, const int n, const int table[8][8]);
    piece(const piece p, bool isWhite);
    piece();
};


piece :: piece(const pair<int, int> m[], const type_piece t, const bool can_jump,const  bool isWhite,const bool iter, const int n, const int table[8][8])
{
    this->moves = m;
    this->type = t;
    this->can_jump = can_jump;
    this->isWhite = isWhite;
    this->iter = iter;
    this->nb_dir = n;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j ++)
        {
            this->table[i][j] = table[i][j];
        }
    }
}

piece :: piece(const piece p, bool isWhite)
{
    this->moves = p.moves;
    this->type = p.type;
    this->can_jump = p.can_jump;
    this->isWhite = isWhite;
    this->iter = p.iter;
    this->nb_dir = p.nb_dir;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j ++)
        {
            this->table[i][j] = p.table[i][j];
        }
    }
}

piece White_King = piece(king_moves, K, false, true, false, 8, king_middle_game_table);
piece White_Queen = piece(king_moves, Q, false, true, true, 8, queen_table);
piece White_Knight = piece(knight_moves, N, true, true, false, 8, knight_table);
piece White_rook = piece(rook_moves, R, false, true, true, 4, rook_table);
piece White_Bishop = piece(bishop_moves, B, false, true, true, 4, bishop_table);
piece White_Pawn = piece(king_moves, P, false, true, false, 8, pawn_table);         //A MODIFIER?
piece pieces[6]= {White_King, White_Queen, White_Knight, White_Bishop, White_rook, White_Pawn};

piece :: piece()
{
    *this = piece(White_King, true);
}

class coup{
    public:
piece p;
square dep;
square arr;
bool is_capture;
coup (const piece p, const square dep, const square arr, const bool is_capture);
coup();
};

coup:: coup()
{
    this->p = piece();
    this->dep = square();
    this->arr = square();
    this->is_capture = false;
}
coup :: coup (const piece p, const square dep, const square arr,const bool is_capture)
{
    this->p = p;
    this-> dep = dep;
    this-> arr = arr;
    this-> is_capture = is_capture;
}

class Echiquier
{
    public :
    piece* board[8][8];
    square* en_passant; //nullptr si en passant impossible, pointe sur la case où l'on peut prendre en passant sinon
    bool wcpr;      //white can petit roque
    bool wcgr;      //white can grand roque
    bool bcpr;      //black can petit roque
    bool bcgr;      //black can gran roque

    Echiquier();        //génère la représentation de la position de départ
    Echiquier& operator =(const Echiquier& e);      //opérateur par copie
    void make_move (const coup& c);
};

Echiquier& Echiquier :: operator= (const Echiquier& e)      //constructeur par copie
{
    //désallocation des pièces précédentes
     for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            delete this->board[i][j]; // Supprime l'ancienne pièce (évite les fuites)
            this->board[i][j] = nullptr;
        }
    }

    // Copier les pièces de `e`
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (e.board[i][j] != nullptr)
            {
                this->board[i][j] = new piece(*(e.board[i][j])); // Clonage dynamique
            }
            else
            {
                this->board[i][j] = nullptr;
            }
        }
    }

    // 3. Copier le pointeur en_passant (attention, ne pas copier l'objet si c'est alloué dynamiquement)
    this->en_passant = e.en_passant;
    this->wcpr = e.wcpr;
    this->wcgr = e.wcgr;
    this->bcgr = e.bcgr;
    this->bcpr = e.bcpr;
    return *this;
}



ostream& operator<<(ostream& os,const piece& p){
    if (p.isWhite)
    {
        //La pièce est noire
        os << "W";
    }
    else
    {
        //La pièce est blanche
        os << "B";
    }
    os << nom_piece[p.type];
    return os;
}

ostream& operator<<(ostream& os,const Echiquier& e){
for (int j = 0; j < 8; j++)
{
    //os << "___";
}
os << "\n";
for(int i=0; i < 8; i++)

{   os << "|";
    for(int j = 0; j < 8; j++)
    {
        piece* case_act = e.board[7 - i][j];
        if (case_act != nullptr)
            {
            os << *(case_act);
            os << "|";
            }
        else
        {
            os << "  |";
        }
    }
    os << "\n";
    for (int j = 0; j < 8; j++)
    {
        //os << "___";
    }
    //os << "\n";
}
return os;
}

void Echiquier :: make_move(const coup& c)
{
    (this->board)[c.arr.i][c.arr.j] = (this->board)[c.dep.i][c.dep.j];
    (this->board)[c.dep.i][c.dep.j] = nullptr;
    //cout << (*this);

    //MODIFIER LE CARACTERE EN PASSANT
}

bool is_legal(const coup& c, const Echiquier& e)
{
    piece p = c.p;
    piece* ptr_p_dep = e.board[c.dep.i][c.dep.j];
    bool res = false;
    if (ptr_p_dep !=nullptr)
    {
        piece p_dep = *ptr_p_dep;
        if (p.type != P && p.type == p_dep.type)    //ce n'est pas un pion
        {
            int n = p.nb_dir;
            for(int l = 0; l < n; l++)
            {
                bool peut_continuer = true;
                square s = c.dep;
                for(s += p.moves[l];is_in(s) && peut_continuer; s+=p.moves[l])
                {
                    res = res || (s == c.arr);
                    piece* p_s = e.board[s.i][s.j];
                    peut_continuer = p.iter;     // si on ne peut pas aller plusieurs fois dans la même direction, on ne peut jamais continuer
                    if (p_s != nullptr)
                    {
                        //on arrive sur une pièce
                        peut_continuer = false;
                        res = res && (p_s->isWhite != p.isWhite);   //si on pensait que le coup était légal mais qu'une pièce allié est sur la case d'arrivée
                    }
                }
            }
        }
        else if (p.type == P && p_dep.type == p.type)
        {
            int di = 1;
            if (!p.isWhite)
            {
                di = -1;
            }
            int i_dep = 1;
            if (!p.isWhite)
            {
                i_dep = 6;
            }
            if (!c.is_capture)
            {
                res = (e.board[c.arr.i][c.arr.j] == nullptr);
                if ((c.arr.i - c.dep.i == di && c.arr.j == c.dep.j))
                {
                    return(c.dep.i >=1 && c.dep.i <= 6 && e.board[c.arr.i][c.arr.j] == nullptr);
                }
                if (((c.arr.i - c.dep.i ==  2 * di) && (c.arr.j == c.dep.j)))
                {
                    return(c.dep.i >=1 && c.dep.i <= 6 && e.board[c.arr.i][c.arr.j] == nullptr && e.board[c.dep.i + di][c.arr.j] == nullptr);
                }
            }
        }
    }
    return res;
}





 //Implémentation

 Echiquier :: Echiquier()
 {

    //Pions et cases vide

    for (int j = 0; j < 8; j++)
    {
        (*this).board[1][j] = new piece(White_Pawn, true);
        (*this).board[6][j] = new piece(White_Pawn, false);
        for (int i = 2; i < 6; i++)
        {
            (*this).board[i][j] = nullptr;
        }
    }

    //Pièces blanches

    (*this).board[0][0] = new piece(White_rook, true);
    (*this).board[0][1] = new piece(White_Knight, true);
    (*this).board[0][2] = new piece(White_Bishop, true);
    (*this).board[0][3] = new piece(White_Queen, true);
    (*this).board[0][4] = new piece(White_King, true);
    (*this).board[0][5] = new piece(White_Bishop, true);
    (*this).board[0][6] = new piece(White_Knight, true);
    (*this).board[0][7] = new piece(White_rook, true);

    //Pièces noires

    (*this).board[7][0] = new piece(White_rook, false);
    (*this).board[7][1] = new piece(White_Knight, false);
    (*this).board[7][2] = new piece(White_Bishop, false);
    (*this).board[7][3] = new piece(White_Queen, false);
    (*this).board[7][4] = new piece(White_King, false);
    (*this).board[7][5] = new piece(White_Bishop, false);
    (*this).board[7][6] = new piece(White_Knight, false);
    (*this).board[7][7] = new piece(White_rook, false);

    (*this).en_passant = nullptr;
    this->wcpr = true;
    this->wcgr = true;
    this->bcgr = true;
    this->bcpr = true;
}

float eval(const Echiquier& e)
{
    float res = 0;
    for(int i = 0; i < 8; i++)
    {

        for (int j = 0; j < 8; j++)
        {
            //cout << "i : " << i << "j : " << j << "\n";
            piece* case_actuelle = e.board[i][j];
            if (case_actuelle != nullptr)
            {
                piece p = *(case_actuelle);
                if (p.isWhite)
                {
                    res += val_piece[p.type] * 100;
                    res += p.table[i][j];
                }
                else
                {
                    //la pièce est noire
                    res -= val_piece[p.type] * 100;
                    res -= p.table[7-i][7-j];
                }
            }
        }
    }
    //cout << "a survécu";
    return (res / 100);
}

square :: square(int i, int j)
{
    (*this).i = i;
    (*this).j = j;
}

bool square :: operator != (const square& s)
{
    return ((*this).i != s.i || (*this).j != s.j);
}

bool square :: operator == (const square& s)
{
    return (!((*this) != s));
}



/*list<coup> coups_legaux(const Echiquier& e, bool is_white)
{
    auto board = e.board;
    list<coup> liste_coups;
    coup cp;
    for (int i =0; i < 8; i++)
    {
        for (int (j = 0; j < 8; j++))
        {
            square s = e[i][j];
            if (s!= nullptr)
                {
                  piece p = *s;
                  if (is_white && p >= 6)
                  {
                      //on a trouvé une pièce blanche et c'est aux Blancs de jouer
                      if (p % 6 == 0)
                      {
                          //on bouge le roi

                          //ATTENTION AUCUNE GESTION DES ECHECS
                          for (int di = -1; di < 2; di++)
                          {
                              for (int dj = -1; dj < 2; dj ++)
                              {
                                  square arr = square (s.i + di, s.j + dj);
                                  if (is_in(arr) && arr != s){
                                      //on est dedans et ce n'est pas le mouvement nul
                                      if (board[arr.i][arr.j] == nullptr)
                                      {
                                          cp.dep =s;
                                          cp.arr = arr;
                                          cp.is_capture = false;
                                          cp.p = p;
                                          liste_coups.push_back(cp);
                                      }
                                      else
                                      {
                                          if (*board[arr.i, arr.j +dj ] < 6)
                                          {
                                              //il y a une pièce noire sur la case d'arrivée
                                              cp.dep =s;
                                              cp.arr = arr;
                                              cp.is_capture = true;
                                              cp.p = p;
                                              liste_coups.push_back(cp);
                                            }
                                      }
                                  }
                              }
                          }
                      }
                      if (p % 6 == 1)
                      {
                          // on bouge la dame
                          for (int di = -1; di < 2; di++)
                          {
                              for (int dj = -1; dj < 2; dj ++)
                              {
                                  //on a choisit une des 8 directions
                                  if (di!= 0 || dj != 0)
                                  {
                                      bool peut_continuer = true;
                                      for(square arr = s; is_in(arr) && peut_continuer; arr.i +=di)
                                      {
                                           if (board[arr.i][arr.j] == nullptr)
                                          {
                                              cp.dep =s;
                                              cp.arr = arr;
                                              cp.is_capture = false;
                                              cp.p = p;
                                              liste_coups.push_back(cp);
                                          }
                                          else
                                          {
                                              if (*board[arr.i, arr.j +dj ] < 6)
                                              {
                                                  //il y a une pièce noire sur la case d'arrivée
                                                  cp.dep =s;
                                                  cp.arr = arr;
                                                  cp.is_capture = true;
                                                  cp.p = p;
                                                  liste_coups.push_back(cp);
                                                  peut_continuer == false;
                                              }
                                          }

                                          arr.j +=dj;
                                      }
                                  }

                              }
                          }
                      }
                  }
                }
        }
    }
}
*/

/*bool is_legal (const square& dep, const square& arr, const piece& p, const Echiquier e)
{
    bool is_white = ~(p< 6);
    if ( p % 6 == 5)
    {
      //on bouge un pion
        if (is_white)
        {
            if (arr.i != dep.i + 1)
            {
                //le pion n'a pas avancé d'une seule case
                if(dep.i == )
            }
            if (e.board[arr.i][arr.j] ! nullptr)
        }
    }
}
*/

piece char2p(const char c)        //lettre to pièce (renvoie la pièce noire)
{
    if (c == 'K')
    {
        return White_King;
    }
     if (c == 'Q')
    {
        return White_Queen;
    }
     if (c == 'N')
    {
        return White_Knight;
    }
     if (c == 'B')
    {
        return White_Bishop;
    }
    if (c== 'R')
    {
        return White_rook;
    }
    else
    {
        return White_Pawn;
    }
}


int col2int (const char c)      //c est une lettre entre a et h représentant une colonne. Renvoie le j correspondant
{
   if (c == 'a')
    {
        return 0;
    }
     if (c == 'b')
    {
        return 1;
    }
     if (c == 'c')
    {
        return 2;
    }
     if (c == 'd')
    {
        return 3;
    }
     if (c == 'e')
    {
        return 4;
    }
    if (c == 'f')
    {
        return 5;
    }
    if (c == 'g')
    {
        return 6;
    }
    if (c == 'h')
    {
        return 7;
    }
    else
    {
        return -1;
    }
}

coup alg2coup(char alg[], bool is_white)
{
    //cout << "est rentre\n";
    piece p = piece(char2p(alg[0]), is_white);
    //cout << "p : " << p << "\n";
    int j_dep = col2int(alg[1]);
    //cout << "j_dep : "<< j_dep << "\n";
    int i_dep = alg[2] - '0' -  1;
    bool is_capture = (alg[3] == 'x');
    //cout << "dep = (" << i_dep << "," << j_dep << ")\n";
    int j_arr = col2int(alg[4]);
    int i_arr = alg[5] - '0' - 1;
    //cout << "dep = (" << i_dep << "," << j_dep << ")\n";
    //cout << "arr = (" << i_arr << "," << j_arr << ")\n";
    return coup(p, square(i_dep, j_dep), square(i_arr, j_arr), is_capture);
}

square trouve_depart(const square& arr, const enum type_piece p, bool isWhite, bool is_capture, const Echiquier& e)
{
    //renvoie une case où se trouve une pièce de type p de "couleur" isWhite pouvant aller en arr (peu opti)
    auto board = e.board;
    int j = 0;
    square trouve[8];
    int l = 0;
    for (int i = 0; i < 8; i ++ )
    {
        for (j = 0; j < 8; j ++)
        {
            if (board[i][j] != nullptr)
            {
                piece p_act = *(board[i][j]);
                if (((p_act.type == p) && (p_act.isWhite) == isWhite))
                {
                    trouve[l].i = i;
                    trouve[l].j = j;
                    l++;
                }
            }
        }
    }
    int k = 0;
    for(; k < l; k++)
    {
        if (is_legal(coup(piece(pieces[p], isWhite),trouve[k], arr, is_capture),e))
        {
            return(trouve[k]);
        }
    }
    return trouve[k];               //ATTENTION, RENVOIE QUELQUE CHOSE MEME SI COUP ABSURDE!!!
}

coup lit_alg(const char* c_alg, bool is_white)
{
    //on suppose pour l'instant que le coup est légal
    coup cp = coup();    //coup renvoyé
    int l = 0;      //indice dans la chaîne lue
    piece p = piece();
    //p.type = static_cast<type_piece> (l2p(c_alg[l]));  //pièce déplacée
    if (!is_white)
    {
        p.isWhite += false;
    }
    cp.p = p;
    l++;
    cp.is_capture = (c_alg[l] == 'x');
    l++;
    cp.dep.j =  col2int(c_alg[l]);
    l++;
    cp.dep.i = c_alg[l] - '0';
    return cp;
}


//Gestion de l'algo min-max



 class Position{
 public :
 Echiquier e;
 bool white_turn;      //True ssi aux Blancs de jouer
 Position* fille;       //première position fille regardée
 Position* soeur;   //prochaine position regardée
 coup c; //coup permettant de passer de la mère à elle

 float eval_pos() const;
 Position(const Echiquier& e, const bool& white_turn, Position* fille, Position* soeur, const coup& c);   //fille et soeur seront modifiables!!!
 void prof_suivante(int prof);
 ~Position();        //efface la position ainsi que les filles
 };

 Position :: Position(const Echiquier& e, const bool& white_turn,Position* fille, Position* soeur, const coup& c)
 {
     //nb_pos ++;
     //cout << "nb_pos : "<< nb_pos << "\n";
     (*this).e = e;
     this-> white_turn = white_turn;
     this-> fille = fille;
     this->soeur = soeur;
     this->c = c;
 }

 float Position :: eval_pos() const
 {
     return eval(this->e);
 }

 class meilleur_coup{
 public :
 float evaluation;
 coup c;

 meilleur_coup(const float& eval,const coup& c);
 };

 meilleur_coup :: meilleur_coup(const float& eval,const coup& c)
 {
     this->evaluation = eval;
     this->c = c;
 }

 Position :: ~Position()
 {
    //cout << "debut delete \n";
    if (this->soeur!=nullptr)
    {
        //cout << "delete soeur \n";
        delete this->soeur;
        this->soeur = nullptr;
        //cout << "soeur deleted \n";
    }
    if (this->fille!=nullptr)
    {
        //cout << "delete fille \n";
        delete this->fille;
        this->fille = nullptr;
    }

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            delete this->e.board[i][j];  // Supprime chaque pièce allouée dynamiquement
            this->e.board[i][j] = nullptr;
        }
    }
     nb_pos --;
     //cout << "nb_pos : "<< nb_pos << "\n";

 }

void Position :: prof_suivante(int prof)
{
    //cout << "Appel a prof " << prof << "\n";
    //int capture_trouvees = 0;
    Echiquier e  = this->e;
    //cout << e << "\n";
    bool should_look = true;
    Position * fille_prec = nullptr;
    bool must_capture = false;
    if (prof <= 0)
    {
        must_capture = true;
    }
    if (prof < -4)     //résout le problème sans trop que je sache pourquoi
    {
        should_look = false;
    }
    if (should_look)
    {
        for(int i = 0; i< 8; i++)
        {
            for(int j = 0; j < 8; j++)
            {
                //cout << "i :" << i << "; j : " << j << "\n";
                piece* ptr_piece = e.board[i][j];
                if (ptr_piece != nullptr)
                {
                    piece p = *ptr_piece;
                    // cout << this->white_turn;
                    if (p.type != P && (p.isWhite == this->white_turn))    //ce n'est pas un pion
                    {
                        int n = p.nb_dir;
                        for(int l = 0; l < n; l++)
                        {
                            bool peut_continuer = true;
                            square s = square(i,j);
                            for(s += p.moves[l];is_in(s) && peut_continuer; s+=p.moves[l])
                            {
                                piece* ptr_p_s = e.board[s.i][s.j];     //eventuelle piece sur la case arr
                                peut_continuer = p.iter;     // si on ne peut pas aller plusieurs fois dans la même direction, on ne peut jamais continuer
                                if (e.board[s.i][s.j] != nullptr)
                                {
                                    //on arrive sur une pièce
                                    piece p_s = *(e.board[s.i][s.j]);               //IL SEMBLERAIT QUE CA SOIT DIFFERRENT DE "piece p_s = *(ptr_p_s)" !!!!
                                    //cout << "Echiquier depart : \n" << e;
                                    //cout << "i : " << i << " j: " << j << "\n";
                                    //cout << "piece bougee : " << p << "\n";
                                    //cout << "on arrive sur une piece :   " << p_s << *(e.board[s.i][s.j]) << "\n";
                                    //cout << "case d arrivee : (" << s.i << "," << s.j<<")\n";
                                    //cout << (p_s.isWhite ? "true" : "false" )<<  (this->white_turn ? "true" : "false") << "\n";
                                    peut_continuer = false;
                                    if (p_s.isWhite != this->white_turn)
                                    {
                                        //on a trouvé une capture
                                        //cout << "on a trouve une capture !! \n";
                                        Echiquier nouv_e = e;
                                        //cout << "nouv_e avant capture\n" << nouv_e << "\n";
                                        coup c = coup(p, square(i,j),s, true);
                                        nouv_e.make_move(c);
                                        //cout << "nouv_e apres capture\n" << nouv_e << "\n";
                                        //capture_trouvees++;
                                        //cout << "capture_trouvees :"   << capture_trouvees << "\n";
                                        Position* fille =new Position(nouv_e, !white_turn, nullptr, fille_prec, c);
                                        //fille->prof_suivante(prof - 1);
                                        fille_prec = fille;
                                        this->fille = fille;
                                    }
                                }
                                else
                                {
                                    //on a trouvé un coup qui atterrit sur une case vide
                                    if (!must_capture)
                                    {
                                        //cout << "!must capture piece\n";
                                        Echiquier nouv_e = e;
                                        //cout << "nouv_e avant coup \n" << nouv_e << "\n";
                                        nouv_e.make_move(coup(p, square(i,j),s, false));
                                        //cout << "nouv_e apres coup \n" << nouv_e << "\n";
                                        //cout << this->white_turn << "\n";
                                        //cout << nouv_e;
                                        //cout << "\n";
                                        Position* fille = new Position(nouv_e, !white_turn,nullptr, fille_prec, coup(p, square(i,j),s, false));
                                        //fille->prof_suivante(prof-1);
                                        fille_prec = fille;
                                        this->fille = fille;
                                    }
                                }
                            }
                        }
                    }

                    else if ((p.type == P) && (p.isWhite == this->white_turn) )
                    {
                        //on bouge un pion
                        //cout << "on bouge un pion \n";
                        square dep = square(i,j);
                        int di = 1;
                        int i_dep =1;
                        if (!white_turn)
                        {
                            di = -1;
                            i_dep = 6;
                        }
                        if (!must_capture)
                        {
                            //cout << "white_turn\n";
                            if (is_in(square(i+di,j)))
                            {
                                //cout << "case devant est dans l'echiquier \n";
                                if(e.board[i + di][j] == nullptr)
                                {
                                    //la case devant le pion est libre
                                    //cout << "pion blanc avance d'une case \n";
                                    Echiquier nouv_e = e;
                                    //cout << "nouv_e avant coup \n" << nouv_e << "\n";
                                    nouv_e.make_move(coup(p, dep, square(i+di, j), false));
                                    //cout << "nouv_e apres coup \n" << nouv_e << "\n";
                                    Position * fille = new Position(nouv_e, !white_turn, nullptr, fille_prec, coup(p, dep, square(i+di, j), false));
                                    //fille->prof_suivante(prof-1);
                                    fille_prec = fille;
                                    this->fille = fille;
                                    //cout << "on avance le pion " << j << "\n";
                                    //cout << this->white_turn << "\n";
                                    //cout << nouv_e;
                                    //cout << "\n";
                                    if (is_in(square(i+2 * di , j)))
                                    {
                                        if((e.board[i+2 * di ][j] == nullptr) && (i == i_dep))
                                        {
                                            //cout << "pion blanc avance de deux cases \n";
                                            //le pion peut avancer de deux cases
                                            Echiquier nouv_nouv_e = e;
                                            //cout << "nouv_nouv_e avant coup \n" << nouv_nouv_e << "\n";
                                            nouv_nouv_e.make_move(coup(p, dep, square(i+2 * di, j), false));
                                            //cout << "nouv_nouv_e apres coup \n" << nouv_nouv_e << "\n";
                                            //cout << "avant l'appel de new position\n";
                                            Position * fille2 = new Position(nouv_nouv_e, !white_turn,nullptr, fille_prec, coup(p, dep, square(i+2 * di, j), false));
                                            //cout << "avant l'appel de prof_suivante \n";
                                            //fille2->prof_suivante(prof-1);
                                            //cout << "la fille a été gérée \n";
                                            fille_prec = fille2;
                                            this->fille = fille2;
                                        }
                                    }
                                    //cout << "on a géré les avancées de un et deux\n";
                                }
                            }
                        }
                        if (is_in(square(i+di, j+1)))
                        {
                            //cout << "is_in diagonale droite\n";
                            if(e.board[i+di][j+1] != nullptr)
                            {
                                //la case en diagonale du pion est occupée
                                piece p2 = *(e.board[i+di][j+1]);
                                if(p2.isWhite != white_turn)
                                {
                                    //une capture est possible
                                    Echiquier nouv_e = e;
                                    nouv_e.make_move(coup(p, dep, square(i+di, j+1), true));
                                    //capture_trouvees++;
                                    //cout << "capture_trouvees :"   << capture_trouvees << "\n";
                                    Position * fille = new Position(nouv_e, !white_turn,nullptr, fille_prec, coup(p, dep, square(i+di, j+1), true));
                                    //fille->prof_suivante(prof-1);
                                    fille_prec = fille;
                                    this->fille = fille;
                                }
                            }
                        }
                        if (is_in(square(i+di, j-1)))
                        {
                            //AJOUTER EN PASSANT ICI
                            //cout << "is_in diagonale gauche \n";
                            if(e.board[i+di][j-1] != nullptr)
                            {
                                //cout << "diagonale gauche occupee \n";
                                //la case en diagonale du pion est occupée
                                piece p2 = *(e.board[i+di][j-1]);
                                if(p2.isWhite != white_turn)
                                {
                                    //une capture est possible
                                    //cout << "prise en diagonale \n";
                                    Echiquier nouv_e = e;
                                    nouv_e.make_move(coup(p, dep, square(i+di, j-1), true));
                                    //capture_trouvees++;
                                    //cout << "capture_trouvees :"   << capture_trouvees << "\n";
                                    Position * fille = new Position(nouv_e, !white_turn,nullptr, fille_prec, coup(p, dep, square(i+di, j-1), true));
                                    //fille->prof_suivante(prof-1);
                                    fille_prec = fille;
                                    this->fille = fille;
                                }
                            }
                            //cout << "fin traitement diagonale gauche\n";
                        }
                        if (e.en_passant != nullptr)
                        {
                            /*if (square(i+di, j-1) == *e.en_passant || square(i+di, j+1) == *e.en_passant)
                            {
                                //AJOUTER LES PRISES EN PASSANT!!!!!!!!!!!
                            }
                            */

                        }

                    }
                }
            }
                //cout << "en bas du for\n";
        }
        //cout << capture_trouvees << "\n";
    }
}





meilleur_coup alpha_beta(Position& pos, int prof, float alpha, float beta, coup c)
{
    //c : meilleur coup trouvé jusqu'à maintenant
    //cout << "Un appel \n";
    pos.prof_suivante(prof);
    //cout << "prof_suivante_generee\n";
    float v = pos.eval_pos();
    if (pos.fille == nullptr || abs(pos.eval_pos()) > 6)
    {
        //cout<< "prof nulle \n" ;
        return (meilleur_coup(pos.eval_pos(), c));
    }
    //cout << pos.fille->e;
    else
    {
        //pos.prof_suivante(prof);
        if (pos.fille == nullptr)
        {
            cout << "Je suis la reine des salopes ! ";
        }
        coup coup_opt = c;
        //cout << e;
        if(pos.white_turn)
        {
            v = -1000;
            int i = 1;
            for(auto fille = pos.fille; fille !=nullptr; fille = fille->soeur)
            {
                i++;
                //cout << "mere white turn : " << (pos.white_turn ? "true" : "false") << "fille white turn : " << (fille->white_turn ? "true" : "false") << "\n";
                //cout << "position fille alpha-beta  : \n" << fille->e << "\n";
                meilleur_coup res = alpha_beta(*fille, prof-1, alpha, beta, c);
                //cout << "res.ealuation : " << res.evaluation << endl;
                if (res.evaluation > v)
                {
                    //cout<< "Mieux que l'opt blanc " << "white_turn " << (pos.white_turn ? "true" : "false") << " v : " << v <<  " nouv_v : " << res.evaluation  << "\n";
                    coup_opt = fille->c;
                    v = res.evaluation;
                    //cout << "v : " << v<< "\n";
                }
                if (beta <= v)
                {
                    //cout << "coupure alpha\n";
                    Position * temp = pos.fille;
                    pos.fille = nullptr;
                    delete temp;
                    return meilleur_coup(v, coup_opt);
                }
                //Position* temp = fille;
                //fille = fille->soeur;
                //delete temp;  // Libère l'ancienne position
                alpha = max(alpha, v);
            }
            Position * temp = pos.fille;
            pos.fille = nullptr;
            delete temp;
        }
        else
        {
            v = +1000;
            for(auto fille = pos.fille; fille !=nullptr; fille = fille->soeur)
            {
                //cout << "position fille black turn \n" << fille->e << "\n";
                //cout << "mere white turn : " << (pos.white_turn ? "true" : "false") << "fille white turn : " << (fille->white_turn ? "true" : "false") << "\n";
                 meilleur_coup res = alpha_beta(*fille, prof-1, alpha, beta, fille->c);
                if (res.evaluation < v)
                {
                    //cout << prof << endl;
                    //cout << "position mere : \n" << pos.e << "\n";
                    //cout << "position fille black turn \n" << fille->e << "\n";
                    //cout<< "Mieux que l'opt " << "white_turn " << (pos.white_turn ? "true" : "false") << " v : " << v <<  " nouv_v : " << res.evaluation  << "\n";
                    coup_opt = fille->c;
                    v = res.evaluation;
                }
                if (alpha >= v)
                {
                    //cout << "alpha : " << alpha << " v: " << v << " coupure alpha \n";
                    Position * temp = pos.fille;
                    pos.fille = nullptr;
                    delete temp;
                    return meilleur_coup(v, coup_opt);
                }
                beta = min(beta, v);
            }
            Position * temp = pos.fille;
            pos.fille = nullptr;
            delete temp;
        }
        //cout << "arrive au return \n";
        //cout << "v au return : " << v << "\n";
        return (meilleur_coup(v, coup_opt));
    }
}

int main(){
Echiquier e = Echiquier();
int prof = 4 ;
char* coup_alg;
cout << e;
cout << "eval : " << eval(e) << "\n";
//cout << "Entrez un coup";
Position pos= Position(e, true, nullptr, nullptr, coup());
int i = 1;
while (i < 100)
{
    if(true)
    {
        cout << "tour : " << i << "\n";
        //pos.prof_suivante(prof);
        //cout << "prof_suivante generee \n";
        coup c = coup();
        //cout << "coup par defaut genere";
        meilleur_coup mc = alpha_beta(pos, prof, -1000, +1000, coup());
        cout << "eval estimee :" << mc.evaluation << "\n";
        pos.e.make_move(mc.c);
        //cout << "pos.white_turn : " << (pos.white_turn ? "true" : "false") << "\n";
        cout << pos.e;
        cout << "eval : " << eval(pos.e) << "\n";
        //pos = Position(pos.e, ! pos.white_turn, nullptr, nullptr, coup());      //A remplacer par nullptr/ destroy ?
        pos.white_turn = !pos.white_turn;
        //cout << "opposition du tour accomplie";
        //delete &pos;
        //cout << "delete accompli\n";
        //Position pos= Position(e, true, nullptr, nullptr, coup());
        //cout << "fin de boucle\n";
        //pos.white_turn = !pos.white_turn;
        //pos.destroy();
        i++;
    }
    else
    {
        //tour des noirs aka lae joueur.euse
        //cout << "e avant c_in\n" << pos.e;
        char alg[10];
        //cout << "e juste avant c_in\n" << pos.e;
        cin >> alg;
        //cout << "e_juste après le c_in\n" << pos.e;
        coup c = alg2coup(alg, pos.white_turn);
        //cout << "lecture terminee\n";
        //cout << "e avant coup\n" << pos.e;
        pos.e.make_move(c);
        pos.white_turn = !pos.white_turn;
        cout << pos.e;
    }
}
}
