#include "chess.hpp"

char nom_piece[6] = {'K', 'Q', 'N', 'B', 'R', 'P'};
int val_piece[6] = {500, 9, 3, 3, 5, 1};

//===========================================================================
//                          class Square
//===========================================================================
square ::square()
{
    this->i = 0;
    this->j = 0;
};

square &square ::operator+=(const pair<int, int> &d)
{
    this->i += d.first;
    this->j += d.second;
    return (*this);
};

square ::square(int i, int j)
{
    (*this).i = i;
    (*this).j = j;
};

bool square ::operator!=(const square &s)
{
    return ((*this).i != s.i || (*this).j != s.j);
};

bool square ::operator==(const square &s)
{
    return (!((*this) != s));
};

bool is_in(const square &s)
{
    return (s.i < 8 && s.i >= 0 && s.j < 8 && s.j >= 0);
};

//===========================================================================
//                          class Piece
//===========================================================================
piece ::piece(const pair<int, int> m[], const type_piece t, const bool can_jump, const bool isWhite, const bool iter, const int n, const int table[8][8])
{
    this->moves = m;
    this->type = t;
    this->can_jump = can_jump;
    this->isWhite = isWhite;
    this->iter = iter;
    this->nb_dir = n;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            this->table[i][j] = table[i][j];
        }
    }
};

piece ::piece(const piece p, bool isWhite)
{
    this->moves = p.moves;
    this->type = p.type;
    this->can_jump = p.can_jump;
    this->isWhite = isWhite;
    this->iter = p.iter;
    this->nb_dir = p.nb_dir;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            this->table[i][j] = p.table[i][j];
        }
    }
};

piece ::piece()
{
    *this = piece(White_King, true);
};


piece White_King = piece(king_moves, K, false, true, false, 8, king_middle_game_table);
piece White_Queen = piece(king_moves, Q, false, true, true, 8, queen_table);
piece White_Knight = piece(knight_moves, N, true, true, false, 8, knight_table);
piece White_rook = piece(rook_moves, R, false, true, true, 4, rook_table);
piece White_Bishop = piece(bishop_moves, B, false, true, true, 4, bishop_table);
piece White_Pawn = piece(king_moves, P, false, true, false, 8, pawn_table); // A MODIFIER?
piece pieces[6] = {White_King, White_Queen, White_Knight, White_Bishop, White_rook, White_Pawn};


piece :: piece(const type_piece& t, bool isWhite)
{
    if (t==K)
    {
        (*this) = piece (White_King, isWhite);
    }
     if (t==Q)
    {
        (*this) = piece (White_Queen, isWhite);
    }
    if (t==N)
    {
        (*this) = piece (White_Knight, isWhite);
    }
     if (t==R)
    {
        (*this) = piece (White_rook, isWhite);
    }
    if (t==B)
    {
        (*this) = piece (White_Bishop, isWhite);
    }
     if (t==P)
    {
        (*this) = piece (White_Pawn, isWhite);
    }
}

//===========================================================================
//                          class Coup
//===========================================================================
coup::coup()
{
    this->p = piece();
    this->dep = square();
    this->arr = square();
    this->is_capture = false;
    this->is_special = false;
    this->promotion = nullptr;
};

coup ::coup(const piece p, const square dep, const square arr, const bool is_capture, bool is_special, type_piece* promotion)
{
    this->p = p;
    this->dep = dep;
    this->arr = arr;
    this->is_capture = is_capture;
    this->is_special = is_special;
    this->promotion = promotion;
};

//===========================================================================
//                          class Echiquier
//===========================================================================
Echiquier &Echiquier ::operator=(const Echiquier &e) // constructeur par copie
{
    // désallocation des pièces précédentes
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
};

ostream &operator<<(ostream &os, const piece &p)
{
    if (p.isWhite)
    {
        // La pièce est noire
        os << "W";
        os << nom_piece[p.type];
    }
    else
    {
        // La pièce est blanche
        //"\033[31mTexte en rouge\033[0m"
        os <<  "\033[31m" << "B";
        os << nom_piece[p.type];
        os << "\033[0m";
    }
    return os;
};

ostream &operator<<(ostream &os, const Echiquier &e)
{
    for (int j = 0; j < 8; j++)
    {
        // os << "___";
    }
    os << "\n";
    for (int i = 0; i < 8; i++)

    {
        os << "|";
        for (int j = 0; j < 8; j++)
        {
            piece *case_act = e.board[7 - i][j];
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
            // os << "___";
        }
        // os << "\n";
    }
    return os;
};

