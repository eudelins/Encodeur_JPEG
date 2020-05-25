#ifndef RECUP_PARAS_H
#define RECUP_PARAS_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


/* Prend un fichier ppm en argument et renvoie l'en-tête de ce fichier */
uint32_t *paras(FILE *fichier);


/* Récupère le nom de l'image à encoder en ligne de commande et renvoie le chemin d'accès en chaine de caractères */
char *chemin_fichier(char *nom_fichier);


/* Texte à afficher si on a un --help en argument */
void affichage_help();


/* Texte à afficher si on a une erreur d'argument */
void affichage_erreur();


/* Récupère les paramètes optionnels
 * Renvoie "h" si help, "o" si outfile, "s" si sample, "e" si erreur*/
char *paras_optionnels(uint8_t argc, char **argv);


/* Vérifie que la valeur de chaque facteur h ou v être comprise entre 1 et 4 : renvoie true si ok, false sinon
 * Fonction à utiliser avant somme_produits_valeurs et diviser_valeurs car on vérifie ici que les valeur h et v sont bien des uint8_t */
bool encadrement_valeurs(uint32_t h1,
                         uint32_t v1,
                         uint32_t h2,
                         uint32_t v2,
                         uint32_t h3,
                         uint32_t v3);


/* Vérifie que la somme des produits hi x vi est inférieure à 10 : renvoie true si ok, false sinon */
bool somme_produits_valeurs(uint8_t h1,
                            uint8_t v1,
                            uint8_t h2,
                            uint8_t v2,
                            uint8_t h3,
                            uint8_t v3);


/* Vérifie que les facteurs d'échantillonnage des chrominances divisent ceux de la luminance : renvoie true si ok, false sinon */
bool diviser_valeurs(uint8_t h1,
                     uint8_t v1,
                     uint8_t h2,
                     uint8_t v2,
                     uint8_t h3,
                     uint8_t v3);


/* Vérifie que toutes les conditions sur h et v sont vérifiées : renvoie true si ok, false sinon */
bool verif_conditions(uint32_t h1,
                      uint32_t v1,
                      uint32_t h2,
                      uint32_t v2,
                      uint32_t h3,
                      uint32_t v3);


/* Récupère les éléments de chaine à partir de indice_recup */
char *recup_nom(char *chaine, uint8_t indice_recup)


#endif /* RECUP_PARAS_H */
