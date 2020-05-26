#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../include/decoupe_couleur.h"
#include "../include/conversion_YCbCr.h"
#include "../include/encodage_couleur.h"
#include "../include/htables.h"
#include "../include/qtables.h"
#include "../include/huffman.h"
#include "../include/bitstream.h"
#include "../include/jpeg_writer.h"
#include "../include/ecriture.h"

   
/* Ecrit dans le bitstream les bits issus du codage DC d'un bloc */
void codage_DC_bloc(struct Bloc_zigzag Bloc,
                   struct huff_table *table_huff_DC,
                   int16_t *DC_prec,
                   struct bitstream *test_stream)
{
    uint8_t magnitude;
    uint16_t index;
    uint32_t codage_huff;
    uint8_t *nb_bits = malloc(sizeof(uint8_t));
    int16_t *pixels = Bloc.pixels;
    
    magnitude = calcule_magnitude(pixels[0] - *DC_prec);
    index = calcule_index(pixels[0] - *DC_prec, magnitude);
    codage_huff = huffman_table_get_path(table_huff_DC, magnitude, nb_bits);

    // printf("DC:\n\tvalue: %d\t", pixels[0] - *DC_prec);
    // printf("magnitude: %d\t", magnitude);
    // printf("index: %d à écrire sur %d bits\n", index, magnitude);
    // printf("\tHuffman: %d à écrire sur %d bits\n\n", codage_huff, *nb_bits);

    // On écrit dans le bistream
    bitstream_write_bits(test_stream, codage_huff, *nb_bits, false);
    bitstream_write_bits(test_stream, index, magnitude, false);
    
    free(nb_bits);
    *DC_prec = pixels[0];
}


/* Ecrit dans le bitstream les bits issus du codage AC d'un bloc */
void codage_AC_bloc(struct Bloc_zigzag Bloc,
                   struct huff_table *table_huff_AC,
                   struct bitstream *test_stream)
{
    uint8_t RLE_code, magnitude;
    uint16_t index;
    uint32_t codage_huff;
    uint8_t *nb_bits = malloc(sizeof(uint8_t));
    int16_t *pixels = Bloc.pixels;

    // printf("AC:\n");
    uint8_t compteur_0 = 0;
    uint8_t compteur_16_0 = 0;
    for(uint8_t indice = 1; indice < 64; indice++){
        if (pixels[indice] == 0 && compteur_0 < 15) {
            compteur_0 += 1;
        } else if (pixels[indice] == 0 && compteur_0 == 15) {
            // Cas des 16 0 consécutifs
            compteur_0 = 0;
            compteur_16_0 += 1;
        } else {
            
            // On écrit autant de fois OxF0 que nécessaire
            for (uint8_t i = 0; i < compteur_16_0; i++){
                RLE_code = 0xF0;
                codage_huff = huffman_table_get_path(table_huff_AC, RLE_code, nb_bits);
                // printf("\tcode RLE: %d\t", RLE_code);
                // printf("\tHuffman: %d à écrire sur %d bits\n", codage_huff, *nb_bits);
                // printf("\n");
                bitstream_write_bits(test_stream, codage_huff, *nb_bits, false);
            }
            compteur_16_0 = 0;

            // On décale le nombre de 0 de 4 bits vers la gauche (bits de poids forts)
            compteur_0 = compteur_0 << 4;

            // On calcule la magnitude du pixel
            magnitude = calcule_magnitude(pixels[indice]);

            // On calcule l'index
            index = calcule_index(pixels[indice], magnitude);

            RLE_code = compteur_0 + magnitude;
            codage_huff = huffman_table_get_path(table_huff_AC, RLE_code, nb_bits);

            // printf("\tvalue: %d\t", pixels[indice]);
            // printf("magnitude: %d\t", magnitude);
            // printf("index: %d à écrire sur %d bits\n", index, magnitude);
            // printf("\tcode RLE: %d\t", RLE_code);
            // printf("\tHuffman: %d à écrire sur %d bits\n", codage_huff, *nb_bits);
            // printf("\n");

            // On écrit dans le bitstream
            bitstream_write_bits(test_stream, codage_huff, *nb_bits, false);
            bitstream_write_bits(test_stream, index, magnitude, false);

            // On remet le compteur à 0
            compteur_0 = 0;
        }
    }
    