void Echiquier ::make_move(const coup &c)
{
    if(c.is_capture && !c.is_special)
    {
        //capture qui n'est pas en_passant
        delete (this->board)[c.arr.i][c.arr.j];
    }
    (this->board)[c.arr.i][c.arr.j] = (this->board)[c.dep.i][c.dep.j];
    (this->board)[c.dep.i][c.dep.j] = nullptr;
     if (c.is_special)
    {
        if(c.p.type == P)
        {
            //c'est une prise en passant
            int di = 1;
            if (!(c.p.isWhite))
            {
                di = -1;
            }
            //c'est forcément une capture
            delete this->board[c.dep.i][c.arr.j];
            this->board[c.dep.i][c.arr.j] = nullptr;
        }
        if (c.p.type == K)
        {
            //c'est un roque
            if (c.arr.j == 6)
            {
                //c'est un petit roque
                this->board[c.dep.i][5] = this->board[c.dep.i][7];
                this->board[c.dep.i][7] = nullptr;
            }
             if (c.arr.j == 2)
            {
                //c'est un petit roque
                this->board[c.dep.i][3] = this->board[c.dep.i][0];
                this->board[c.dep.i][0] = nullptr;
            }
        }
    }
    this->en_passant = nullptr;
    if (c.p.type == P)
    {
        if (abs(c.arr.i - c.dep.i) == 2)
        //delete this->en_passant;
        //un pion a avancé de deux cases -> maj en_passant
        {this->en_passant = new square((c.arr.i + c.dep.i) / 2, c.dep.j);}
        if (c.promotion != nullptr)
            (*(this->board[c.arr.i][c.arr.j])).type = *c.promotion;
    }
    if (c.p.type == K)
    {
        if (c.p.isWhite)
        {
            this->wcpr = false;
            this->wcgr = false;
        }
        else
        {
            this->bcgr = false;
            this->bcpr = false;
        }
    }
    if (c.p.type == R)
    {
        if (c.p.isWhite)
        {
            if(c.dep.j == 7)
            {
                this->wcpr = false;
            }
             if(c.dep.j == 0)
            {
                this->wcgr = false;
            }
        }
        if (!c.p.isWhite)
        {
            if(c.dep.j == 7)
            {
                this->bcpr = false;
            }
             if(c.dep.j == 0)
            {
                this->bcgr = false;
            }
        }

    }
};

void Echiquier :: unmake_move(const coup& c, square* en_passant, type_piece* capture, bool wcpr, bool wcgr, bool bcpr, bool bcgr)
{
    //cout << "Je suis appelee\n";
    if(!c.is_special)
    {
        this->board[c.dep.i][c.dep.j] = this->board[c.arr.i][c.arr.j];
        this->board[c.arr.i][c.arr.j] = nullptr;

        if (c.promotion !=nullptr)
        {
            piece p = c.p;
            delete this->board[c.arr.i][c.arr.j];
            this->board[c.arr.i][c.arr.j] = nullptr;
            this->board[c.dep.i][c.dep.j] = new piece(White_Pawn, p.isWhite);
        }
        if (capture != nullptr)
        {
            //cout << "Piece capturee dans unmake : " << piece(*capture, !c.p.isWhite) << "\n";
            this->board[c.arr.i][c.arr.j] = new piece(*capture, !c.p.isWhite);}
        //cout << "je suis ligne 306\n";
    }
    else
        {
            piece p = c.p;
            if (p.type == K)
            {
                //c'était un roque
                int i_dep = 0;
                if (!p.isWhite)
                {
                    i_dep = 7;
                }
                int j_dep_tour = 7;
                int j_act_tour = 5;
                int j_act_roi = 6;
                if (c.arr.j != 6)
                {
                    //c'était un grand roque
                    j_dep_tour = 0;
                    j_act_tour = 3;
                    j_act_roi = 2;
                }
                this->board[i_dep][4] = this->board[i_dep][j_act_roi];
                this->board[i_dep][j_act_roi] = nullptr;
                this->board[i_dep][j_dep_tour] = this->board[i_dep][j_act_tour];
                this->board[i_dep][j_act_tour] = nullptr;
            }
            else if (p.type == P)
            {
                //c'était une prise en passant
                int di = 1;
                if (!p.isWhite)
                {
                    di = -1;
                }
                this->board[c.dep.i][c.dep.j] = this->board[c.arr.i][c.arr.j];
                this->board[c.arr.i - di][c.arr.j] = new piece(*capture, !p.isWhite);
                this->board[c.arr.i][c.arr.j] = nullptr;
                if (c.promotion !=nullptr)
                {
                    cout << "UNE PRISE EN PASSANT A DONNE UNE PROMOTION!";
                    piece p = c.p;
                    delete this->board[c.dep.i][c.dep.j];
                    this->board[c.dep.i][c.dep.j] = new piece(White_Pawn, p.isWhite);
                }
            }
        }
    this->en_passant = en_passant;
    this->wcpr = wcpr;
    this->wcgr = wcgr;
    this->bcpr = bcpr;
    this->bcgr = bcgr;
}

