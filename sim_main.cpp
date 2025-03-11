#include <iostream>
#include <string>
#include <array>
using namespace std;
#include "minmax.hpp"

int main()
{
    Echiquier e = Echiquier();
    int prof = 2;
    int n = 0;
    char *coup_alg;
    coup c = coup();
    Historique h = Historique();
    //cout << e;
    cout << "eval : " << eval(e) << "\n";
    // cout << "Entrez un coup";
    Position pos = Position(&e, true, nullptr, nullptr, coup(), nullptr, nullptr);
    int i = 1;
    Position test_mat = Position(pos.e, pos.white_turn, nullptr, nullptr,coup(), nullptr, nullptr);
    test_mat.prof_suivante(1);
    int tmax = 400;
    while (i < tmax && test_mat.fille != nullptr)     //permet de détecter le mat
    {
        //cout << "bcgr : " << (e.bcgr ? "True" : "False") << "\n";
        if (false)     //true pour ordi vs ordi, false pour JvJ, pos.white_turn pour ordi sur les blancs, !pos.white_turn pour ordi sur les noirs
        {
            cout << *(pos.e);
            cout << "tour : " << (i+1) / 2 << "\n";
            // pos.prof_suivante(prof);
            // cout << "prof_suivante generee \n";
            c = coup();
            // cout << "coup par defaut genere";
            meilleur_coup mc = alpha_beta(pos, prof, -1000, +1000, coup());
            type_piece* ptr_type = nullptr;
            if (pos.e->board[c.arr.i][c.arr.j] != nullptr)
            {
                ptr_type = new type_piece(pos.e->board[c.arr.i][c.arr.j]->type);
            }
            cout << "eval estimee :" << mc.evaluation << "\n";
            cout << "Coup joue : " << mc.c << endl;
            pos.e->make_move(mc.c);
            h.update(mc.c, ptr_type, *pos.e);
            // cout << "pos.white_turn : " << (pos.white_turn ? "true" : "false") << "\n";
            //cout << *(pos.e);
            cout << "eval : " << eval(*pos.e) << "\n";
            // pos = Position(pos.e, ! pos.white_turn, nullptr, nullptr, coup());      //A remplacer par nullptr/ destroy ?
            pos.white_turn = !pos.white_turn;
            // cout << "opposition du tour accomplie";
            // delete &pos;
            // cout << "delete accompli\n";
            // Position pos= Position(e, true, nullptr, nullptr, coup());
            // cout << "fin de boucle\n";
            // pos.white_turn = !pos.white_turn;
            // pos.destroy();
        }
        else
        {
            c = coup();
            bool unmake = false;
            bool valide = false;
            while( !(unmake || valide))
            //tour des noirs aka lae joueur.euse
            //cout << "e avant c_in\n" << pos.e;
            {
                cout << *(pos.e);
                cout << "Entrez un coup : ";
                char alg[10];
                for (int j = 0; j < 10; j++)
                {
                    alg[j] = ' ';
                }
                //cout << "e juste avant c_in\n" << pos.e;
                cin >> alg;
                //cout << "e_juste après le c_in\n" << pos.e;
                c = alg2coup(alg, pos.white_turn, *pos.e);
                if (c.p.type == K && c.dep.i == c.arr.i && c.dep.j == c.arr.j && i >= c.dep.i + 1)
                {
                    //Ecrire Kxy-xy pour x une colonne (dans {a,b,..., h} et y un nombre entre 1 et 8 annulera les y derniers coups
                    unmake =true;
                    valide = true;
                    n = c.arr.i;
                }
                if (is_legal(c, *pos.e))
                {
                    char val[10];
                    for (int j = 0; j < 10; j++)
                    {
                        val[j] = ' ';
                    }
                    type_piece* ptr_type = nullptr;
                    if (pos.e->board[c.arr.i][c.arr.j] != nullptr)
                    {
                        ptr_type = new type_piece(pos.e->board[c.arr.i][c.arr.j]->type);
                    }
                    h.update(c, ptr_type, *pos.e);
                    pos.e->make_move(c);
                    cout << *pos.e;
                    cout << "Valider le coup ? (y/n)";
                    cin >> val;
                    if (val[0] == 'y')
                    {
                        valide = true;
                    }
                    else
                    {
                        //cout << "Dans le else\n";
                        valide = false;
                        go_back(*pos.e, 1, i, h);
                        h.go_back(1);
                    }
                    //cout << (valide ? "True" : "false");
                }
                //cout << "lecture terminee\n";
                //cout << "e avant coup\n" << pos.e;
            }
            //cout << (valide ? "True" : "false");
            //cout << "Hors du while";
            if (unmake)
            {
                go_back(*pos.e, n, i-1, h);
                h.go_back(n);
                if (n%2 == 1)
                {
                    pos.white_turn = !pos.white_turn;
                    i = i - n ;
                }
            }
            else
            {
                pos.white_turn = !pos.white_turn;
            }
        }
        i++;
        test_mat = Position(pos.e, pos.white_turn, nullptr, nullptr,coup(), nullptr, nullptr);
        test_mat.prof_suivante(1);
    }
    if (i!= tmax)
    {
        //La partie s'est arrêtée avant d'atteindre le nombre maximal de tour : il y a eu pat ou mat
        if ((pos.e)->ischeck(pos.white_turn))
            {cout << (pos.white_turn ? "Les noirs ont gagne!!" : "Les blancs ont gagne!!");}
        else
            {cout << "C'est un pat";}
    }
};
