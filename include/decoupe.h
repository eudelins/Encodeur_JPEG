#ifndef DECOUPE_H
#define DECOUPE_H

#include <stdint.h>


/* Ouvre le fichier filename avec le mode d'accès mode. Retourne le FILE *
 * correspondant. */
FILE *ouvrir_fichier(const char *filename, const char *mode);


/* Ferme le fichier passé en paramètre. */
void fermer_fichier(FILE *fichier);


struct MCU {
    uint8_t largeur;
    uint8_t hauteur;
    uint8_t **pixels;
};


/* Calcule le nombre de MCUs dans l'image */
uint32_t *calcul_dimensions_MCUs(uint32_t largeur_image, uint32_t hauteur_image,
                                 uint8_t largeur_MCU, uint8_t hauteur_MCU);


/* Découpe l'image en MCUs */
struct MCU ***decoupage(FILE *fichier, uint8_t largeur_MCU, uint8_t hauteur_MCU);


/* Libère la mémoire alloué pour les pixels d'une MCU */
void free_pixel(uint8_t **pixels);


/* Libère la mémoire allouée aux MCUs */
void free_MCUs(struct MCU ***MCUs, uint32_t* dimensions_MCUs);


/* Affiche une MCU */
void print_MCU(struct MCU *MCU);


/* Affiche les MCUs */
void print_MCUs(struct MCU ***MCUs, uint32_t *dimensions_MCUs);


#endif /* DECOUPE_H */