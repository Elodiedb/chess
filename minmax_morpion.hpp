#ifndef MINMAX_HPP_INCLUDED
#define MINMAX_HPP_INCLUDED

#include "morpion.hpp"

//===========================================================================
//                          class Position
//===========================================================================
class Position_morpion
{
public:
    Position_morpion *fille; // première position fille regardée
    Position_morpion *soeur; // prochaine position regardée
    bool white_turn;         // True ssi aux Blancs de jouer
    Morpion m;
    int coup;
    int eval_pos() const;
    Position_morpion(const Morpion &m, const bool &white_turn, Position_morpion *fille, Position_morpion *soeur, const int &c);
    void prof_suivante(int prof);
    ~Position_morpion();
};

//===========================================================================
//                          class Meilleur Coup
//===========================================================================
class meilleur_coup_morpion
{
public:
    float evaluation;
    int coup;

    meilleur_coup_morpion(const float &eval, const int &c);
};
meilleur_coup_morpion alpha_beta_morpion(Position_morpion &pos, int prof, float alpha, float beta, int c);

#endif
