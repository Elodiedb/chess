#include "chess.hpp"

char nom_piece[6] = {'K', 'Q', 'N', 'B', 'R', 'P'};
int val_piece[6] = {500, 9, 3, 3, 5, 1};
int poids_cases_ctrl[6] = {-5, 1, 3, 3, 2, 2};
int poids_cases_prot[6] = {0, 0, 2,2,1,4};
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
        // La pièce est blanche
        os << "W";
        os << nom_piece[p.type];
    }
    else
    {
        // La pièce est noire
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


ostream& operator << (ostream &os, const coup &c)
{
    os << nom_piece[c.p.type];
    os << char('a' + c.dep.j);
    os << c.dep.i + 1;
    os << (c.is_capture ? 'x': '-');
    os << char('a' + c.arr.j);
    os << c.arr.i + 1;
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
        // un roi a bougé : mise à jour des booléens de roque
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
        // une tour a bougé : mise à jour des booléens de roque
        if (c.p.isWhite)
        {
            if(c.dep.j == 7 && c.dep.i == 0)
            {
                this->wcpr = false;
            }
             if(c.dep.j == 0 && c.dep.i == 0)
            {
                this->wcgr = false;
            }
        }
        if (!c.p.isWhite)
        {
            if(c.dep.j == 7 && c.dep.i == 7)
            {
                this->bcpr = false;
            }
             if(c.dep.j == 0 && c.dep.i == 7)
            {
                this->bcgr = false;
            }
        }

    }
};

