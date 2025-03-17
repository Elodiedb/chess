#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <ctime>
#include "morpion.hpp"
#include "minmax_morpion.hpp"

using namespace std;
/*
int main()
{
    Morpion jeu;
    int joueur = 1; // 1 pour le joueur humain, 2 pour l'ordinateur
    int coup;
    int resultat = 0;

    while (resultat == 0)
    {
        jeu.afficher_grille();
        if (joueur == 1)
        {
            cout << "Joueur " << joueur << ", entrez votre coup (0-8) : ";
            cin >> coup;
            int ligne = coup / 3;
            int colonne = coup % 3;
            if (jeu.jouer(joueur, ligne, colonne))
            {
                resultat = jeu.verifier_victoire();
                joueur = 2; // Passe le tour à l'ordinateur
            }
            else
            {
                cout << "Coup invalide. Essayez à nouveau.\n";
            }
        }
        else
        {
            cout << "Tour de l'ordinateur...\n";

            // Augmenter la profondeur à 9 pour une analyse complète
            int profondeur_max = 9;

            cout << "Recherche avec profondeur " << profondeur_max << " (résolution optimale)" << endl;
            Position_morpion pos(jeu, false, nullptr, nullptr, 0);

            // Élargir également les limites alpha/beta pour éviter des restrictions
            meilleur_coup_morpion meilleur_coup = alpha_beta_morpion(pos, profondeur_max, -10000, 10000, 0);

            cout << "Meilleur coup trouvé: " << meilleur_coup.coup << " (évaluation: " << meilleur_coup.evaluation << ")" << endl;

            // Vérifier que le coup est valide
            if (meilleur_coup.coup < 0 || meilleur_coup.coup > 8)
            {
                cout << "ERREUR: Coup invalide " << meilleur_coup.coup << endl;
                // Trouver un coup valide de secours
                for (int i = 0; i < 3; i++)
                {
                    for (int j = 0; j < 3; j++)
                    {
                        if (jeu.grille[i][j] == ' ')
                        {
                            coup = i * 3 + j;
                            goto coup_trouve;
                        }
                    }
                }
            coup_trouve:
                cout << "Utilisation du coup de secours: " << coup << endl;
            }
            else
            {
                coup = meilleur_coup.coup;
            }

            int ligne = coup / 3;
            int colonne = coup % 3;

            cout << "L'ordinateur joue en position: " << coup << " (ligne=" << ligne << ", colonne=" << colonne << ")\n";

            jeu.jouer(joueur, ligne, colonne);
            resultat = jeu.verifier_victoire();
            joueur = 1; // Passe le tour au joueur humain
        }
    }

    jeu.afficher_grille();
    if (resultat == 1)
    {
        cout << "Le joueur 1 (X) a gagné !\n";
    }
    else if (resultat == 2)
    {
        cout << "Le joueur 2 (O) a gagné !\n";
    }
    else
    {
        cout << "Match nul !\n";
    }

    return 0;
}
*/
// ORDINATEUR VS ORDINATEUR

// Fonction pour générer un coup aléatoire valide

int coup_aleatoire(const Morpion &jeu)
{
    // Créer une liste des coups disponibles
    vector<int> coups_disponibles;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (jeu.grille[i][j] == ' ')
            {
                coups_disponibles.push_back(i * 3 + j);
            }
        }
    }

    // Générer un index aléatoire
    static mt19937 rng(static_cast<unsigned int>(time(nullptr)));
    uniform_int_distribution<int> distribution(0, coups_disponibles.size() - 1);
    int index = distribution(rng);

    return coups_disponibles[index];
}

