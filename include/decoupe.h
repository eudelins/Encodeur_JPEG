#ifndef DECOUPE_H
#define DECOUPE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


/* 
    Défini une MCU, la largeur et la hauteur d'une MCU sont toujours 
    égales à 8 en niveau de gris.
*/
struct MCU {
    uint8_t largeur;
    uint8_t hauteur;
    uint8_t **pixels;
};


/* Calcule le nombre de MCUs dans l'image */
uint32_t *calcul_dimensions_MCUs(uint32_t largeur_image, uint32_t hauteur_image,
                                 uint8_t largeur_MCU, uint8_t hauteur_MCU);


/* Découpe l'image en une matrice de MCU */
struct MCU ***decoupage(FILE *fichier, 
                        uint32_t largeur_MCUs,
                        uint32_t hauteur_MCUs,
                        uint32_t largeur_image,
                        uint32_t hauteur_image);


/* Libère la mémoire alloué pour les pixels d'une MCU */
void free_pixel(uint8_t **pixels);


/* Libère la mémoire allouée à la matrice de MCU */
void free_MCUs(struct MCU ***MCUs, uint32_t* dimensions_MCUs);


/* Affiche une MCU */
void print_MCU(struct MCU *MCU);


/* Affiche la matrice de MCU */
void print_MCUs(struct MCU ***MCUs, uint32_t *dimensions_MCUs);


/* Ouvre le fichier filename avec le mode d'accès mode. Retourne le FILE *
 * correspondant. */
FILE *ouvrir_fichier(const char *filename, const char *mode);


/* Ferme le fichier passé en paramètre. */
void fermer_fichier(FILE *fichier);


#endif /* DECOUPE_H */
