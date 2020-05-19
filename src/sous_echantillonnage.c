#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


/* Vérifie que la valeur de chaque facteur h ou v être comprise entre 1 et 4 : renvoie true si ok, false sinon
 * Fonction à utiliser avant somme_produits_valeurs et diviser_valeurs car on vérifie ici que les valeur h et v sont bien des uint8_t */

bool encadrement_valeurs(uint32_t h1,
                         uint32_t v1,
                         uint32_t h2,
                         uint32_t v2,
                         uint32_t h3,
                         uint32_t v3)
{
    bool encadrement = true;

    if (h1 < 1 || h1 > 4) {
        encadrement = false;
    }
    else if (v1 < 1 || v1 > 4) {
        encadrement = false;
    }
    else if (h2 < 1 || h2 > 4) {
        encadrement = false;
    }
    else if (v2 < 1 || v2 > 4) {
        encadrement = false;
    }
    else if (h3 < 1 || h3 > 4) {
        encadrement = false;
    }
    else if (v3 < 1 || v3 > 4) {
        encadrement = false;
    }

    return encadrement;
}


/* Vérifie que la somme des produits hi x vi est inférieure à 10 : renvoie true si ok, false sinon */

bool somme_produits_valeurs(uint8_t h1,
                            uint8_t v1,
                            uint8_t h2,
                            uint8_t v2,
                            uint8_t h3,
                            uint8_t v3)
{
    bool somme_produits = true;

    if (h1 * v1 + h2 * v2 + h3 * v3 > 10) {
        somme_produits = false;
    }

    return somme_produits;
}


/* Vérifie que les facteurs d'échantillonnage des chrominances divisent ceux de la luminance : renvoie true si ok, false sinon */

bool diviser_valeurs(uint8_t h1,
                     uint8_t v1,
                     uint8_t h2,
                     uint8_t v2,
                     uint8_t h3,
                     uint8_t v3)
{
    bool division = true;

    if (h1 % h2 != 0 || h1 % h3 != 0) {
        division = false;
    }
    else if (v1 % v2 != 0 || v1 % v3 != 0) {
        division = false;
    }

    return division;
}


/* Vérifie que toutes les conditions sur h et v sont vérifiées : renvoie true si ok, false sinon */
bool verif_conditions(uint32_t h1,
                      uint32_t v1,
                      uint32_t h2,
                      uint32_t v2,
                      uint32_t h3,
                      uint32_t v3)
{
    bool conditions = true;

    bool encadrement = encadrement_valeurs(h1, v1, h2, v2, h3, v3);
    if (encadrement == false) {
        printf("Pas le bon encadrement\n");
        conditions = false;
    }
    else {
        bool somme_produits = somme_produits_valeurs(h1, v1, h2, v2, h3, v3);
        if (somme_produits == false) {
            printf("La somme des produits est supérieure à 10\n");
            conditions = false;
        }
        else {
            bool division = diviser_valeurs(h1, v1, h2, v2, h3, v3);
            if (division == false) {
                printf("Les facteurs d'échantillonnage des chrominances ne divisent pas ceux de la luminance\n");
                conditions = false;
            }
            else {
                printf("Tout est ok\n");
            }
        }
    }

    return conditions;
}



int main()
{
    uint8_t h1 = 4;
    uint8_t v1 = 2;
    uint8_t h2 = 1;
    uint8_t v2 = 2;
    uint8_t h3 = 1;
    uint8_t v3 = 1;

    bool conditions = verif_conditions(h1, v1, h2, v2, h3, v3);
    return 0;
}