bool is_legal(const coup &c, Echiquier &e)
{
    piece p = c.p;
    piece* ptr_p_dep = e.board[c.dep.i][c.dep.j];
    bool res = false;
    if (ptr_p_dep !=nullptr || !is_in(c.dep) || !is_in(c.arr))
    {
        piece p_dep = *ptr_p_dep;
        if (p.type != P && p.type == p_dep.type && !c.is_special)    //ce n'est pas un pion
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
                        res =( res || ((s == c.arr) && (p_s->isWhite != p.isWhite)));   //si on pensait que le coup était légal mais qu'une pièce allié est sur la case d'arrivée
                    }
                }
            }
        }
        else if (p.type != P && p.type == p_dep.type && c.is_special)
        {
            if(p.type == K)
            {
                //c'est un roque
                //AJOUTER VERIFICATION QUE LE ROI N'EST PAS EN ECHEC ET QUE LES CASES NE SONT PAS CONTROLEES
                res = (c.dep.j == 4);
                res = res && (c.arr.j == 2 || c.arr.j == 6);
                res = res && (!e.is_att(c.dep, !p.isWhite));        //dep n'est pas attaquée par l'adversaire
                res = res && (!e.is_att(c.arr, !p.isWhite));         //arr n'est pas attaquée par l'adversaire
                int i_dep =0;
                if (!p.isWhite)
                {
                    i_dep = 7;
                }
                res = res && (c.dep.i == i_dep);
                if (c.arr.j == 6)
                {
                    //c'est un petit roque
                    res = res && (e.board[i_dep][5] == nullptr) && (e.board[i_dep][6] == nullptr);
                    res = res && (!e.is_att(square(i_dep, 5), !p.isWhite));
                    //rajouter verif echecs
                }
                else if (c.arr.j == 2)
                {
                    //c'est un grand roque
                    res = res && (e.board[i_dep][2] == nullptr) && (e.board[i_dep][3] == nullptr) && (e.board[i_dep][1] == nullptr);
                     res = res && (!e.is_att(square(i_dep, 2), !p.isWhite) && (!e.is_att(square(i_dep, 3), !p.isWhite)));
                }
                if (c.arr.j == 6 && p.isWhite)
                {
                    return res && e.wcpr;
                }
                 if (c.arr.j == 2 && p.isWhite)
                {
                    return res && e.wcgr;
                }
                if (c.arr.j == 6 && !p.isWhite)
                {
                    return res && e.bcpr;
                }
                 if (c.arr.j == 2 && !p.isWhite)
                {
                    return res && e.bcgr;
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
                if ((c.arr.i - c.dep.i == di && c.arr.j == c.dep.j))
                {
                    res = (e.board[c.arr.i][c.arr.j] == nullptr);
                    res = c.dep.i >=1 && c.dep.i <= 6 && res;
                    if (c.promotion == nullptr)
                    {return res;}
                    else
                    {
                        int i_prom = 7;
                        if (!p.isWhite)
                        {
                            i_prom = 0;
                        }
                        return(res && c.arr.i == i_prom && (*(c.promotion) != P));
                    }
                }
                if (((c.arr.i - c.dep.i ==  2 * di) && (c.arr.j == c.dep.j)))
                {
                    res = (e.board[c.arr.i][c.arr.j] == nullptr);
                    return(c.dep.i == i_dep && e.board[c.arr.i][c.arr.j] == nullptr && e.board[c.dep.i + di][c.arr.j] == nullptr);
                }
            }
            else
            {
                //c'est une capture
                res = (c.arr.i - c.dep.i == di);
                res = res &&(abs(c.arr.j - c.dep.j) == 1);
                piece* ptr_p = e.board[c.arr.i][c.arr.j] ;
                if (ptr_p == nullptr)
                {
                    if (e.en_passant !=nullptr)
                    {
                        return (*(e.en_passant) == c.arr);       //pas de pièce sur la case de capture, ça ne peut être que en passant
                    }
                    else
                    {
                        return false;
                    }
                }
                res = res && (ptr_p->isWhite != p.isWhite);
                if (c.promotion != nullptr)
                {
                    int i_prom = 7;
                    if (!p.isWhite)
                    {
                        i_prom = 0;
                    }
                    return(res && c.arr.i == i_prom && (*(c.promotion) != P));
                }
            }
        }
    }
    return res;
};

