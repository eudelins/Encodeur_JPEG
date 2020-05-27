#ifndef CONVERSION_YCbCr_H
#define CONVERSION_YCbCr_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../include/decoupe_couleur.h"


//***************************************************/
/* Module de conversion des MCUs RGB en MCUs YCbCr */
/***************************************************/


/* Renvoie la valeur de la luminance Y pour un pixel RGB */
int16_t conversion_Y(struct Pixel_RGB pixelRGB);


/* Renvoie la valeur de la chrominance Cb pour un pixel RGB */
int16_t conversion_Cb(struct Pixel_RGB pixelRGB);


/* Renvoie la valeur de la chrominance Cr pour un pixel RGB */
int16_t conversion_Cr(struct Pixel_RGB pixelRGB);


/* Structure d'un bloc après conversion en YCbCr */
struct Bloc_YCbCr {
    int16_t **pixels;
};


/* Structure d'une MCU après conversion en YCbCr */
struct MCU_YCbCr {
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


/* Fonction qui prend en entrée une MCU_RGB et qui renvoie la MCU convertie en YCbCr (soit de structure MCU_YCbCr) */
struct MCU_YCbCr *conversion_MCU(struct MCU_RGB *MCU);


/* Fonction qui fait la conversion d'une matrice de MCU_RGB en une matrice de MCU_YCbCr */
struct MCU_YCbCr ***conversion_matrice_MCUs(struct MCU_RGB ***MCU_a_convertir,
                                            uint32_t nb_MCUs_largeur,
                                            uint32_t nb_MCUs_hauteur);


/* Affiche un Bloc_YCbCr (qui est un bloc d'entiers) */
void print_bloc_entiers(struct Bloc_YCbCr *bloc_a_afficher);


/* Affiche une MCU_YCbCr */
void print_MCU_YCbCr(struct MCU_YCbCr *MCU_YCbCr_a_afficher);


/* Affiche toutes les MCU_YCbCr (soit la matrice de MCUs_YCbCr) */
void print_matrice_MCU_YCbCr(struct MCU_YCbCr ***MCUs_YCbCr_a_afficher,
                             uint32_t nb_MCUs_largeur,
                             uint32_t nb_MCUs_hauteur);


/* Libère la mémoire allouée par une matrice de pixels (qui sont des entiers) */
void free_pixels_YCbCr(int16_t **pixels);


/* Libère la mémoire allouée par une matrice de blocs YCbCr */
void free_bloc_YCbCR(struct Bloc_YCbCr **blocs,
                     uint8_t h,
                     uint8_t v);


/* Libère la mémoire allouée par une matrice de MCUs YCbCr */
void free_MCUs_YCbCr(struct MCU_YCbCr ***matrice_MCUs_converti,
                     uint32_t *dimensions_MCUs);

#endif /* CONVERSION_YCbCr_H */

