#ifndef SOUS_ECHANTILLONAGE_H
#define SOUS_ECHANTILLONAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "../include/conversion_YCbCr.h"


/* On libère un bloc de pixels */
void free_bloc(struct Bloc_YCbCr bloc);


/* On libère un tableau de blocs */
void free_blocs(struct Bloc_YCbCr **blocs, uint8_t vi, uint8_t hi);


/* On libère une MCU_YCbCr */
void free_MCU(struct MCU_YCbCr *MCU_YCbCr);


/* Libère la mémoire allouée par une matrice de MCUs YCbCr */
void free_MCUs_YCbCr_val(struct MCU_YCbCr ***matrice_MCUs_sous_ech,
                         uint32_t *dimensions_MCUs);



/*******************************************/
/* Partie consacrée au sous-echantillonage */
/*******************************************/


/* Différentes fonctions permettant de faire la moyenne
  d'un certain nombre de pixels de manière horizontale
  ou verticale, sur la chrominance Cb ou Cr*/


/* Moyenne horizontale sur Cb */
int16_t moyenne_horizontale_Cb(struct MCU_YCbCr *MCU_YCbCr,
                                    uint8_t hauteur_pix,
                                    uint8_t largeur_pix,
                                    uint8_t hauteur,
                                    uint8_t largeur);


/* Moyenne horizontale sur Cr */
int16_t moyenne_horizontale_Cr(struct MCU_YCbCr *MCU_YCbCr,
                                    uint8_t hauteur_pix,
                                    uint8_t largeur_pix,
                                    uint8_t hauteur,
                                    uint8_t largeur);


/* Moyenne verticale sur Cb */
int16_t moyenne_verticale_Cb(struct MCU_YCbCr *MCU_YCbCr,
                                    uint8_t hauteur_pix,
                                    uint8_t largeur_pix,
                                    uint8_t hauteur,
                                    uint8_t largeur);


/* Moyenne verticale sur Cr */
int16_t moyenne_verticale_Cr(struct MCU_YCbCr *MCU_YCbCr,
                                    uint8_t hauteur_pix,
                                    uint8_t largeur_pix,
                                    uint8_t hauteur,
                                    uint8_t largeur);


/* Fonction permettant de sous_echantilloner horizontalement une MCU_YCbCr */
struct MCU_YCbCr *sous_echantillonage_horizontal(struct MCU_YCbCr *MCU_YCbCr);


/* Fonction permettant de sous_echantilloner verticalement une MCU_YCbCr */
struct MCU_YCbCr *sous_echantillonage_vertical(struct MCU_YCbCr *MCU_YCbCr);


struct MCU_YCbCr *sous_echantillonage(struct MCU_YCbCr *MCU_YCbCr);


struct MCU_YCbCr ***sous_echantillone(struct MCU_YCbCr ***MCU_a_sous_ech,
                                      uint32_t nb_MCUs_largeur,
                                      uint32_t nb_MCUs_hauteur);


void print_bloc(struct Bloc_YCbCr bloc);


void print_MCU_YCbCr_val(struct MCU_YCbCr *MCU_YCbCr);


/* Affiche la matrice de MCU_YCbCr */
void print_matrice_MCU_YCbCr_val(struct MCU_YCbCr ***MCUs_YCbCr_a_afficher,
                             uint32_t nb_MCUs_largeur,
                             uint32_t nb_MCUs_hauteur);



#endif /* SOUS_ECHANTILLONAGE_H */
