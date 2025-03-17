#include "minmax.hpp"
#include "chess.hpp"

//===========================================================================
//                          class Position
//===========================================================================
Position ::Position(Echiquier* e, const bool &white_turn, Position *fille, Position *soeur, const coup &c, square* en_passant, type_piece* capture)
{
    (*this).e = e;
    this->white_turn = white_turn;
    this->fille = fille;
    this->soeur = soeur;
    this->c = c;
    this->capture = capture;
    this->en_passant = en_passant;
}

float Position ::eval_pos() const
{
    return eval(*(this->e));
}

Position ::~Position()
{
    if (this->soeur != nullptr)
    {
        delete this->soeur;
        this->soeur = nullptr;
    }
    if (this->fille != nullptr)
    {
        delete this->fille;
        this->fille = nullptr;
    }
}

void Position :: prof_suivante(int prof)
{
    Echiquier* e  = this->e;
    bool wcpr = e->wcpr;
    bool wcgr = e->wcgr;
    bool bcpr = e->bcpr;
    bool bcgr = e->bcgr;
    square * en_passant = e->en_passant;
    bool should_look = true;
    Position * fille_prec = nullptr;
    bool must_capture = false;
    if (e->ischeck(!this->white_turn))
    {
        //le roi adverse est en échec pendant notre tour, on ne peut pas continuer de jouer
        return;
    }
    if (prof <= 0)
    {
        must_capture = true;
    }
    if (prof < -4)     
    {
        should_look = false;        //on accorde 4 de profondeur supplémentaire pour regarder les captures, passées ces 4, on ne regarde plus rien
    }
    if (should_look)
    {
        //Parcours des cases de l'échiquier
        for(int i = 0; i< 8; i++)
        {
            for(int j = 0; j < 8; j++)
            {
                piece* ptr_piece = e->board[i][j];
                if (ptr_piece != nullptr)
                {
                    //On a trouvé une pièce
                    piece p = *ptr_piece;
                    if (p.type != P && (p.isWhite == this->white_turn))    //ce n'est pas un pion et la pièce est de la bonne couleur
                    {
                        if (p.type == K)
                        {
                            // on regarde les roques, s'ils sont légaux (les autres déplacements du roi sont gérés comme ceux d'une pièce classique)
                            int i_dep = 0;
                            if (!this->white_turn)
                            {
                                i_dep = 7;
                            }
                            coup grand_roque = coup(piece(char2p('K'), this->white_turn), square(i_dep, 4), square(i_dep, 2),false, true, nullptr);
                            if (is_legal(grand_roque, *e))
                            {
                                Position* fille = new Position(e, !white_turn,nullptr, fille_prec, grand_roque, e->en_passant);
                                fille_prec = fille;
                                this->fille = fille;
                            }
                            coup petit_roque = coup(piece(char2p('K'), this->white_turn), square(i_dep, 4), square(i_dep, 6),false, true, nullptr);
                            if (is_legal(petit_roque, *e))
                            {
                                Position* fille = new Position(e, !white_turn,nullptr, fille_prec, petit_roque, e->en_passant);
                                fille_prec = fille;
                                this->fille = fille;
                            }
                        }
                        int n = p.nb_dir;
                        for(int l = 0; l < n; l++)
                        {
                            bool peut_continuer = true;
                            square s = square(i,j);
                            for(s += p.moves[l];is_in(s) && peut_continuer; s+=p.moves[l])
                            {
                                piece* ptr_p_s = e->board[s.i][s.j];     //eventuelle piece sur la case arr
                                peut_continuer = p.iter;     // si on ne peut pas aller plusieurs fois dans la même direction, on ne peut jamais continuer
                                if (e->board[s.i][s.j] != nullptr)
                                {
                                    //on arrive sur une pièce
                                    piece p_s = *(e->board[s.i][s.j]);               //IL SEMBLERAIT QUE CA SOIT DIFFERRENT DE "piece p_s = *(ptr_p_s)" !!!!
                                    peut_continuer = false;
                                    if (p_s.isWhite != this->white_turn)
                                    {
                                        //on a trouvé une capture
                                        coup c = coup(p, square(i,j),s, true);
                                        piece piece_capt = *e->board[s.i][s.j];
                                        type_piece* ptr_type =  new type_piece(piece_capt.type);
                                        e->make_move(c);
                                        if (e->ischeck(this->white_turn))
                                        {
                                            //cette capture met notre roi en échec : elle n'est pas légale
                                            e->unmake_move(c, en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                        }
                                        else
                                        {
                                            Position* fille =new Position(e, !white_turn, nullptr, fille_prec, c, e->en_passant, ptr_type);
                                            fille_prec = fille;
                                            this->fille = fille;
                                            e->unmake_move(c, en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);;
                                        }
                                    }
                                }
                                else
                                {
                                    //on a trouvé un coup qui atterrit sur une case vide
                                    if (!must_capture)
                                    {
                                        coup c = coup(p, square(i,j),s, false);
                                        e->make_move(c);
                                        if (e->ischeck(this->white_turn))
                                        {
                                            //ce coup met notre roi en échec : il n'est en fait pas légal
                                            e->unmake_move(c,en_passant, nullptr, wcpr, wcgr, bcpr, bcgr);
                                        }
                                        else
                                        {
                                            Position* fille = new Position(e, !white_turn,nullptr, fille_prec, coup(p, square(i,j),s, false), e->en_passant);
                                            fille_prec = fille;
                                            this->fille = fille;
                                            e->unmake_move(c,en_passant, nullptr, wcpr, wcgr, bcpr, bcgr);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    else if ((p.type == P) && (p.isWhite == this->white_turn) )        //c'est un pion et il est de la bonne couleur
                    {
                        //on bouge un pion
                        square dep = square(i,j);
                        int di = 1;
                        int i_dep =1;
                        if (!white_turn)
                        {
                            di = -1;
                            i_dep = 6;
                        }
                        type_piece * prom = nullptr;
                        if (i + di == 0 || i + di == 7)
                        {
                            prom = new type_piece(Q);        //l'ordi ne regarde que des promotions en dame par simplicité
                        }
                        if (!must_capture)
                        {
                            //on ne regarde que les captures
                            if (is_in(square(i+di,j)))
                            {
                                if(e->board[i + di][j] == nullptr)
                                {
                                    //la case devant le pion est libre
                                    coup c = coup(p, dep, square(i+di, j), false, false, prom);
                                    e->make_move(c);
                                    if (e->ischeck(this->white_turn))
                                    {
                                        // le coup met notre roi en échec : il n'est en fait pas légal
                                        e->unmake_move(c, en_passant, nullptr, wcpr, wcgr, bcpr, bcgr);
                                    }
                                    else
                                    {
                                        Position * fille = new Position(e, !white_turn, nullptr, fille_prec, c, e->en_passant);
                                        fille_prec = fille;
                                        this->fille = fille;
                                         e->unmake_move(c, en_passant, nullptr, wcpr, wcgr, bcpr, bcgr);
                                    }
                                    if (is_in(square(i+2 * di , j)))
                                    {
                                        if((e->board[i+2 * di ][j] == nullptr) && (i == i_dep))
                                        {
                                            //le pion a le droit d'avancer de deux cases (on est déjà dans le cas où la case juste devant est libre)
                                            coup c = coup(p, dep, square(i+2 * di, j), false, false, prom);
                                            e->make_move(c);
                                            if (e->ischeck(this->white_turn))
                                            {
                                                //le coup megt notre roi en échec, il n'est en fait pas légal
                                                e->unmake_move(c, en_passant, nullptr, wcpr, wcgr, bcpr, bcgr);
                                            }
                                            else
                                            {
                                                Position * fille2 = new Position(e, !white_turn,nullptr, fille_prec,c,e->en_passant);
                                                fille_prec = fille2;
                                                this->fille = fille2;
                                                e->unmake_move(c, en_passant, nullptr, wcpr, wcgr, bcpr, bcgr);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (is_in(square(i+di, j+1)))
                        {
                            if (e->en_passant != nullptr)
                            {
                                if (*(e->en_passant) == square(i+di, j+1))
                                {
                                    //prise en passsant
                                    piece * ptr_piece_capt = e->board[i][j+1];
                                    type_piece* ptr_type = nullptr;
                                    if (ptr_piece_capt != nullptr)
                                    {
                                        ptr_type = new type_piece(ptr_piece_capt->type);
                                    }
                                    coup c = coup(p, dep, square(i+di, j+1), true, true, prom);
                                    e->make_move(c);
                                    if(e->ischeck(this->white_turn))
                                    {
                                        //le coup met notre roi en échec, il n'est en fait pas légal
                                        e->unmake_move(c, en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                    }
                                    else
                                    {
                                        Position * fille = new Position(e, !white_turn,nullptr, fille_prec,c,e->en_passant, ptr_type);
                                        fille_prec = fille;
                                        this->fille = fille;
                                        e->unmake_move(c, en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                    }
                                }
                            }
                            if(e->board[i+di][j+1] != nullptr)
                            {
                                //la case en diagonale du pion est occupée
                                piece p2 = *(e->board[i+di][j+1]);
                                if(p2.isWhite != white_turn)
                                {
                                    //une capture est possible
                                    piece * ptr_piece_capt = e->board[i+di][j+1];
                                    type_piece* ptr_type = nullptr;
                                    if (ptr_piece_capt != nullptr)
                                    {
                                        ptr_type = new type_piece(ptr_piece_capt->type);
                                    }
                                    coup c = coup(p, dep, square(i+di, j+1), true, false, prom);
                                    e->make_move(c);
                                    if(e->ischeck(this->white_turn))
                                    {
                                        //le coup met notre roi en échec, il n'est en fait pas légal
                                        e->unmake_move(c,en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                    }
                                    else
                                    {
                                        Position * fille = new Position(e, !white_turn,nullptr, fille_prec,c, e->en_passant,ptr_type);
                                        //fille->prof_suivante(prof-1);
                                        fille_prec = fille;
                                        this->fille = fille;
                                        e->unmake_move(c,en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                    }
                                }
                            }
                        }
                        if (is_in(square(i+di, j-1)))
                        {
                            if (e->en_passant != nullptr)
                            {
                                if (*(e->en_passant) == square(i+di, j-1))
                                {
                                    //prise en passant
                                    piece * ptr_piece_capt = e->board[i][j-1];
                                    type_piece* ptr_type = nullptr;
                                    //cout << "A trouve une capture\n";
                                    if (ptr_piece_capt != nullptr)
                                    {
                                        ptr_type = new type_piece(ptr_piece_capt->type);
                                    }
                                    coup c = coup(p, dep, square(i+di, j-1), true, true, prom);
                                    e->make_move(c);
                                    if(e->ischeck(this->white_turn))
                                    {
                                        //le coup met notre roi en échec, il n'est en fait pas légal
                                        e->unmake_move(c, en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                    }
                                    else
                                    {
                                        Position * fille = new Position(e, !white_turn,nullptr, fille_prec,c, e->en_passant, ptr_type);
                                        fille_prec = fille;
                                        this->fille = fille;
                                        e->unmake_move(c, en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                    }
                                }
                            }
                            if(e->board[i+di][j-1] != nullptr)
                            {
                                //la case en diagonale du pion est occupée
                                piece p2 = *(e->board[i+di][j-1]);
                                if(p2.isWhite != white_turn)
                                {
                                    //une capture est possible
                                    piece * ptr_piece_capt = e->board[i+di][j-1];
                                    type_piece* ptr_type = nullptr;
                                    if (ptr_piece_capt != nullptr)
                                    {
                                        ptr_type = new type_piece(ptr_piece_capt->type);
                                    }
                                    coup c = coup(p, dep, square(i+di, j-1), true, false, prom);
                                    e->make_move(c);
                                    if (e->ischeck(this->white_turn))
                                    {
                                        //le coup met notre roi en échec, il n'est en fait pas légal
                                        e->unmake_move(c, en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                    }
                                    else
                                    {
                                        Position * fille = new Position(e, !white_turn,nullptr, fille_prec, c, e->en_passant,ptr_type );
                                        fille_prec = fille;
                                        this->fille = fille;
                                        e->unmake_move(c, en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                    }
                                }
                            }
                        }

                    }
                }
            }
        }
    }
};


//===========================================================================
//                          class Meilleur Coup
//===========================================================================
meilleur_coup ::meilleur_coup(const float &eval, const coup &c)
{
    this->evaluation = eval;
    this->c = c;
};

meilleur_coup alpha_beta(Position &pos, int prof, float alpha, float beta, coup c)
{
    // c : meilleur coup trouvé jusqu'à maintenant
    pos.prof_suivante(prof);
    float v = pos.eval_pos();
    if (pos.fille == nullptr)        
    {
        //il n'y a pas de position plus en profondeur à regarder :  cas terminal
        return (meilleur_coup(pos.eval_pos(), c));
    }
    else
    {
        coup coup_opt = c;
        if (pos.white_turn)
        {
            v = -1000;
            int i = 1;
            for (auto fille = pos.fille; fille != nullptr; fille = fille->soeur)
            {
                i++;
                bool wcpr = fille->e->wcpr;
                bool wcgr = fille->e->wcgr;
                bool bcpr = fille->e->bcpr;
                bool bcgr = fille->e->bcgr;
                pos.e->make_move(fille->c);
                meilleur_coup res = alpha_beta(*fille, prof - 1, alpha, beta, c);
                if (res.evaluation > v)
                {
                    coup_opt = fille->c;
                    v = res.evaluation;
                }
                if (beta <= v)
                {
                    Position *temp = pos.fille;
                    pos.fille = nullptr;
                    delete temp;
                    pos.e->unmake_move(fille->c, fille->en_passant, fille->capture, wcpr, wcgr, bcpr, bcgr);
                    return meilleur_coup(v, coup_opt);
                }
                alpha = max(alpha, v);
                pos.e->unmake_move(fille->c, fille->en_passant, fille->capture, wcpr, wcgr, bcpr, bcgr);
            }
            Position *temp = pos.fille;
            pos.fille = nullptr;
            delete temp;
        }
        else
        {
            v = +1000;
            for (auto fille = pos.fille; fille != nullptr; fille = fille->soeur)
            {
                bool wcpr = fille->e->wcpr;
                bool wcgr = fille->e->wcgr;
                bool bcpr = fille->e->bcpr;
                bool bcgr = fille->e->bcgr;
                pos.e->make_move(fille->c);
                meilleur_coup res = alpha_beta(*fille, prof - 1, alpha, beta, fille->c);
                if (res.evaluation < v)
                {
                    coup_opt = fille->c;
                    v = res.evaluation;
                }
                if (alpha >= v)
                {
                    Position *temp = pos.fille;
                    pos.fille = nullptr;
                    delete temp;
                    pos.e->unmake_move(fille->c, fille->en_passant, fille->capture, wcpr, wcgr, bcpr, bcgr);
                    return meilleur_coup(v, coup_opt);
                }
                beta = min(beta, v);
                pos.e->unmake_move(fille->c, fille->en_passant, fille->capture, wcpr, wcgr, bcpr, bcgr);
            }
            Position *temp = pos.fille;
            pos.fille = nullptr;
            delete temp;
        }
        return (meilleur_coup(v, coup_opt));
    }
};
