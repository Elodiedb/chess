#include <iostream>
#include <string>
#include <array>
using namespace std;
#include "chess.hpp"
#include "minmax.hpp"

int main()
{
    Echiquier e = Echiquier();
    int prof = 4;
    char *coup_alg;
    cout << e;
    cout << "eval : " << eval(e) << "\n";
    // cout << "Entrez un coup";
    Position pos = Position(e, true, nullptr, nullptr, coup());
    int i = 1;
    while (i < 100)
    {
        if (true)
        {
            cout << "tour : " << i << "\n";
            // pos.prof_suivante(prof);
            // cout << "prof_suivante generee \n";
            coup c = coup();
            // cout << "coup par defaut genere";
            meilleur_coup mc = alpha_beta(pos, prof, -1000, +1000, coup());
            cout << "eval estimee :" << mc.evaluation << "\n";
            pos.e.make_move(mc.c);
            // cout << "pos.white_turn : " << (pos.white_turn ? "true" : "false") << "\n";
            cout << pos.e;
            cout << "eval : " << eval(pos.e) << "\n";
            // pos = Position(pos.e, ! pos.white_turn, nullptr, nullptr, coup());      //A remplacer par nullptr/ destroy ?
            pos.white_turn = !pos.white_turn;
            // cout << "opposition du tour accomplie";
            // delete &pos;
            // cout << "delete accompli\n";
            // Position pos= Position(e, true, nullptr, nullptr, coup());
            // cout << "fin de boucle\n";
            // pos.white_turn = !pos.white_turn;
            // pos.destroy();
            i++;
        }
        else
        {
            coup c = coup();
            while(!is_legal(c,pos.e))
            //tour des noirs aka lae joueur.euse
            //cout << "e avant c_in\n" << pos.e;
            {
                cout << "Entrez un coup : ";
                char alg[10];
                for (int i = 0; i < 10; i++)
                {
                    alg[i] = ' ';
                }
                //cout << "e juste avant c_in\n" << pos.e;
                cin >> alg;
                //cout << "e_juste après le c_in\n" << pos.e;
                c = alg2coup(alg, pos.white_turn, pos.e);
                //cout << "lecture terminee\n";
                //cout << "e avant coup\n" << pos.e;
            }
            pos.e.make_move(c);
            pos.white_turn = !pos.white_turn;
            cout << pos.e;
            }
    }
};
