#ifndef SOUS_ECHANTILLONNAGE_H
#define SOUS_ECHANTILLONNAGE_H

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
/* Partie consacrée au sous-échantillonnage */
/*******************************************/


/* Fait la moyenne sur un certain nombre de pixels Cb alignés horizontalement */
int16_t moyenne_horizontale_Cb(struct MCU_YCbCr *MCU_YCbCr,
                                    uint8_t hauteur_pix,
                                    uint8_t largeur_pix,
                                    uint8_t hauteur,
                                    uint8_t largeur);


/* Fait la moyenne sur un certain nombre de pixels Cr alignés horizontalement */
int16_t moyenne_horizontale_Cr(struct MCU_YCbCr *MCU_YCbCr,
                                    uint8_t hauteur_pix,
                                    uint8_t largeur_pix,
                                    uint8_t hauteur,
                                    uint8_t largeur);


/* Fait la moyenne sur un certain nombre de pixels Cb alignés verticalement */
int16_t moyenne_verticale_Cb(struct MCU_YCbCr *MCU_YCbCr,
                                    uint8_t hauteur_pix,
                                    uint8_t largeur_pix,
                                    uint8_t hauteur,
                                    uint8_t largeur);


/* Fait la moyenne sur un certain nombre de pixels Cr alignés verticalement */
int16_t moyenne_verticale_Cr(struct MCU_YCbCr *MCU_YCbCr,
                                    uint8_t hauteur_pix,
                                    uint8_t largeur_pix,
                                    uint8_t hauteur,
                                    uint8_t largeur);


/* Sous-échantillonne horizontalement une MCU_YCbCr */
struct MCU_YCbCr *sous_echantillonnage_horizontal(struct MCU_YCbCr *MCU_YCbCr);


/* Sous-échantillonne verticalement une MCU_YCbCr */
struct MCU_YCbCr *sous_echantillonnage_vertical(struct MCU_YCbCr *MCU_YCbCr);


/* Sous-échantillonne une MCU en supposant que tous les
paramètres vérifient les conditions sur les hi et vi */
struct MCU_YCbCr *sous_echantillonnage(struct MCU_YCbCr *MCU_YCbCr);


/* Sous-échantillonne une matrice de MCU_YCbCr, on suppose ici aussi
 que tous les paramètres vérifient les conditions sur les hi et vi */
struct MCU_YCbCr ***sous_echantillonne(struct MCU_YCbCr ***MCU_a_sous_ech,
                                      uint32_t nb_MCUs_largeur,
                                      uint32_t nb_MCUs_hauteur);


/* Affiche un bloc */
void print_bloc(struct Bloc_YCbCr bloc);


/* Affiche une MCU_YCbCr */
void print_MCU_YCbCr_val(struct MCU_YCbCr *MCU_YCbCr);


/* Affiche la matrice de MCU_YCbCr */
void print_matrice_MCU_YCbCr_val(struct MCU_YCbCr ***MCUs_YCbCr_a_afficher,
                             uint32_t nb_MCUs_largeur,
                             uint32_t nb_MCUs_hauteur);



#endif /* SOUS_ECHANTILLONAGE_H */
