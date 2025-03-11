#include "minmax.hpp"
#include "chess.hpp"

//===========================================================================
//                          class Position
//===========================================================================
Position ::Position(Echiquier* e, const bool &white_turn, Position *fille, Position *soeur, const coup &c, square* en_passant, type_piece* capture)
{
    // nb_pos ++;
    // cout << "nb_pos : "<< nb_pos << "\n";
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
    // cout << "debut delete \n";
    if (this->soeur != nullptr)
    {
        // cout << "delete soeur \n";
        delete this->soeur;
        this->soeur = nullptr;
        // cout << "soeur deleted \n";
    }
    if (this->fille != nullptr)
    {
        // cout << "delete fille \n";
        delete this->fille;
        this->fille = nullptr;
    }

    /*for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            delete this->e.board[i][j]; // Supprime chaque pièce allouée dynamiquement
            this->e.board[i][j] = nullptr;
        }
    }
    */
    // nb_pos--;
    //  cout << "nb_pos : "<< nb_pos << "\n";
}

void Position :: prof_suivante(int prof)
{
    //cout << "Appel a prof " << prof << "\n";
    //int capture_trouvees = 0;
    Echiquier* e  = this->e;
    bool wcpr = e->wcpr;
    bool wcgr = e->wcgr;
    bool bcpr = e->bcpr;
    bool bcgr = e->bcgr;
    square * en_passant = e->en_passant;
    //cout << *e << "\n";
    bool should_look = true;
    Position * fille_prec = nullptr;
    bool must_capture = false;
    if (e->ischeck(!this->white_turn))
    {
        //le roi adverse est en échec
        return;
    }
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
                piece* ptr_piece = e->board[i][j];
                if (ptr_piece != nullptr)
                {
                    piece p = *ptr_piece;
                    // cout << this->white_turn;
                    if (p.type != P && (p.isWhite == this->white_turn))    //ce n'est pas un pion
                    {
                        if (p.type == K)
                        {
                            int i_dep = 0;
                            if (!this->white_turn)
                            {
                                i_dep = 7;
                            }
                            coup grand_roque = coup(piece(char2p('K'), this->white_turn), square(i_dep, 4), square(i_dep, 2),false, true, nullptr);
                            if (is_legal(grand_roque, *e))
                            {
                                //e->make_move(grand_roque);
                                Position* fille = new Position(e, !white_turn,nullptr, fille_prec, grand_roque, e->en_passant);
                                fille_prec = fille;
                                this->fille = fille;
                            }
                            coup petit_roque = coup(piece(char2p('K'), this->white_turn), square(i_dep, 4), square(i_dep, 6),false, true, nullptr);
                            if (is_legal(petit_roque, *e))
                            {
                                //e->make_move(petit_roque);
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
                                        //cout << "nouv_e avant capture\n" << nouv_e << "\n";
                                        coup c = coup(p, square(i,j),s, true);
                                        piece piece_capt = *e->board[s.i][s.j];
                                        type_piece* ptr_type =  new type_piece(piece_capt.type);
                                        e->make_move(c);
                                        if (e->ischeck(this->white_turn))
                                        {
                                            e->unmake_move(c, en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                        }
                                        else
                                        {
                                            //cout << "A trouve une capture de "<< piece_capt << "\n";
                                            //e->make_move(c);
                                            //cout << "nouv_e apres capture\n" << nouv_e << "\n";
                                            //capture_trouvees++;
                                            //cout << "capture_trouvees :"   << capture_trouvees << "\n";
                                            Position* fille =new Position(e, !white_turn, nullptr, fille_prec, c, e->en_passant, ptr_type);
                                            //fille->prof_suivante(prof - 1);
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
                                        //cout << "!must capture piece\n";
                                        //cout << "nouv_e avant coup \n" << nouv_e << "\n";
                                        //e->make_move(coup(p, square(i,j),s, false));
                                        //cout << "nouv_e apres coup \n" << nouv_e << "\n";
                                        //cout << this->white_turn << "\n";
                                        //cout << nouv_e;
                                        //cout << "\n";
                                        coup c = coup(p, square(i,j),s, false);
                                        e->make_move(c);
                                        if (e->ischeck(this->white_turn))
                                        {
                                            e->unmake_move(c,en_passant, nullptr, wcpr, wcgr, bcpr, bcgr);
                                        }
                                        else
                                        {
                                            Position* fille = new Position(e, !white_turn,nullptr, fille_prec, coup(p, square(i,j),s, false), e->en_passant);
                                            //fille->prof_suivante(prof-1);
                                            fille_prec = fille;
                                            this->fille = fille;
                                            e->unmake_move(c,en_passant, nullptr, wcpr, wcgr, bcpr, bcgr);
                                        }
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
                        type_piece * prom = nullptr;
                        if (i + di == 0 || i + di == 7)
                        {
                            prom = new type_piece(Q);
                        }
                        if (!must_capture)
                        {
                            //cout << "white_turn\n";
                            if (is_in(square(i+di,j)))
                            {
                                //cout << "case devant est dans l'echiquier \n";
                                if(e->board[i + di][j] == nullptr)
                                {
                                    //la case devant le pion est libre
                                    //cout << "pion blanc avance d'une case \n";
                                    //cout << "nouv_e avant coup \n" << nouv_e << "\n";
                                    //e->make_move(coup(p, dep, square(i+di, j), false));
                                    //cout << "nouv_e apres coup \n" << nouv_e << "\n";
                                    coup c = coup(p, dep, square(i+di, j), false, false, prom);
                                    e->make_move(c);
                                    if (e->ischeck(this->white_turn))
                                    {
                                        e->unmake_move(c, en_passant, nullptr, wcpr, wcgr, bcpr, bcgr);
                                    }
                                    else
                                    {
                                        Position * fille = new Position(e, !white_turn, nullptr, fille_prec, c, e->en_passant);
                                        //fille->prof_suivante(prof-1);
                                        fille_prec = fille;
                                        this->fille = fille;
                                        //cout << "on avance le pion " << j << "\n";
                                        //cout << this->white_turn << "\n";
                                        //cout << nouv_e;
                                        //cout << "\n";
                                         e->unmake_move(c, en_passant, nullptr, wcpr, wcgr, bcpr, bcgr);
                                    }
                                    if (is_in(square(i+2 * di , j)))
                                    {
                                        if((e->board[i+2 * di ][j] == nullptr) && (i == i_dep))
                                        {
                                            //cout << "pion blanc avance de deux cases \n";
                                            //le pion peut avancer de deux cases
                                            //cout << "nouv_nouv_e avant coup \n" << nouv_nouv_e << "\n";
                                            //e->make_move(coup(p, dep, square(i+2 * di, j), false));
                                            //cout << "nouv_nouv_e apres coup \n" << nouv_nouv_e << "\n";
                                            //cout << "avant l'appel de new position\n";
                                            coup c = coup(p, dep, square(i+2 * di, j), false, false, prom);
                                            e->make_move(c);
                                            if (e->ischeck(this->white_turn))
                                            {
                                                e->unmake_move(c, en_passant, nullptr, wcpr, wcgr, bcpr, bcgr);
                                            }
                                            else
                                            {
                                                Position * fille2 = new Position(e, !white_turn,nullptr, fille_prec,c,e->en_passant);
                                                //cout << "avant l'appel de prof_suivante \n";
                                                //fille2->prof_suivante(prof-1);
                                                //cout << "la fille a été gérée \n";
                                                fille_prec = fille2;
                                                this->fille = fille2;
                                                e->unmake_move(c, en_passant, nullptr, wcpr, wcgr, bcpr, bcgr);
                                            }
                                        }
                                    }
                                //cout << "on a géré les avancées de un et deux\n";
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
                                     //cout << "prise en diagonale \n";
                                    //e->make_move(coup(p, dep, square(i+di, j+1), true));
                                    //capture_trouvees++;
                                    //cout << "capture_trouvees :"   << capture_trouvees << "\n";
                                    piece * ptr_piece_capt = e->board[i][j+1];
                                    type_piece* ptr_type = nullptr;
                                    //cout << "A trouve une capture\n";
                                    if (ptr_piece_capt != nullptr)
                                    {
                                        ptr_type = new type_piece(ptr_piece_capt->type);
                                    }
                                    coup c = coup(p, dep, square(i+di, j+1), true, true, prom);
                                    e->make_move(c);
                                    if(e->ischeck(this->white_turn))
                                    {
                                        e->unmake_move(c, en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                    }
                                    else
                                    {
                                        //piece * ptr_capt = e->board[i][j+1];
                                        Position * fille = new Position(e, !white_turn,nullptr, fille_prec,c,e->en_passant, ptr_type);
                                        //fille->prof_suivante(prof-1);
                                        fille_prec = fille;
                                        this->fille = fille;
                                        e->unmake_move(c, en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                    }
                                }
                            }
                            //cout << "is_in diagonale droite\n";
                            if(e->board[i+di][j+1] != nullptr)
                            {
                                //la case en diagonale du pion est occupée
                                piece p2 = *(e->board[i+di][j+1]);
                                if(p2.isWhite != white_turn)
                                {
                                    //une capture est possible
                                    //e->make_move(coup(p, dep, square(i+di, j+1), true));
                                    //capture_trouvees++;
                                    //cout << "capture_trouvees :"   << capture_trouvees << "\n";
                                    piece * ptr_piece_capt = e->board[i+di][j+1];
                                    type_piece* ptr_type = nullptr;
                                    //cout << "A trouve une capture\n";
                                    if (ptr_piece_capt != nullptr)
                                    {
                                        ptr_type = new type_piece(ptr_piece_capt->type);
                                    }
                                    coup c = coup(p, dep, square(i+di, j+1), true, false, prom);
                                    e->make_move(c);
                                    if(e->ischeck(this->white_turn))
                                    {
                                        e->unmake_move(c,en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                    }
                                    else
                                    {
                                        //piece * ptr_capt = e->board[i + di][j+1];
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
                                    //cout << "Je suis dans LA prise en passant\n";
                                    //prise en passant
                                     //cout << "prise en diagonale \n";
                                    //e->make_move(coup(p, dep, square(i+di, j-1), true));
                                    //capture_trouvees++;
                                    //cout << "capture_trouvees :"   << capture_trouvees << "\n";
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
                                        e->unmake_move(c, en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                    }
                                    else
                                    {
                                        //piece* ptr_capt = e->board[i][j-1];
                                        Position * fille = new Position(e, !white_turn,nullptr, fille_prec,c, e->en_passant, ptr_type);
                                        //fille->prof_suivante(prof-1);
                                        fille_prec = fille;
                                        this->fille = fille;
                                        e->unmake_move(c, en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                    }
                                }
                            }
                            //cout << "is_in diagonale gauche \n";
                            if(e->board[i+di][j-1] != nullptr)
                            {
                                //cout << "diagonale gauche occupee \n";
                                //la case en diagonale du pion est occupée
                                piece p2 = *(e->board[i+di][j-1]);
                                if(p2.isWhite != white_turn)
                                {
                                    //une capture est possible
                                    //cout << "prise en diagonale \n";
                                    //e->make_move(coup(p, dep, square(i+di, j-1), true));
                                    //capture_trouvees++;
                                    //cout << "capture_trouvees :"   << capture_trouvees << "\n";
                                    piece * ptr_piece_capt = e->board[i+di][j-1];
                                    type_piece* ptr_type = nullptr;
                                    //cout << "A trouve une capture\n";
                                    if (ptr_piece_capt != nullptr)
                                    {
                                        ptr_type = new type_piece(ptr_piece_capt->type);
                                    }
                                    coup c = coup(p, dep, square(i+di, j-1), true, false, prom);
                                    e->make_move(c);
                                    if (e->ischeck(this->white_turn))
                                    {
                                        e->unmake_move(c, en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                    }
                                    else
                                    {
                                        //piece * ptr_capt = e->board[i+di][j-1];
                                        Position * fille = new Position(e, !white_turn,nullptr, fille_prec, c, e->en_passant,ptr_type );
                                        //fille->prof_suivante(prof-1);
                                        fille_prec = fille;
                                        this->fille = fille;
                                        e->unmake_move(c, en_passant, ptr_type, wcpr, wcgr, bcpr, bcgr);
                                    }
                                }
                            }
                            //cout << "fin traitement diagonale gauche\n";
                        }
                        if (e->en_passant != nullptr)
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
    // cout << "Un appel \n";
    pos.prof_suivante(prof);
    // cout << "prof_suivante_generee\n";
    float v = pos.eval_pos();
    if (pos.fille == nullptr)        //A MODIFIER?
    {
        // cout<< "prof nulle \n" ;
        return (meilleur_coup(pos.eval_pos(), c));
    }
    // cout << pos.fille->e;
    else
    {
        // pos.prof_suivante(prof);
        if (pos.fille == nullptr)
        {
            cout << "Je suis la reine des salopes ! ";
        }
        coup coup_opt = c;
        // cout << e;
        if (pos.white_turn)
        {
            v = -1000;
            int i = 1;
            for (auto fille = pos.fille; fille != nullptr; fille = fille->soeur)
            {
                i++;
                //cout << "mere white turn : " << (pos.white_turn ? "true" : "false") << "fille white turn : " << (fille->white_turn ? "true" : "false") << "\n";
                bool wcpr = fille->e->wcpr;
                bool wcgr = fille->e->wcgr;
                bool bcpr = fille->e->bcpr;
                bool bcgr = fille->e->bcgr;
                pos.e->make_move(fille->c);
                //cout << "position fille blanche alpha-beta  : \n" << *(fille->e) << "\n";
                meilleur_coup res = alpha_beta(*fille, prof - 1, alpha, beta, c);
                //cout << "meilleur coup des noirs trouve \n";
                // cout << "res.ealuation : " << res.evaluation << endl;
                if (res.evaluation > v)
                {
                    // cout<< "Mieux que l'opt blanc " << "white_turn " << (pos.white_turn ? "true" : "false") << " v : " << v <<  " nouv_v : " << res.evaluation  << "\n";
                    coup_opt = fille->c;
                    v = res.evaluation;
                    // cout << "v : " << v<< "\n";
                }
                if (beta <= v)
                {
                    // cout << "coupure alpha\n";
                    Position *temp = pos.fille;
                    pos.fille = nullptr;
                    delete temp;
                    pos.e->unmake_move(fille->c, fille->en_passant, fille->capture, wcpr, wcgr, bcpr, bcgr);
                    return meilleur_coup(v, coup_opt);
                }
                // Position* temp = fille;
                // fille = fille->soeur;
                // delete temp;  // Libère l'ancienne position
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
                // cout << "mere white turn : " << (pos.white_turn ? "true" : "false") << "fille white turn : " << (fille->white_turn ? "true" : "false") << "\n";
                bool wcpr = fille->e->wcpr;
                bool wcgr = fille->e->wcgr;
                bool bcpr = fille->e->bcpr;
                bool bcgr = fille->e->bcgr;
                pos.e->make_move(fille->c);
                //cout << "position fille black turn \n" << *(fille->e) << "\n";
                meilleur_coup res = alpha_beta(*fille, prof - 1, alpha, beta, fille->c);
                //cout << "meilleur coup des blancs trouve \n";
                if (res.evaluation < v)
                {
                    // cout << prof << endl;
                    // cout << "position mere : \n" << pos.e << "\n";
                    // cout << "position fille black turn \n" << fille->e << "\n";
                    // cout<< "Mieux que l'opt " << "white_turn " << (pos.white_turn ? "true" : "false") << " v : " << v <<  " nouv_v : " << res.evaluation  << "\n";
                    coup_opt = fille->c;
                    v = res.evaluation;
                }
                if (alpha >= v)
                {
                    // cout << "alpha : " << alpha << " v: " << v << " coupure alpha \n";
                    Position *temp = pos.fille;
                    pos.fille = nullptr;
                    delete temp;
                    //cout << "unmake move black pruning\n";
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
        // cout << "arrive au return \n";
        // cout << "v au return : " << v << "\n";
        return (meilleur_coup(v, coup_opt));
    }
};
