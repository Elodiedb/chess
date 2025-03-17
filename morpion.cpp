#include <iostream>
#include "morpion.hpp"

using namespace std;

Morpion::Morpion()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            grille[i][j] = ' ';
        }
    }
}

void Morpion::afficher_grille()
{
    for (int i = 0; i < 3; i++)
    {
        cout << " ";
        for (int j = 0; j < 3; j++)
        {
            cout << grille[i][j];
            if (j < 2)
                cout << "|";
        }
        cout << "\n";
        if (i < 2)
            cout << " -----\n";
    }
}

bool Morpion::jouer(int joueur, int ligne, int colonne)
{
    if (ligne < 0 || ligne >= 3 || colonne < 0 || colonne >= 3 || grille[ligne][colonne] != ' ')
    {
        return false;
    }
    grille[ligne][colonne] = (joueur == 1) ? 'X' : 'O';
    return true;
}

int Morpion::verifier_victoire() const
{
    // Vérifier les lignes
    for (int i = 0; i < 3; i++)
    {
        if (grille[i][0] != ' ' && grille[i][0] == grille[i][1] && grille[i][1] == grille[i][2])
        {
            return (grille[i][0] == 'X') ? 1 : 2;
        }
    }

    // Vérifier les colonnes
    for (int j = 0; j < 3; j++)
    {
        if (grille[0][j] != ' ' && grille[0][j] == grille[1][j] && grille[1][j] == grille[2][j])
        {
            return (grille[0][j] == 'X') ? 1 : 2;
        }
    }

    // Vérifier les diagonales
    if (grille[0][0] != ' ' && grille[0][0] == grille[1][1] && grille[1][1] == grille[2][2])
    {
        return (grille[0][0] == 'X') ? 1 : 2;
    }
    if (grille[0][2] != ' ' && grille[0][2] == grille[1][1] && grille[1][1] == grille[2][0])
    {
        return (grille[0][2] == 'X') ? 1 : 2;
    }

    // Vérifier s'il y a encore des coups possibles
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (grille[i][j] == ' ')
            {
                return 0; // Le jeu continue
            }
        }
    }

    return 3; // Match nul
}