#ifndef CONVERSION_ELO_H
#define CONVERSION_ELO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../include/decoupe_tout_couleur.h"


/* Fonction qui prend en entrée un pixel RGB et qui renvoie la valeur de la luminance Y */
int16_t conversion_Y(struct Pixel_RGB pixelRGB);


/* Fonction qui prend en entrée un pixel RGB et qui renvoie la valeur de la chrominance Cb */
int16_t conversion_Cb(struct Pixel_RGB pixelRGB);


/* Fonction qui prend en entrée un pixel RGB et qui renvoie la valeur de la luminance Cr */
int16_t conversion_Cr(struct Pixel_RGB pixelRGB);


/* Structure d'un bloc après conversion YCbCr */
struct Bloc_YCbCr {
    int16_t **pixels;
};


/* Structure d'une MCU_YCbCr */
struct MCU_YCbCr {
    uint8_t largeur;
    uint8_t hauteur;
    uint8_t h1;
    uint8_t v1;
    uint8_t h2;
    uint8_t v2;
    uint8_t h3;
    uint8_t v3;
    struct Bloc_YCbCr **blocs_Y;
    struct Bloc_YCbCr **blocs_Cb;
    struct Bloc_YCbCr **blocs_Cr;
};


/* Fonction qui prend en entrée une MCU qui a pour arguments :
 * - largeur de la MCU
 * - hauteur de la MCU
 * - matrice de pixels RGB
 * - matrice de blocs de matrices de pixels RGB
 * Elle retourne une MCU qui a pour arguments :
 * - largeur de la MCU
 * - hauteur de la MCU
 * - matrice de blocs de matrices de pixels Y
 * - matrice de blocs de matrices de pixels Cb
 * - matrice de blocs de matrices de pixels Cr */
struct MCU_YCbCr *conversion_MCU(struct MCU_RGB *MCU);


/* Fonction qui fait la conversion d'une matrice de MCU_RGB et une matrice de MCU_YCbCr */
struct MCU_YCbCr ***conversion_matrice_MCUs(struct MCU_RGB ***MCU_a_convertir,
                                            uint32_t nb_MCUs_largeur,
                                            uint32_t nb_MCUs_hauteur);



/* Affiche un bloc d'entiers */
void print_bloc_entiers(struct Bloc_YCbCr *bloc_a_afficher);


/* Affiche une MCU_YCbCr */
void print_MCU_YCbCr(struct MCU_YCbCr *MCU_YCbCr_a_afficher);


/* Affiche la matrice de MCU_YCbCr */
void print_matrice_MCU_YCbCr(struct MCU_YCbCr ***MCUs_YCbCr_a_afficher,
                             uint32_t nb_MCUs_largeur,
                             uint32_t nb_MCUs_hauteur);


/* Libère la mémoire allouée par une matrice d'entiers */
void free_pixels_YCbCr(int16_t **pixels);


/* Libère la mémoire allouée par une matrice de blocs YCbCr */
void free_bloc_YCbCR(struct Bloc_YCbCr **blocs,
                     uint8_t largeur_MCU,
                     uint8_t hauteur_MCU);


/* Libère la mémoire allouée par une matrice de MCUs YCbCr */
void free_MCUs_YCbCr(struct MCU_YCbCr ***matrice_MCUs_converti,
                     uint32_t *dimensions_MCUs,
                     uint8_t largeur_MCU,
                     uint8_t hauteur_MCU);

#endif /* DECOUPE_TOUT_H */
