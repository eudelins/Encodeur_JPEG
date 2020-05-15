#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/decoupe.h"
#include "../include/encodage.h"
#include "../include/htables.h"
#include "../include/huffman.h"
#include "../include/bitstream.h"


/* Affiche n en binaire si n est codé sur nb_bits bits */
void print_binaire(uint32_t n, uint8_t nb_bits)
{
    for (int8_t i = nb_bits; i >= 0; i--){
        printf("%d", (n >> i ) & 1);
    }
    printf("\n");
}


/* Calcule x puissance y */
int16_t puissance(uint8_t x, uint8_t y)
{
    int16_t res = 1;
    for (uint8_t i = 0; i < y; i++){
        res = res * x;
    }
    return res;
}


/* Calcul la magnitude de pixel */
uint8_t calcule_magnitude(int16_t pixel){
    uint8_t magnitude = 0;
    while (!( -puissance(2, magnitude) < pixel && 
                pixel < puissance(2,magnitude) )){
        magnitude += 1;
    }
    return magnitude;
}


/* Calcul l'indice de pixel qui est de magnitude magnitude */
int8_t calcule_index(int16_t pixel, uint8_t magnitude)
{
    int8_t index = 0;
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
    uint8_t magnitude, index;
    uint32_t codage_huff;
    uint8_t *nb_bits = malloc(sizeof(uint8_t));
    int16_t *pixels = MCU_zigzag->pixels_zigzag;
    
    magnitude = calcule_magnitude(pixels[0] - *DC_prec);
    index = calcule_index(pixels[0] - *DC_prec, magnitude);
    codage_huff = huffman_table_get_path(table_huff_DC_Y, magnitude, nb_bits);
    *DC_prec = pixels[0];

    // On écrit dans le bistream
    bitstream_write_bits(test_stream, codage_huff, *nb_bits, false);
    bitstream_write_bits(test_stream, index, magnitude, false);
    
    free(nb_bits);
}


/* Ecrit dans le bitstream les bits issus du codage AC d'une MCU */
void codage_AC_MCU(struct MCU_zigzag *MCU_zigzag,
                   struct huff_table *table_huff_AC_Y,
                   struct bitstream *test_stream)
{
    uint8_t RLE_code, magnitude, index;
    uint32_t codage_huff;
    uint8_t *nb_bits = malloc(sizeof(uint8_t));
    int16_t *pixels = MCU_zigzag->pixels_zigzag;

    int8_t compteur_0 = 0;
    for(uint8_t indice = 1; indice < 64; indice++){
        if (pixels[indice] == 0 && compteur_0 < 16) {
            compteur_0 += 1;
        } else if (pixels[indice] == 0 && compteur_0 < 16) {
            // Cas des 16 0 consécutifs
            compteur_0 = 0;
            RLE_code = 0xF0;
            codage_huff = huffman_table_get_path(table_huff_AC_Y, RLE_code, nb_bits);
            bitstream_write_bits(test_stream, codage_huff, *nb_bits, false);
        } else {
            // On décale le nombre de 0 de 4 bits vers la gauche (bits de poids forts)
            compteur_0 = compteur_0 << 4;

            // On calcule la magnitude du pixel
            magnitude = calcule_magnitude(pixels[indice]);

            // On calcule l'index
            index = calcule_index(pixels[indice], magnitude);

            RLE_code = compteur_0 + magnitude;
            codage_huff = huffman_table_get_path(table_huff_AC_Y, RLE_code, nb_bits);

            // On écrit dans le bitstream
            bitstream_write_bits(test_stream, codage_huff, *nb_bits, false);
            bitstream_write_bits(test_stream, index, magnitude, false);

            // On remet le compteur à 0
            compteur_0 = 0;
        }
    }
    
    // End of Block
    RLE_code = 0x00;
    codage_huff = huffman_table_get_path(table_huff_AC_Y, RLE_code, nb_bits);
    bitstream_write_bits(test_stream, codage_huff, *nb_bits, false);
 
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


/* Affiche le codage AC/DC des MCUs */
struct bitstream *codage_AC_DC(struct MCU_zigzag ***MCUs_zigzag, uint32_t largeur_MCUs, uint32_t hauteur_MCUs)
{
    // On crée un bitstream
    struct bitstream *test_stream = bitstream_create("test.jpg");
    
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
    
    // On crée une variable contenant la valeur du DC précédent
    int16_t *DC_prec = malloc(sizeof(int16_t));
    *DC_prec = 0;
    
    for (uint32_t hauteur = 0; hauteur < hauteur_MCUs; hauteur++){
        for (uint32_t largeur = 0; largeur < largeur_MCUs; largeur++){
            // On écrit dans le bitstream
            codage_AC_DC_MCU(MCUs_zigzag[hauteur][largeur], 
                             table_huff_DC_Y, 
                             table_huff_AC_Y, 
                             DC_prec, 
                             test_stream);
        }
    }
    free(DC_prec);
    huffman_table_destroy(table_huff_DC_Y);
    huffman_table_destroy(table_huff_AC_Y);

    bitstream_flush(test_stream);
    return test_stream;
}



int main(void)
{
    FILE *fichier = ouvrir_fichier("images/invader.pgm", "r");

    // Découpage en MCUs
    struct MCU ***MCUs = decoupage(fichier, 8, 8);
    uint32_t *dimensions_MCUs = calcul_dimensions_MCUs(8, 8, 8, 8);
    uint32_t largeur_MCUs = dimensions_MCUs[0], hauteur_MCUs = dimensions_MCUs[1];

    // DCT
    struct MCU_freq ***MCUs_freq = transf_cos(MCUs, largeur_MCUs, hauteur_MCUs);
    
    // Réorganisation zigzag
    struct MCU_zigzag ***MCUs_zigzag = zigzag(MCUs_freq, largeur_MCUs, hauteur_MCUs);
    
    // Quantification
    quantification(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);

    // Codage AC-DC
    struct bitstream *test_stream = codage_AC_DC(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);

    bitstream_destroy(test_stream);
    free_MCUs_zigzag(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);
    free_MCUs_freq(MCUs_freq, largeur_MCUs, hauteur_MCUs);
    free_MCUs(MCUs, dimensions_MCUs);
    fermer_fichier(fichier);
    return 0;
}



// printf("DC:\n\tvalue: %d\t", pixels[0] - *DC_prec);
// printf("magnitude: %d\t", magnitude);
// printf("index: %d à écrire sur %d bits\n", index, magnitude);
// printf("\tHuffman: %d à écrire sur %d bits\n", codage_huff, *nb_bits);


// printf("\tvalue: %d\t", pixels[indice]);
// printf("magnitude: %d\t", magnitude);
// printf("index: %d à écrire sur %d bits\n", index, magnitude);
// printf("\tcode RLE: %d\t", RLE_code);
// printf("\tHuffman: %d à écrire sur %d bits\n", codage_huff, *nb_bits);
// printf("\n");


// printf("\tvalue: endofblock");
// printf("\tcode RLE: %d\t", RLE_code);
// printf("Huffman: %d à écrire sur %d bits\n", codage_huff, *nb_bits);
// printf("\n");
