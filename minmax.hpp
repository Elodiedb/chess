#ifndef MINMAX_HPP_INCLUDED
#define MINMAX_HPP_INCLUDED

#include "chess.hpp"

//===========================================================================
//                          class Position
//===========================================================================
class Position
{
public:
    Echiquier* e;
    bool white_turn; // True ssi aux Blancs de jouer
    Position *fille; // première position fille regardée
    Position *soeur; // prochaine position regardée
    coup c;          // coup permettant de passer de la mère à elle
    square* en_passant;
    type_piece* capture;

    float eval_pos() const;
    Position(Echiquier *e, const bool &white_turn, Position *fille, Position *soeur, const coup &c,square* en_passant, type_piece * capture = nullptr); // fille et soeur seront modifiables!!!
    void prof_suivante(int prof);
    ~Position(); // efface récursivement la position ainsi que les filles et les soeurs
};

//===========================================================================
//                          class Meilleur Coup
//===========================================================================
class meilleur_coup
{
public:
    float evaluation;
    coup c;

    meilleur_coup(const float &eval, const coup &c);
};
meilleur_coup alpha_beta(Position &pos, int prof, float alpha, float beta, coup c);

#endif
