#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../include/decoupe.h"
#include "../include/encodage.h"
#include "../include/htables.h"
#include "../include/qtables.h"
#include "../include/huffman.h"
#include "../include/bitstream.h"
#include "../include/jpeg_writer.h"


/***********************************************************************/
/* Module d'écriture du fichier jpeg pour les images en niveau de gris */
/***********************************************************************/


/* Calcule x puissance y */
uint32_t puissance(uint8_t x, uint8_t y)
{
    uint16_t res = 1;
    for (uint8_t i = 0; i < y; i++){
        res = res * x;
    }
    return res;
}


/* Calcul la magnitude d'un pixel */
uint8_t calcule_magnitude(int16_t pixel)
{
    uint8_t magnitude = 0;
    int16_t borne_inf = -puissance(2, magnitude);
    int16_t borne_sup = puissance(2, magnitude);
    while (!(borne_inf < pixel && pixel < borne_sup )){
        magnitude += 1;
        borne_inf = -puissance(2, magnitude);
        borne_sup = puissance(2, magnitude);
    }
    return magnitude;
}


/* Calcul l'indice de pixel qui est de magnitude magnitude */
uint16_t calcule_index(int16_t pixel, uint8_t magnitude)
{
    uint16_t index = 0;
    int16_t borne_inf = puissance(2, magnitude - 1);
    int16_t borne_sup = puissance(2, magnitude) - 1;
    if (pixel < 0){
        for (int16_t val = -borne_sup; val <= -borne_inf; val++){
            if (val == pixel){
                return index;
            }
            index += 1;
        }
    } else {
        index += borne_inf;
        for (int16_t val = borne_inf; val <= borne_sup; val++){
            if (val == pixel){
                return index;
            }
            index += 1;
        }
    }
    return index;
}

   
/* Ecrit dans le bitstream les bits issus du codage DC d'une MCU */
void codage_DC_MCU(struct MCU_zigzag *MCU_zigzag,
                   struct huff_table *table_huff_DC_Y,
                   int16_t *DC_prec,
                   struct bitstream *test_stream)
{
    uint8_t magnitude;
    uint16_t index;
    uint32_t codage_huff;
    uint8_t *nb_bits = malloc(sizeof(uint8_t));
    int16_t *pixels = MCU_zigzag->pixels_zigzag;
    
    magnitude = calcule_magnitude(pixels[0] - *DC_prec);
    index = calcule_index(pixels[0] - *DC_prec, magnitude);
    codage_huff = huffman_table_get_path(table_huff_DC_Y, magnitude, nb_bits);

    // On écrit dans le bistream
    bitstream_write_bits(test_stream, codage_huff, *nb_bits, false);
    bitstream_write_bits(test_stream, index, magnitude, false);
    
    free(nb_bits);
    *DC_prec = pixels[0];
}


/* Ecrit dans le bitstream les bits issus du codage AC d'une MCU */
void codage_AC_MCU(struct MCU_zigzag *MCU_zigzag,
                   struct huff_table *table_huff_AC_Y,
                   struct bitstream *test_stream)
{
    uint8_t RLE_code, magnitude;
    uint16_t index;
    uint32_t codage_huff;
    uint8_t *nb_bits = malloc(sizeof(uint8_t));
    int16_t *pixels = MCU_zigzag->pixels_zigzag;

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
                codage_huff = huffman_table_get_path(table_huff_AC_Y, RLE_code, nb_bits);
                bitstream_write_bits(test_stream, codage_huff, *nb_bits, false);
            }
            compteur_16_0 = 0;

            // On décale le nombre de 0 de 4 bits vers la gauche (bits de poids fort)
            compteur_0 = compteur_0 << 4;

            // On calcule la magnitude du pixel (bits de poids faible)
            magnitude = calcule_magnitude(pixels[indice]);

            // On calcule l'index
            index = calcule_index(pixels[indice], magnitude);

            RLE_code = compteur_0 + magnitude;
            codage_huff = huffman_table_get_path(table_huff_AC_Y, RLE_code, nb_bits);

            // On écrit dans le bitstream
            bitstream_write_bits(test_stream, codage_huff, *nb_bits, false);
            bitstream_write_bits(test_stream, index, magnitude, false);

            // On remet le compteur de 0 à 0
            compteur_0 = 0;
        }
    }
    
    // End of Block
    if (pixels[63] == 0){
        RLE_code = 0x00;
        codage_huff = huffman_table_get_path(table_huff_AC_Y, RLE_code, nb_bits);
        bitstream_write_bits(test_stream, codage_huff, *nb_bits, false);
    }
 
    free(nb_bits);
}


/* Affiche le codage AC/DC d'une MCU */
void codage_AC_DC_MCU(struct MCU_zigzag *MCU_zigzag,
                      struct huff_table *table_huff_DC_Y,
                      struct huff_table *table_huff_AC_Y,
                      int16_t *DC_prec,
                      struct bitstream *test_stream)
{
    // Codage DC
    codage_DC_MCU(MCU_zigzag, table_huff_DC_Y, DC_prec, test_stream);

    // Codage AC
    codage_AC_MCU(MCU_zigzag, table_huff_AC_Y, test_stream);
}