Echiquier ::Echiquier()
{

    // Pions et cases vide

    for (int j = 0; j < 8; j++)
    {
        (*this).board[1][j] = new piece(White_Pawn, true);
        (*this).board[6][j] = new piece(White_Pawn, false);
        for (int i = 2; i < 6; i++)
        {
            (*this).board[i][j] = nullptr;
        }
    }

    // Pièces blanches

    (*this).board[0][0] = new piece(White_rook, true);
    (*this).board[0][1] = new piece(White_Knight, true);
    (*this).board[0][2] = new piece(White_Bishop, true);
    (*this).board[0][3] = new piece(White_Queen, true);
    (*this).board[0][4] = new piece(White_King, true);
    (*this).board[0][5] = new piece(White_Bishop, true);
    (*this).board[0][6] = new piece(White_Knight, true);
    (*this).board[0][7] = new piece(White_rook, true);

    // Pièces noires

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
};

bool Echiquier :: is_att(const square& s, bool byWhite)
{
    //cout << "Dans is att\n";
    bool res = false;
    for(int i = 0; i < 8 && !res; i++)
    {
        for (int j = 0; j < 8 && !res; j++)
        {
            //A OPTI EN BOUCLANT SUR LES PIECES?
            piece * ptr_piece = this->board[i][j];
            if (ptr_piece != nullptr)
            {
                piece p = *ptr_piece;
                if(p.isWhite == byWhite)
                {
                    res = res || is_legal(coup(p, square(i,j), s,true, false, nullptr), *this);
                    if (p.type==P)
                    {
                        //on ne regarde pas la prise en passant, mais la promotion
                        res = res || is_legal(coup(p, square(i,j), s, true, false, new type_piece(Q)), *this);
                    }
                }
            }
        }
    }
    return res;
}

bool Echiquier :: ischeck(bool White)
{
    //cout << "Dans ischeck \n";
    bool trouve = false;
    int i_king = 0;
    int j_king = 0;
    for (int i = 0; i< 8 && !trouve; i++)
    {
        for  (int j = 0; j <8 && !trouve; j++)
        {
            piece * ptr_piece = this->board[i][j];
            if (ptr_piece != nullptr)
            {
                piece p= *ptr_piece;
                if (((p.type == K) && (p.isWhite == White)))
                {
                    i_king = i;
                    j_king = j;
                    trouve = true;
                }
            }
        }
    }
    return this->is_att(square(i_king, j_king), !White);
}

float eval(Echiquier &e)
{
    float res = 0;
    for (int i = 0; i < 8; i++)
    {

        for (int j = 0; j < 8; j++)
        {
            // cout << "i : " << i << "j : " << j << "\n";
            piece *case_actuelle = e.board[i][j];
            if (case_actuelle != nullptr)
            {
                piece p = *(case_actuelle);
                if (p.isWhite)
                {
                    res += val_piece[p.type] * 100;
                    //res += p.table[i][j];
                    square s = square(i,j);
                    res += e.nb_cases_ctrl(s);
                }
                else
                {
                    // la pièce est noire
                    res -= val_piece[p.type] * 100;
                    square s = square(i,j);
                    res -= e.nb_cases_ctrl(s);
                    //res -= p.table[7 - i][7 - j];
                }
            }
        }
    }
    // cout << "a survécu";
    return (res / 100);
};

//===========================================================================
//                       fonctions codage standard
//===========================================================================

piece char2p(const char c)        //lettre to pièce (renvoie la pièce blanche)
{
    if (c == 'K' || c == 'k')
    {
        return White_King;
    }
     if (c == 'Q' || c == 'q')
    {
        return White_Queen;
    }
     if (c == 'N' || c == 'n')
    {
        return White_Knight;
    }
     if (c == 'B' || c == 'b')
    {
        return White_Bishop;
    }
    if (c== 'R' || c == 'r')
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
   if (c == 'a' || c == 'A')
    {
        return 0;
    }
     if (c == 'b' || c == 'B')
    {
        return 1;
    }
     if (c == 'c' || c == 'C')
    {
        return 2;
    }
     if (c == 'd' || c == 'D')
    {
        return 3;
    }
     if (c == 'e' || c == 'E')
    {
        return 4;
    }
    if (c == 'f' || c == 'F')
    {
        return 5;
    }
    if (c == 'g' || c == 'G')
    {
        return 6;
    }
    if (c == 'h' || c == 'H')
    {
        return 7;
    }
    else
    {
        return -1;
    }
};