// Fonction qui simule une partie et retourne le résultat
int jouer_une_partie(bool afficher_details = false, bool premier_coup_aleatoire = false, bool deuxieme_coup_aleatoire = true)
{
    Morpion jeu;
    int joueur = 1; // 1 pour X, 2 pour O
    int coup;
    int resultat = 0;
    int tour = 0;

    while (resultat == 0)
    {
        tour++;
        if (afficher_details)
        {
            cout << "\n=== TOUR " << tour << " ===" << endl;
            jeu.afficher_grille();

            // Petit délai pour pouvoir suivre le jeu
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            cout << "Ordinateur " << joueur << " (" << (joueur == 1 ? "X" : "O") << ") réfléchit..." << endl;
        }

        // Premier coup aléatoire pour le joueur 1 au tour 1
        if (premier_coup_aleatoire && tour == 1 && joueur == 1)
        {
            coup = coup_aleatoire(jeu);
            if (afficher_details)
            {
                cout << "Premier coup aléatoire (X): " << coup << endl;
            }
        }
        // Modifier la condition pour le deuxième coup aléatoire
        else if (deuxieme_coup_aleatoire && tour == 2 && joueur == 2)
        {
            coup = coup_aleatoire(jeu);
            if (afficher_details)
            {
                cout << "Premier coup aléatoire pour O: " << coup << endl;
            }
        }
        else
        {
            // Profondeur maximale pour exploration complète
            int profondeur_max = 9;

            // Créer la position à analyser
            Position_morpion pos(jeu, joueur == 1, nullptr, nullptr, 0);

            // Limites alpha/beta extrêmes pour éviter les restrictions
            meilleur_coup_morpion meilleur_coup = alpha_beta_morpion(pos, profondeur_max, -10000, 10000, 0);

            if (afficher_details)
            {
                cout << "Meilleur coup trouvé: " << meilleur_coup.coup
                     << " (évaluation: " << meilleur_coup.evaluation << ")" << endl;
            }

            // Vérifier que le coup est valide
            if (meilleur_coup.coup < 0 || meilleur_coup.coup > 8)
            {
                if (afficher_details)
                    cout << "ERREUR: Coup invalide " << meilleur_coup.coup << endl;

                // Trouver un coup valide de secours
                for (int i = 0; i < 3; i++)
                {
                    for (int j = 0; j < 3; j++)
                    {
                        if (jeu.grille[i][j] == ' ')
                        {
                            coup = i * 3 + j;
                            goto coup_trouve;
                        }
                    }
                }
            coup_trouve:
                if (afficher_details)
                    cout << "Utilisation du coup de secours: " << coup << endl;
            }
            else
            {
                coup = meilleur_coup.coup;
            }
        }

        int ligne = coup / 3;
        int colonne = coup % 3;

        if (afficher_details)
        {
            cout << "Ordinateur " << joueur << " joue en position: " << coup
                 << " (ligne=" << ligne << ", colonne=" << colonne << ")" << endl;
        }

        jeu.jouer(joueur, ligne, colonne);
        resultat = jeu.verifier_victoire();

        // Changer de joueur
        joueur = (joueur == 1) ? 2 : 1;
    }

    if (afficher_details)
    {
        cout << "\n=== FIN DE LA PARTIE ===" << endl;
        jeu.afficher_grille();

        if (resultat == 1)
            cout << "L'ordinateur 1 (X) a gagné!" << endl;
        else if (resultat == 2)
            cout << "L'ordinateur 2 (O) a gagné!" << endl;
        else
            cout << "Match nul!" << endl;
    }

    return resultat;
}

int main()
{
    const int NB_PARTIES = 100;
    int victoires_X = 0;
    int victoires_O = 0;
    int matchs_nuls = 0;

    cout << "=== SIMULATION DE " << NB_PARTIES << " PARTIES DE MORPION ===" << endl;
    cout << "Le premier coup de O (deuxième coup de la partie) sera aléatoire" << endl;
    cout << "Appuyez sur Enter pour commencer...";
    cin.get();

    cout << "Simulation en cours..." << endl;

    // Jouer les parties avec deuxième coup aléatoire
    for (int partie = 1; partie <= NB_PARTIES; partie++)
    {
        cout << "Partie " << partie << "/" << NB_PARTIES << "..." << endl;

        int resultat = jouer_une_partie(false, false, true); // détails=false, premier_coup=normal, deuxième_coup=aléatoire

        // Compter les résultats
        if (resultat == 1)
            victoires_X++;
        else if (resultat == 2)
            victoires_O++;
        else
            matchs_nuls++;
    }

    cout << "\n=== RÉSULTATS DES " << NB_PARTIES << " PARTIES AVEC DEUXIÈME COUP ALÉATOIRE ===" << endl;
    cout << "Victoires X (joueur 1): " << victoires_X << " (" << (victoires_X * 100.0 / NB_PARTIES) << "%)" << endl;
    cout << "Victoires O (joueur 2): " << victoires_O << " (" << (victoires_O * 100.0 / NB_PARTIES) << "%)" << endl;
    cout << "Matchs nuls: " << matchs_nuls << " (" << (matchs_nuls * 100.0 / NB_PARTIES) << "%)" << endl;

    if (victoires_X > 0 || victoires_O > 0)
    {
        cout << "\nCONCLUSION: Avec un deuxième coup aléatoire, certaines parties peuvent être gagnées!" << endl;
        if (victoires_X > victoires_O)
            cout << "Le joueur X peut capitaliser sur un mauvais coup de O." << endl;
        else if (victoires_O > victoires_X)
            cout << "Le joueur O peut gagner malgré un premier coup sous-optimal." << endl;
    }
    else
    {
        cout << "\nCONCLUSION: Même avec un deuxième coup aléatoire, l'algorithme joue parfaitement ensuite!" << endl;
    }

    // Demander à l'utilisateur s'il veut voir une partie détaillée
    cout << "\nVoulez-vous voir une partie détaillée avec deuxième coup aléatoire? (o/n): ";
    char reponse;
    cin >> reponse;
    if (reponse == 'o' || reponse == 'O')
    {
        cout << "\nDémonstration d'une partie détaillée:" << endl;
        jouer_une_partie(true, false, true); // détails=true, premier_coup=normal, deuxième_coup=aléatoire
    }

    return 0;
}