    // End of Block
    if (pixels[63] == 0){
        RLE_code = 0x00;
        codage_huff = huffman_table_get_path(table_huff_AC, RLE_code, nb_bits);
        // printf("\tvalue: endofblock");
        // printf("\tcode RLE: %d\t", RLE_code);
        // printf("Huffman: %d à écrire sur %d bits\n", codage_huff, *nb_bits);
        // printf("\n");
        bitstream_write_bits(test_stream, codage_huff, *nb_bits, false);
    }
 
    free(nb_bits);
}


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
                        struct bitstream *test_stream)
{
    // Codage des blocs Y
    for (uint8_t hauteur_Y = 0; hauteur_Y < MCU_zigzag->v1; hauteur_Y++) {
        for (uint8_t largeur_Y = 0; largeur_Y < MCU_zigzag->h1; largeur_Y++){
            codage_DC_bloc(MCU_zigzag->blocs_Y_zigzag[hauteur_Y][largeur_Y],
                           table_huff_DC_Y,
                           DC_prec_Y,
                           test_stream);
            codage_AC_bloc(MCU_zigzag->blocs_Y_zigzag[hauteur_Y][largeur_Y],
                           table_huff_AC_Y,
                           test_stream);
        }
    }

    // Quantification des blocs Cb
    for (uint8_t hauteur_Cb = 0; hauteur_Cb < MCU_zigzag->v2; hauteur_Cb++) {
        for (uint8_t largeur_Cb = 0; largeur_Cb < MCU_zigzag->h2; largeur_Cb++){
            codage_DC_bloc(MCU_zigzag->blocs_Cb_zigzag[hauteur_Cb][largeur_Cb],
                           table_huff_DC_Cb,
                           DC_prec_Cb,
                           test_stream);
            codage_AC_bloc(MCU_zigzag->blocs_Cb_zigzag[hauteur_Cb][largeur_Cb],
                           table_huff_AC_Cb,
                           test_stream);
        }
    }

    // Quantification des blocs Cr
    for (uint8_t hauteur_Cr = 0; hauteur_Cr < MCU_zigzag->v3; hauteur_Cr++) {
        for (uint8_t largeur_Cr = 0; largeur_Cr < MCU_zigzag->h3; largeur_Cr++){
            codage_DC_bloc(MCU_zigzag->blocs_Cr_zigzag[hauteur_Cr][largeur_Cr],
                           table_huff_DC_Cr,
                           DC_prec_Cr,
                           test_stream);
            codage_AC_bloc(MCU_zigzag->blocs_Cr_zigzag[hauteur_Cr][largeur_Cr],
                           table_huff_AC_Cr,
                           test_stream);
        }
    }
}