coup alg2coup(char alg[], bool is_white, const Echiquier& e)
{
    //cout << "est rentre\n";
    piece p = piece(char2p(alg[0]), is_white);
    //cout << "p : " << p << "\n";
    if (alg[0] = 'O' && alg[1] == '-' && alg[2] == 'O')
    {
        if(alg[3] == '-' && alg[4] == 'O')
        {
            //c'est un grand roque
            //cout << "Je comprends que grand roque\n";
            int i_dep =0;
            if (!is_white)
            {
                i_dep = 7;
            }
            return coup(piece(char2p('K'), is_white), square(i_dep, 4), square(i_dep, 2),false, true, nullptr);
        }
        else
        {
            //c'est un petit roque
            int i_dep =0;
            if (!is_white)
            {
                i_dep = 7;
            }
            return coup(piece(char2p('K'), is_white), square(i_dep, 4), square(i_dep, 6),false, true, nullptr);
        }
    }
    int j_dep = col2int(alg[1]);
    //cout << "j_dep : "<< j_dep << "\n";
    int i_dep = alg[2] - '0' -  1;
    bool is_capture = (alg[3] == 'x');
    //cout << "dep = (" << i_dep << "," << j_dep << ")\n";
    int j_arr = col2int(alg[4]);
    int i_arr = alg[5] - '0' - 1;
    bool is_special = false;
    //cout << "dep = (" << i_dep << "," << j_dep << ")\n";
    //cout << "arr = (" << i_arr << "," << j_arr << ")\n";
    if (is_capture && (e.board[i_arr][j_arr] == nullptr))
    {
        is_special = true;      //soit en passant soit illégal
    }
    type_piece* promotion = nullptr;
    if (alg[6] == '=')
        promotion = new type_piece ((piece(char2p(alg[7]))).type);
    return coup(p, square(i_dep, j_dep), square(i_arr, j_arr), is_capture, is_special, promotion);
};

coup lit_alg(const char *c_alg, bool is_white)
{
    // on suppose pour l'instant que le coup est légal
    coup cp = coup(); // coup renvoyé
    int l = 0;        // indice dans la chaîne lue
    piece p = piece();
    // p.type = static_cast<type_piece> (l2p(c_alg[l]));  //pièce déplacée
    if (!is_white)
    {
        p.isWhite += false;
    }
    cp.p = p;
    l++;
    cp.is_capture = (c_alg[l] == 'x');
    l++;
    cp.dep.j = col2int(c_alg[l]);
    l++;
    cp.dep.i = c_alg[l] - '0';
    return cp;
};

int Echiquier :: nb_cases_ctrl(square& s)
{
    //cout << "Dans nb_cases_ctrl\n";
    //cout << "(s.i, s.j) = (" << s.i << "," << s.j <<")\n";
    int res = 0;
    piece * ptr_piece = this->board[s.i][s.j];
    if (ptr_piece == nullptr)
    {
        //cout << "case ctrl par rien \n";
        return 0;
    }
    else
    {
        //cout << "nb_cases_ctrl dans le premier else\n";
        piece p = *ptr_piece;
        if (p.type!=P)
        {
            int n = p.nb_dir;
            for(int l = 0; l < n; l++)
            {
                //cout << "l nb cases ctrl : " << l << "\n";
                bool peut_continuer = true;
                for(s += p.moves[l];is_in(s) && peut_continuer; s+=p.moves[l])
                {
                    piece* ptr_p_s = this->board[s.i][s.j];     //eventuelle piece sur la case arr
                    peut_continuer = p.iter;     // si on ne peut pas aller plusieurs fois dans la même direction, on ne peut jamais continuer
                    if (this->board[s.i][s.j] != nullptr)
                    {
                        //on arrive sur une pièce
                        peut_continuer = false;
                        //on compte comme un contrôle même si c'est une pièce alliée
                        res ++;
                    }
                    else
                    {
                        //on arrive sur une case libre
                        res++;
                    }
                }
            }
        }
        else
        {
            //c'est un pion
            //cout << "Cases ctrl par un pion\n";
            if(s.j == 0 || s.j == 7)
            {return 1;}
            else
            {
                return 2;
            }
        }
    }
    //cout << "Sorti à la fin de nb_cases_ctrl\n";
    return res;
}
