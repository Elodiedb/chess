#include "minmax_morpion.hpp"
#include "morpion.hpp"
#include <algorithm>
#include <iostream>

//===========================================================================
//                          class Position_morpion
//===========================================================================
Position_morpion::Position_morpion(const Morpion &m, const bool &white_turn, Position_morpion *fille, Position_morpion *soeur, const int &coup)
{
    this->m = m;
    this->white_turn = white_turn;
    this->fille = fille;
    this->soeur = soeur;
    this->coup = coup;
}

int Position_morpion::eval_pos() const
{
    int n = this->m.verifier_victoire();
    if (n == 1)
        return 10;
    else if (n == 2)
        return -10;
    else if (n == 3)
        return 0;

    int score = 0;

    // Centre (valeur la plus élevée)
    if (m.grille[1][1] == 'X')
        score += 3;
    else if (m.grille[1][1] == 'O')
        score -= 3;

    // Coins (valeur moyenne)
    if (m.grille[0][0] == 'X')
        score += 2;
    else if (m.grille[0][0] == 'O')
        score -= 2;

    if (m.grille[0][2] == 'X')
        score += 2;
    else if (m.grille[0][2] == 'O')
        score -= 2;

    if (m.grille[2][0] == 'X')
        score += 2;
    else if (m.grille[2][0] == 'O')
        score -= 2;

    if (m.grille[2][2] == 'X')
        score += 2;
    else if (m.grille[2][2] == 'O')
        score -= 2;

    // Bords (valeur faible)
    if (m.grille[0][1] == 'X')
        score += 1;
    else if (m.grille[0][1] == 'O')
        score -= 1;

    if (m.grille[1][0] == 'X')
        score += 1;
    else if (m.grille[1][0] == 'O')
        score -= 1;

    if (m.grille[1][2] == 'X')
        score += 1;
    else if (m.grille[1][2] == 'O')
        score -= 1;

    if (m.grille[2][1] == 'X')
        score += 1;
    else if (m.grille[2][1] == 'O')
        score -= 1;

    return score;
}

void Position_morpion::prof_suivante(int prof)
{
    Morpion e = this->m;
    int n = e.verifier_victoire();

    if (n != 0 || prof <= 0)
    {
        this->fille = nullptr;
        return;
    }

    if (this->fille != nullptr)
    {
        Position_morpion *current = this->fille;
        while (current != nullptr)
        {
            Position_morpion *next = current->soeur;
            current->soeur = nullptr;
            delete current;
            current = next;
        }
        this->fille = nullptr;
    }

    // Ordre optimal: centre, coins, bords
    const int ordre_optimal[9][2] = {
        {1, 1},
        {0, 0},
        {0, 2},
        {2, 0},
        {2, 2},
        {0, 1},
        {1, 0},
        {1, 2},
        {2, 1}};

    Position_morpion *fille_prec = nullptr;

    for (int k = 0; k < 9; k++)
    {
        int i = ordre_optimal[k][0];
        int j = ordre_optimal[k][1];

        if (e.grille[i][j] == ' ')
        {
            Morpion nouv_e = e;
            nouv_e.jouer(this->white_turn ? 1 : 2, i, j);

            Position_morpion *fille = new Position_morpion(nouv_e, !this->white_turn, nullptr, nullptr, i * 3 + j);

            if (fille_prec != nullptr)
                fille_prec->soeur = fille;
            else
                this->fille = fille;

            fille_prec = fille;

            if (prof > 1)
                fille->prof_suivante(prof - 1);
        }
    }
}

Position_morpion::~Position_morpion()
{
    Position_morpion *current = this->fille;
    while (current != nullptr)
    {
        Position_morpion *next = current->soeur;
        current->soeur = nullptr;
        delete current;
        current = next;
    }
}

//===========================================================================
//                          class Meilleur Coup
//===========================================================================
meilleur_coup_morpion::meilleur_coup_morpion(const float &eval, const int &c)
{
    this->evaluation = eval;
    this->coup = c;
}

meilleur_coup_morpion alpha_beta_morpion(Position_morpion &pos, int prof, float alpha, float beta, int coup)
{
    static int compteur = 0;
    if (++compteur > 100000)
    {
        std::cout << "ARRÊT FORCÉ après " << compteur << " appels" << std::endl;
        compteur = 0;
        return meilleur_coup_morpion(0, coup);
    }

    int etat_jeu = pos.m.verifier_victoire();
    if (etat_jeu != 0)
    {
        float eval;
        if (etat_jeu == 1)
            eval = 10 + prof;
        else if (etat_jeu == 2)
            eval = -10 - prof;
        else
            eval = 0;
        return meilleur_coup_morpion(eval, coup);
    }

    if (prof <= 0)
        return meilleur_coup_morpion(pos.eval_pos(), coup);

    pos.fille = nullptr;
    pos.prof_suivante(1);

    if (pos.fille == nullptr)
        return meilleur_coup_morpion(pos.eval_pos(), coup);

    int coup_opt = coup;
    float v;

    if (pos.white_turn)
    {
        v = -10000;
        for (Position_morpion *fille = pos.fille; fille != nullptr; fille = fille->soeur)
        {
            meilleur_coup_morpion res = alpha_beta_morpion(*fille, prof - 1, alpha, beta, fille->coup);
            if (res.evaluation > v)
            {
                coup_opt = fille->coup;
                v = res.evaluation;
            }
            alpha = std::max(alpha, v);
            if (beta <= alpha)
                break;
        }
    }
    else
    {
        v = 10000;
        for (Position_morpion *fille = pos.fille; fille != nullptr; fille = fille->soeur)
        {
            meilleur_coup_morpion res = alpha_beta_morpion(*fille, prof - 1, alpha, beta, fille->coup);
            if (res.evaluation < v)
            {
                coup_opt = fille->coup;
                v = res.evaluation;
            }
            beta = std::min(beta, v);
            if (beta <= alpha)
                break;
        }
    }

    if (prof >= 8)
        compteur = 0;

    return meilleur_coup_morpion(v, coup_opt);
}