void cree_image_couleur(struct MCU_zigzag_Y ***MCUs_zigzag, 
                        uint32_t largeur_MCUs, 
                        uint32_t hauteur_MCUs,
                        uint32_t largeur_image, 
                        uint32_t hauteur_image,
                        char *chemin,
                        char *chemin_jpg)
{
    // Création de la table de Huffman pour coder les magnitudes des DC des bloc Y
    uint8_t *nb_symb_per_lengths_DC = htables_nb_symb_per_lengths[DC][Y];
    uint8_t *symbols_DC = htables_symbols[DC][Y];
    uint8_t nb_symbols_DC = htables_nb_symbols[DC][Y];
    struct huff_table *table_huff_DC_Y = huffman_table_build(nb_symb_per_lengths_DC, symbols_DC, nb_symbols_DC);
    
    // Création de la table de Huffman pour coder les AC des blocs Y
    uint8_t *nb_symb_per_lengths_AC = htables_nb_symb_per_lengths[AC][Y];
    uint8_t *symbols_AC = htables_symbols[AC][Y];
    uint8_t nb_symbols_AC = htables_nb_symbols[AC][Y];
    struct huff_table *table_huff_AC_Y = huffman_table_build(nb_symb_per_lengths_AC, symbols_AC, nb_symbols_AC);

    // Création de la table de Huffman pour coder les magnitudes des DC des blocs Cb
    nb_symb_per_lengths_DC = htables_nb_symb_per_lengths[DC][Cb];
    symbols_DC = htables_symbols[DC][Cb];
    nb_symbols_DC = htables_nb_symbols[DC][Cb];
    struct huff_table *table_huff_DC_Cb = huffman_table_build(nb_symb_per_lengths_DC, symbols_DC, nb_symbols_DC);
    
    // Création de la table de Huffman pour coder les AC des blocs Cb
    nb_symb_per_lengths_AC = htables_nb_symb_per_lengths[AC][Cb];
    symbols_AC = htables_symbols[AC][Cb];
    nb_symbols_AC = htables_nb_symbols[AC][Cb];
    struct huff_table *table_huff_AC_Cb = huffman_table_build(nb_symb_per_lengths_AC, symbols_AC, nb_symbols_AC);

    // Création de la table de Huffman pour coder les magnitudes des DC des blocs Cr
    nb_symb_per_lengths_DC = htables_nb_symb_per_lengths[DC][Cr];
    symbols_DC = htables_symbols[DC][Cr];
    nb_symbols_DC = htables_nb_symbols[DC][Cr];
    struct huff_table *table_huff_DC_Cr = huffman_table_build(nb_symb_per_lengths_DC, symbols_DC, nb_symbols_DC);
    
    // Création de la table de Huffman pour coder les AC des blocs Cr
    nb_symb_per_lengths_AC = htables_nb_symb_per_lengths[AC][Cr];
    symbols_AC = htables_symbols[AC][Cr];
    nb_symbols_AC = htables_nb_symbols[AC][Cr];
    struct huff_table *table_huff_AC_Cr = huffman_table_build(nb_symb_per_lengths_AC, symbols_AC, nb_symbols_AC);

    // On crée l'image
    struct jpeg *image = jpeg_create();

    // On indique les paramètres de l'image
    jpeg_set_ppm_filename(image, chemin);
    // char *chemin_jpg = cree_chemin_jpg(chemin);
    jpeg_set_jpeg_filename(image, chemin_jpg);
    jpeg_set_image_width(image, largeur_image);   
    jpeg_set_image_height(image, hauteur_image);
    jpeg_set_nb_components(image, 1);
    jpeg_set_sampling_factor(image, Y, H, MCUs_zigzag[0][0]->h1);
    jpeg_set_sampling_factor(image, Cb, H, MCUs_zigzag[0][0]->h2);
    jpeg_set_sampling_factor(image, Cr, H, MCUs_zigzag[0][0]->h3);
    jpeg_set_sampling_factor(image, Y, V, MCUs_zigzag[0][0]->v1);
    jpeg_set_sampling_factor(image, Cb, V, MCUs_zigzag[0][0]->v2);
    jpeg_set_sampling_factor(image, Cr, V, MCUs_zigzag[0][0]->v3);
    jpeg_set_huffman_table(image, DC, Y, table_huff_DC_Y);
    jpeg_set_huffman_table(image, DC, Cb, table_huff_DC_Cb);
    jpeg_set_huffman_table(image, DC, Cr, table_huff_DC_Cr);
    jpeg_set_huffman_table(image, AC, Y, table_huff_AC_Y);
    jpeg_set_huffman_table(image, AC, Cb, table_huff_AC_Cb);
    jpeg_set_huffman_table(image, AC, Cr, table_huff_AC_Cr);
    jpeg_set_quantization_table(image, Y, quantification_table_Y);
    jpeg_set_quantization_table(image, Cb, quantification_table_CbCr);
    jpeg_set_quantization_table(image, Cr, quantification_table_CbCr);

    // Ecrit l'en-tête
    jpeg_write_header(image);

    // Ecrit le contenu
    struct bitstream *test_stream = jpeg_get_bitstream(image);
    
    int16_t *DC_prec_Y = malloc(sizeof(int16_t));
    int16_t *DC_prec_Cb = malloc(sizeof(int16_t));
    int16_t *DC_prec_Cr = malloc(sizeof(int16_t));
    *DC_prec_Y = 0, *DC_prec_Cb = 0, *DC_prec_Cr = 0;
    
    for (uint32_t hauteur = 0; hauteur < hauteur_MCUs; hauteur++){
        for (uint32_t largeur = 0; largeur < largeur_MCUs; largeur++){
            // On écrit dans le bitstream
            // printf("MCU %d encodée:\n", hauteur * largeur_MCUs + largeur);
            // print_MCU_zigzag(MCUs_zigzag[hauteur][largeur]);
            // printf("\n");
    
            codage_MCU_couleur(MCUs_zigzag[hauteur][largeur], 
                               table_huff_DC_Y,
                               table_huff_DC_Cb,
                               table_huff_DC_Cr,
                               table_huff_AC_Y,
                               table_huff_AC_Cb,
                               table_huff_AC_Cr,
                               DC_prec_Y,
                               DC_prec_Cb,
                               DC_prec_Cr,
                               test_stream);
        }
    }

    // Marque la fin de l'image
    jpeg_write_footer(image);

    free(DC_prec_Y);
    free(DC_prec_Cb);
    free(DC_prec_Cr);
    free(chemin_jpg);

    jpeg_destroy(image);
}


