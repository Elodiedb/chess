#ifndef MORPION_HPP_INCLUDED
#define MORPION_HPP_INCLUDED

class Morpion
{
public:
    Morpion();
    void afficher_grille();
    bool jouer(int joueur, int ligne, int colonne);
    int verifier_victoire() const;
    char grille[3][3];
};

#endif