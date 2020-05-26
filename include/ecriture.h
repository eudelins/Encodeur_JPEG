#ifndef ECRITURE_H
#define ECRITURE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bitstream.h"
#include "huffman.h"


/* Calcule x puissance y */
uint16_t puissance(uint8_t x, uint8_t y);


/* Calcul la magnitude de pixel */
uint8_t calcule_magnitude(int16_t pixel);


/* Calcul l'indice de pixel qui est de magnitude magnitude */
uint16_t calcule_index(int16_t pixel, uint8_t magnitude);


/* Ecrit dans le bitstream les bits issus du codage DC d'une MCU */
void codage_DC_MCU(struct MCU_zigzag *MCU_zigzag,
                   struct huff_table *table_huff_DC_Y,
                   int16_t *DC_prec,
                   struct bitstream *test_stream);


/* Ecrit dans le bitstream les bits issus du codage AC d'une MCU */
void codage_AC_MCU(struct MCU_zigzag *MCU_zigzag,
                   struct huff_table *table_huff_AC_Y,
                   struct bitstream *test_stream);


/* Affiche le codage AC/DC d'une MCU */
void codage_AC_DC_MCU(struct MCU_zigzag *MCU_zigzag,
                      struct huff_table *table_huff_DC_Y,
                      struct huff_table *table_huff_AC_Y,
                      int16_t *DC_prec,
                      struct bitstream *test_stream);


/* Renvoie le chemin jpg à partir du chemin pgm */
char *cree_chemin_jpg(char *chemin);


void cree_image(struct MCU_zigzag ***MCUs_zigzag, 
                uint32_t largeur_MCUs, 
                uint32_t hauteur_MCUs,
                uint32_t largeur_image, 
                uint32_t hauteur_image,
                char *chemin,
                char *chemin_jpg);



#endif /* ECRITURE_H */