/* Renvoie le chemin jpg à partir du chemin pgm ou ppm */
char *cree_chemin_jpg(char *chemin)
{
    size_t taille = strlen(chemin);
    char *chemin_jpg = malloc((taille + 1) * sizeof(char));
    
    for (uint32_t indice = 0; indice < taille - 3; indice++) {
        chemin_jpg[indice] = chemin[indice];
    }
    chemin_jpg[taille - 3] = 'j';
    chemin_jpg[taille - 2] = 'p';
    chemin_jpg[taille - 1] = 'g';
    chemin_jpg[taille] = '\0';
    return chemin_jpg;
}


/* Crée l'image jpeg en niveau de gris */
void cree_image(struct MCU_zigzag ***MCUs_zigzag, 
                uint32_t largeur_MCUs, 
                uint32_t hauteur_MCUs,
                uint32_t largeur_image, 
                uint32_t hauteur_image,
                char *chemin,
                char *chemin_jpg)
{
    // Création de la table de Huffman pour coder les magnitudes des DC
    uint8_t *nb_symb_per_lengths_DC = htables_nb_symb_per_lengths[DC][Y];
    uint8_t *symbols_DC = htables_symbols[DC][Y];
    uint8_t nb_symbols_DC = htables_nb_symbols[DC][Y];
    struct huff_table *table_huff_DC_Y = huffman_table_build(nb_symb_per_lengths_DC, symbols_DC, nb_symbols_DC);
    
    // Création de la table de Huffman pour coder les AC
    uint8_t *nb_symb_per_lengths_AC = htables_nb_symb_per_lengths[AC][Y];
    uint8_t *symbols_AC = htables_symbols[AC][Y];
    uint8_t nb_symbols_AC = htables_nb_symbols[AC][Y];
    struct huff_table *table_huff_AC_Y = huffman_table_build(nb_symb_per_lengths_AC, symbols_AC, nb_symbols_AC);

    // On crée l'image
    struct jpeg *image = jpeg_create();

    // On indique les paramètres de l'image
    jpeg_set_ppm_filename(image, chemin);
    jpeg_set_jpeg_filename(image, chemin_jpg);
    jpeg_set_image_width(image, largeur_image);   
    jpeg_set_image_height(image, hauteur_image);
    jpeg_set_nb_components(image, 1);
    jpeg_set_sampling_factor(image, Y, H, 1);
    jpeg_set_sampling_factor(image, Y, V, 1);
    jpeg_set_huffman_table(image, DC, Y, table_huff_DC_Y);
    jpeg_set_huffman_table(image, AC, Y, table_huff_AC_Y);
    jpeg_set_quantization_table(image, Y, quantification_table_Y);

    // Ecrit l'en-tête
    jpeg_write_header(image);

    // On récupère le bitstream pour écrire le contenu de l'image
    struct bitstream *test_stream = jpeg_get_bitstream(image);
    
    int16_t *DC_prec = malloc(sizeof(int16_t));
    *DC_prec = 0;
    
    // On écrit le contenu dans le bitstream
    for (uint32_t hauteur = 0; hauteur < hauteur_MCUs; hauteur++){
        for (uint32_t largeur = 0; largeur < largeur_MCUs; largeur++){
            codage_AC_DC_MCU(MCUs_zigzag[hauteur][largeur], 
                             table_huff_DC_Y, 
                             table_huff_AC_Y, 
                             DC_prec, 
                             test_stream);
        }
    }

    // Marque la fin de l'image
    jpeg_write_footer(image);

    free(DC_prec);
    free(chemin_jpg);
    jpeg_destroy(image);
}


// int main(void)
// {
//     // char *filename = "images/complexite.pgm";
//     FILE *fichier = ouvrir_fichier("images/complexite.pgm", "r");

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
//     uint32_t *dimensions_MCUs = calcul_dimensions_MCUs(largeur_image, hauteur_image, 8, 8);
//     uint32_t largeur_MCUs = dimensions_MCUs[0], hauteur_MCUs = dimensions_MCUs[1];
//     struct MCU ***MCUs = decoupage(fichier, largeur_MCUs, hauteur_MCUs, largeur_image, hauteur_image);
//     print_MCUs(MCUs, dimensions_MCUs);

//     // DCT
//     struct MCU_freq ***MCUs_freq = transf_cos(MCUs, largeur_MCUs, hauteur_MCUs);
//     print_MCUs_freq(MCUs_freq, largeur_MCUs, hauteur_MCUs);    

//     // Réorganisation zigzag
//     struct MCU_zigzag ***MCUs_zigzag = zigzag(MCUs_freq, largeur_MCUs, hauteur_MCUs);
    
//     // Quantification
//     quantification(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);
//     print_MCUs_zigzag(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);

//     // Codage AC-DC
//     cree_image(MCUs_zigzag, largeur_MCUs, hauteur_MCUs, largeur_image, hauteur_image);

//     free_MCUs_zigzag(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);
//     free_MCUs_freq(MCUs_freq, largeur_MCUs, hauteur_MCUs);
//     free_MCUs(MCUs, dimensions_MCUs);
//     fermer_fichier(fichier);
//     return 0;
// }