void Echiquier :: unmake_move(const coup& c, square* en_passant, type_piece* capture, bool wcpr, bool wcgr, bool bcpr, bool bcgr)
{
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
            // On remet la piècen qui avait été capturée
            this->board[c.arr.i][c.arr.j] = new piece(*capture, !c.p.isWhite);}
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
                    // Logiquement ce cas n'est jamais appelé : une prise en passant ne peut pas aboutir sur une promotion
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
    if (ptr_p_dep !=nullptr && is_in(c.dep) && is_in(c.arr))
    {
        // Il y avait bien une pièce sur la case de départ, qui est bien à l'intérieur de l'échiquier
        piece p_dep = *ptr_p_dep;
        if (p.type != P && p.type == p_dep.type && !c.is_special)    //ce n'est pas un pion, ce n'est pas un coup spécial et le type de pièce est cohérent
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
            //ce n'est pas un pion mais le coup est un coup spécial (et le type est cohérent)
            if(p.type == K)
            {
                //c'est un roque
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
            //C'est un coup de pion (et le type est cohérent)
            int di = 1;
            if (!p.isWhite)
            {
                di = -1;
            }
            int i_dep = 1;            //rangée de départ des pions de la couleur
            if (!p.isWhite)
            {
                i_dep = 6;
            }
            if (!c.is_capture)
            {
                if ((c.arr.i - c.dep.i == di && c.arr.j == c.dep.j))
                {
                    // C'est un déplacement d'une case vers "l'avant", cohérent avec un coup de pion
                    res = (e.board[c.arr.i][c.arr.j] == nullptr);            //la case d'arrivée est libre
                    res = c.dep.i >=1 && c.dep.i <= 6 && res;
                    if (c.promotion == nullptr)
                    {return res && (c.arr.i != 7) && (c.arr.i) != 0;}  //il n'est jamais légal de déplacer un pion sur la toute première ou toute dernière rangée (hors promotion)
                    else
                    {
                        //c'est une promotion
                        int i_prom = 7;        //rangée de promotion pour la couleur concernée
                        if (!p.isWhite)
                        {
                            i_prom = 0;
                        }
                        return(res && c.arr.i == i_prom && (*(c.promotion) != P) && (*(c.promotion) != K));      //on peut promouvoir en tout sauf roi et pion
                    }
                }
                if (((c.arr.i - c.dep.i ==  2 * di) && (c.arr.j == c.dep.j)))
                {
                    //C'est un coup de deux cases vers "l'avant"
                    res = (e.board[c.arr.i][c.arr.j] == nullptr);
                    return(c.dep.i == i_dep && e.board[c.arr.i][c.arr.j] == nullptr && e.board[c.dep.i + di][c.arr.j] == nullptr);
                }
            }
            else
            {
                //c'est une capture
                res = (c.arr.i - c.dep.i == di);        //c'est un déplacement de 1 vers "l'avant"
                res = res &&(abs(c.arr.j - c.dep.j) == 1);        // c'est un déplacement de 1 sur le côté
                piece* ptr_p = e.board[c.arr.i][c.arr.j] ;
                if (ptr_p == nullptr)
                {
                    //C'est une capture qui arrive sur une case libre, ça ne peut être qu'une prise en passant
                    if (e.en_passant !=nullptr)
                    {
                        return (*(e.en_passant) == c.arr);
                    }
                    else
                    {
                        return false;
                    }
                }
                // La case d'arrivée était occupée
                res = res && (ptr_p->isWhite != p.isWhite);        //la pièce capturée n'est pas de la couleur du pion déplacé
                if (c.promotion != nullptr)
                {
                    // C'est une capture qui aboutit à une promotion
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
    //Crée l'échiquier de départ

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
    bool res = false;
    for(int i = 0; i < 8 && !res; i++)
    {
        for (int j = 0; j < 8 && !res; j++)
        {
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
                        //(ie il est légal de capturer sur la case s avec le pion tout en faisant une promotion (en dame))
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
    bool trouve = false;
    int i_king = 0;
    int j_king = 0;
    // on cherche le roi potentiellement en échec sur l'échiquier
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
    //on parcourt les cases de l'échiquiers
    for (int i = 0; i < 8; i++)
    {

        for (int j = 0; j < 8; j++)
        {
            piece *case_actuelle = e.board[i][j];
            if (case_actuelle != nullptr)
            {
                piece p = *(case_actuelle);
                if (p.isWhite)
                {
                    res += val_piece[p.type] * 100;        //valeur  brute de la pièce
                    res += p.table[i][j];            //valeur de la table
                    square s = square(i,j);
                    res += poids_cases_ctrl[p.type] * e.nb_cases_ctrl(s).first;        //valeur due aux cases contrôlées
                    res += poids_cases_prot[p.type] * e.nb_cases_ctrl(s).second;        //valeur due aux cases protégées
                }
                else
                {
                    // la pièce est noire
                    res -= val_piece[p.type] * 100;         //valeur  brute de la pièce
                    square s = square(i,j);
                    res -= poids_cases_ctrl[p.type] * e.nb_cases_ctrl(s).first;      //valeur due aux cases contrôlées
                    res -=  poids_cases_prot[p.type] * e.nb_cases_ctrl(s).second;     //valeur due aux cases protégées
                    res -= p.table[7 - i][j];      //valeur de la table (renversée car table prévue pour les blancs)
                }
            }
        }
    }
    return (res / 100);        //on a multiplié par 100 pour éviter les arrondis dus aux flottants mais on renvoie une évalutaion standard (le pion vaut 1)
};

void go_back(Echiquier& e, int n, int n_tour, Historique h)
{
    //revient n tour(s) en arrière sachant quee nous sommes aactuellement au n_tour ème tour de la partie
    for(int i= n_tour - 1; i>=0 && i >= n_tour - n; i -- )
    {
        e.unmake_move(h.c[i], h.en_passant[i], h.capture[i], h.wcpr[i], h.wcgr[i], h.bcpr[i], h.bcgr[i]);
    }
}

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
        return 0;     // l'entrée ne correspond pas à une colonne standard, on renvoie l'équivalent de 'a' pour éviter un débordement, l'utilisateur pourra annuler
    }
};

coup alg2coup(char alg[], bool is_white, const Echiquier& e)
{
    piece p = piece(char2p(alg[0]), is_white);
    if (alg[0] = 'O' && alg[1] == '-' && alg[2] == 'O')
    {
        //c'est un roque
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
    int i_dep = alg[2] - '0' -  1;
    bool is_capture = (alg[3] == 'x');
    int j_arr = col2int(alg[4]);
    int i_arr = alg[5] - '0' - 1;
    bool is_special = false;
    if (is_capture && (e.board[i_arr][j_arr] == nullptr))
    {
        is_special = true;      //soit en passant soit illégal (capture sans pièce sur la case d'arrivée)
    }
    type_piece* promotion = nullptr;
    if (alg[6] == '=')
        promotion = new type_piece ((piece(char2p(alg[7]))).type);
    return coup(p, square(i_dep, j_dep), square(i_arr, j_arr), is_capture, is_special, promotion);
};

coup lit_alg(const char *c_alg, bool is_white)
{
    //fonction obsolète
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

pair<int, int> Echiquier :: nb_cases_ctrl(const square& s)
{
    //Donne le nombre de cases controlées par la pièce sur le square s et le nombre de cases protégées (ie avec allié dessus)
    square r = square(s.i, s.j);
    int ctrl = 0;
    int prot = 0;
    piece * ptr_piece = this->board[s.i][s.j];
    if (ptr_piece == nullptr)
    {
        return pair<int, int>(ctrl, prot);
    }
    else
    {
        piece p = *ptr_piece;
        if (p.type!=P)
        {
            int n = p.nb_dir;
            for(int l = 0; l < n; l++)
            {
                square r = square(s.i, s.j);
                bool peut_continuer = true;
                for(r += p.moves[l];is_in(r) && peut_continuer; r+=p.moves[l])
                {
                    piece* ptr_p_s = this->board[r.i][r.j];     //eventuelle piece sur la case arr
                    peut_continuer = p.iter;     // si on ne peut pas aller plusieurs fois dans la même direction, on ne peut jamais continuer
                    if (this->board[r.i][r.j] != nullptr)
                    {
                        //on arrive sur une pièce
                        peut_continuer = false;
                        //on compte comme un protégée si c'est une pièce alliée
                        piece p_adv = *board[r.i][r.j];
                        if (p_adv.isWhite == p.isWhite)
                            {prot ++;}
                        else
                        {
                            ctrl++;
                        }
                    }
                    else
                    {
                        //on arrive sur une case libre
                        ctrl++;
                    }
                }
            }
        }
        else
        {
            //c'est un pion
            int di = (p.isWhite ? +1 : -1);
            if(s.j == 0 || s.j == 7)
            {
                int dj = (s.j==0 ? 1 : -1);
                if (this->board[s.i + di][s.j + dj] == nullptr)
                {
                    ctrl ++;
                }
                else
                {
                    piece p_adv = *board[s.i + di][s.j+dj];
                    if (p_adv.isWhite == p.isWhite)
                    {prot++;}
                    else
                    {
                        ctrl++;
                    }
                }
                return pair<int, int>(ctrl, prot);
            }
            else
            {
                for (int dj = -1; dj <= 1; dj += 2)
                {
                    if (this->board[s.i + di][s.j + dj] == nullptr)
                    {
                        ctrl ++;
                    }
                    else
                    {
                        prot++;
                    }
                }
                return pair<int, int>(ctrl, prot);
            }
        }
    }
    return pair<int, int>(ctrl, prot);
}

//Classe historique
Historique :: Historique()
{
    this->c = vector<coup> ();
    this->en_passant = vector <square*> ();
    this->capture = vector <type_piece*> ();
    this->wcpr = vector <bool> ();
    this->wcgr = vector <bool> ();
    this->bcpr = vector <bool> ();
    this->bcgr = vector <bool> ();
}

void Historique :: update(coup c, type_piece* capture, Echiquier& e)
{
    this->c.push_back(c);
    this->en_passant.push_back(e.en_passant);
    this->capture.push_back(capture);
    this->wcpr.push_back(e.wcpr);
    this->wcgr.push_back(e.wcgr);
    this->bcpr.push_back(e.bcpr);
    this->bcgr.push_back(e.bcgr);
}

void Historique :: go_back(int n)
{
    for(int i = 1; i < n; i++)
    {
        this->c.pop_back();
        this->en_passant.pop_back();
        this->capture.pop_back();
        this->wcpr.pop_back();
        this->wcgr.pop_back();
        this->bcpr.pop_back();
        this->bcgr.pop_back();
    }
}

int char2ind_cond(char cond[])
{
    if (cond[0] == 'o' && cond[2] == 'o')
    {
        return 0;
    }
     if (cond[0] == 'j' && cond[2] == 'j')
    {
        return 1;
    }
    if (cond[0] == 'j' && cond[2] == 'o')
    {
        return 3;
    }
     if (cond[0] == 'o' && cond[2] == 'j')
    {
        return 2;
    }
    return 0;

}
