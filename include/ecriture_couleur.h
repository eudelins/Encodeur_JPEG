#ifndef ECRITURE_COULEUR_H
#define ECRITURE_COULEUR_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../include/decoupe_tout_couleur.h"
#include "../include/conversion_elo.h"
#include "../include/simon_encodage.h"
#include "../include/htables.h"
#include "../include/huffman.h"
#include "../include/bitstream.h"
#include "../include/jpeg_writer.h"
#include "../include/ecriture.h"


/* Ecrit dans le bitstream les bits issus du codage DC d'un bloc */
void codage_DC_bloc(struct Bloc_zigzag Bloc,
                   struct huff_table *table_huff_DC,
                   int16_t *DC_prec,
                   struct bitstream *test_stream);


/* Ecrit dans le bitstream les bits issus du codage AC d'un bloc */
void codage_AC_bloc(struct Bloc_zigzag Bloc,
                   struct huff_table *table_huff_AC,
                   struct bitstream *test_stream);



/* Ecrit dans le bitstream les bits issus du codage AC/DC d'une MCU */
void codage_MCU_couleur(struct MCU_zigzag_Y *MCU_zigzag,
                        struct huff_table *table_huff_DC_Y,
                        struct huff_table *table_huff_DC_Cb,
                        struct huff_table *table_huff_DC_Cr,
                        struct huff_table *table_huff_AC_Y,
                        struct huff_table *table_huff_AC_Cb,
                        struct huff_table *table_huff_AC_Cr,
                        int16_t *DC_prec_Y,
                        int16_t *DC_prec_Cb,
                        int16_t *DC_prec_Cr,
                        struct bitstream *test_stream);


void cree_image_couleur(struct MCU_zigzag_Y ***MCUs_zigzag, 
                        uint32_t largeur_MCUs, 
                        uint32_t hauteur_MCUs,
                        uint32_t largeur_image, 
                        uint32_t hauteur_image,
                        char *chemin,
                        char *chemin_jpg);


#endif /* ECRITURE_COULEUR_H */