// int main(void)
// {
//     char *chemin = "images/biiiiiig.ppm";
//     FILE *fichier = ouvrir_fichier(chemin, "r");

//     // On récupère l'en-tête (P5 ou P6)
//     char en_tete[10];
//     fgets(en_tete, 10, fichier);

//     // On récupère les dimensions de l'image
//     char dimensions[30];
//     uint32_t largeur_image, hauteur_image;
//     fgets(dimensions, 30, fichier);
//     sscanf(dimensions, "%u %u", &largeur_image, &hauteur_image);
    
//     // On saute une ligne
//     char couleurs_max[10];
//     fgets(couleurs_max, 10, fichier);

//     // Découpage en MCUs
//     uint32_t *dimensions_MCUs = calcul_dimensions_MCUs_RGB(largeur_image, hauteur_image, 1, 1);
//     uint32_t largeur_MCUs = dimensions_MCUs[0], hauteur_MCUs = dimensions_MCUs[1];
//     struct MCU_RGB ***MCUs = decoupage_MCUs(fichier, largeur_image, hauteur_image, largeur_MCUs, hauteur_MCUs, 1, 1, 1, 1, 1, 1);
//     MCUs = decoupage_MCUs_en_blocs(MCUs, largeur_MCUs, hauteur_MCUs, 1, 1);
//     // print_MCUs_RGB(MCUs, dimensions_MCUs);

//     // Conversion YCbCr
//     struct MCU_YCbCr ***MCUs_YCbCr = conversion_matrice_MCUs(MCUs, largeur_MCUs, hauteur_MCUs);
//     // print_matrice_MCU_YCbCr(MCUs_YCbCr, largeur_MCUs, hauteur_MCUs);

//     // DCT
//     struct MCU_freq_Y ***MCUs_freq = transf_cos_Y(MCUs_YCbCr, largeur_MCUs, hauteur_MCUs);
//     // print_MCUs_freq_Y(MCUs_freq, largeur_MCUs, hauteur_MCUs);    

//     // Réorganisation zigzag
//     struct MCU_zigzag_Y ***MCUs_zigzag = zigzag_Y(MCUs_freq, largeur_MCUs, hauteur_MCUs);
//     // print_MCUs_zigzag_Y(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);
    
//     // Quantification
//     quantification_couleur(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);
//     // print_MCUs_zigzag_Y(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);

//     // Codage AC-DC
//     cree_image_couleur(MCUs_zigzag, largeur_MCUs, hauteur_MCUs,
//                        largeur_image, hauteur_image, chemin);

//     free_MCUs_YCbCr(MCUs_YCbCr, dimensions_MCUs);
//     free_MCUs_zigzag_Y(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);
//     free_MCUs_freq_Y(MCUs_freq, largeur_MCUs, hauteur_MCUs);
//     free_MCUs_dims_RGB(MCUs, dimensions_MCUs);
//     fermer_fichier(fichier);
//     return 0;
// }


// test
