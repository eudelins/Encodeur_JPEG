#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../include/bitstream.h"
#include "../include/htables.h"
#include "../include/huffman.h"


/********************/
/* Types de données */
/********************/


/* Type opaque représentant un arbre de Huffman. */
struct huff_table;


/*
    Type contenant l'intégralité des informations 
    nécessaires à l'écriture de l'en-tête JPEG.
*/
struct jpeg{
    uint8_t magic_number;
    char *ppm_filename;
    char *jpg_filename;
    uint32_t image_width;
    uint32_t image_height;
    uint8_t nb_components;
    uint8_t **sampling_factor;
    struct huff_table ***huffman_tables;
    uint8_t **quantification_table;
    struct bitstream *donnees;
};



/***********************************************/
/* Ouverture, fermeture et fonctions générales */
/***********************************************/


/* Alloue la mémoire nécessaire au sampling_factor */
uint8_t **malloc_sampling(void)
{
    uint8_t **sampling_factor = malloc(3 * sizeof(uint8_t *));
    for (uint8_t i = 0; i < 3; i++){
        uint8_t *sampling_fact = malloc(2 * sizeof(uint8_t));
        sampling_factor[i] = sampling_fact;
    }
    return sampling_factor;
}


/* Alloue la mémoire nécessaire aux tables de huffman */
struct huff_table ***malloc_huff_tables(void)
{
    struct huff_table ***tables = malloc(2 * sizeof(struct huff_table **));
    for (uint8_t acdc = 0; acdc < 2; acdc++){
        struct huff_table **tables_acdc = malloc(3 * sizeof(struct huff_table *));
        tables[acdc] = tables_acdc;
    }
    return tables;
}


/* Libère la mémoire allouée au sampling_factor */
void free_sampling(uint8_t **sampling_factor)
{
    for (uint8_t i = 0; i < 3; i++){
        free(sampling_factor[i]);
    }
    free(sampling_factor);
}


/* Libère la mémoire allouée aux tables de huffman */
void free_huff_tables(struct jpeg *jpg)
{
    uint8_t nb_cc = 2 * (jpg->magic_number == 6) + 1;

    for (uint8_t acdc = 0; acdc < 2; acdc++){
        for (uint8_t cc = 0; cc < nb_cc; cc++){
            huffman_table_destroy(jpg->huffman_tables[acdc][cc]);
        }
        free(jpg->huffman_tables[acdc]);
    }
    free(jpg->huffman_tables);
}


/* Alloue et retourne une nouvelle structure jpeg */
extern struct jpeg *jpeg_create(void)
{
    struct jpeg *new_jpeg = malloc(sizeof(struct jpeg));
    new_jpeg->quantification_table = malloc(3 * sizeof(uint8_t *));
    new_jpeg->huffman_tables = malloc_huff_tables();
    new_jpeg->sampling_factor = malloc_sampling();
    return new_jpeg; 
}


/* Détruit une structure jpeg */
extern void jpeg_destroy(struct jpeg *jpg)
{
    bitstream_destroy(jpg->donnees);
    free(jpg->quantification_table);
    free_huff_tables(jpg);
    free_sampling(jpg->sampling_factor);
    free(jpg->jpg_filename);
    free(jpg->ppm_filename);
    free(jpg);
}


/* Ecrit le footer JPEG (marqueur EOI) dans le fichier de sortie. */
extern void jpeg_write_footer(struct jpeg *jpg)
{
    // On force l'écriture des bits en attente dans le buffer
    bitstream_flush(jpg->donnees);
    // End of Image
    bitstream_write_bits(jpg->donnees, 0xffd9, 16, true);  
}


/* Ecrit dans l'en-tête la section Application data */
void ecrit_APPx(struct bitstream *donnees)
{
    bitstream_write_bits(donnees, 0xffe0, 16, true);  // En-tête section
    bitstream_write_bits(donnees, 0x0010, 16, true);  // Longueur section
    bitstream_write_bits(donnees, 'J', 8, true);
    bitstream_write_bits(donnees, 'F', 8, true);
    bitstream_write_bits(donnees, 'I', 8, true);
    bitstream_write_bits(donnees, 'F', 8, true);
    bitstream_write_bits(donnees, '\0', 8, true);
    bitstream_write_bits(donnees, 1, 8, true);
    bitstream_write_bits(donnees, 1, 8, true);
    bitstream_write_bits(donnees, 0, 32, true);
    bitstream_write_bits(donnees, 0, 24, true);
}


/* Ecrit dans l'en-tête la section commentaire */
void ecrit_com(struct bitstream *donnees)
{
    bitstream_write_bits(donnees, 0xfffe, 16, true);  // En-tête section
    bitstream_write_bits(donnees, 0x0010, 16, true);  // Longueur section
    bitstream_write_bits(donnees, 'E', 8, true);
    bitstream_write_bits(donnees, 'Z', 8, true);
    bitstream_write_bits(donnees, ' ', 8, true);
    bitstream_write_bits(donnees, 'l', 8, true);
    bitstream_write_bits(donnees, 'e', 8, true);
    bitstream_write_bits(donnees, ' ', 8, true);
    bitstream_write_bits(donnees, 'p', 8, true);
    bitstream_write_bits(donnees, 'r', 8, true);
    bitstream_write_bits(donnees, 'o', 8, true);
    bitstream_write_bits(donnees, 'j', 8, true);
    bitstream_write_bits(donnees, 'e', 8, true);
    bitstream_write_bits(donnees, 't', 8, true);
    bitstream_write_bits(donnees, ' ', 8, true);
    bitstream_write_bits(donnees, 'C', 8, true);
}


/* Ecrit dans l'en-tête la section DQT */
void ecrit_DQT(struct jpeg *jpg)
{
    bitstream_write_bits(jpg->donnees, 0xffdb, 16, true);  // En-tête section
    bitstream_write_bits(jpg->donnees, 0x0043, 16, true);  // Longueur section
    bitstream_write_bits(jpg->donnees, 0, 8, true);
    
    for (uint8_t i = 0; i < 64; i++){
        bitstream_write_bits(jpg->donnees, jpg->quantification_table[Y][i], 8, true);
    }

    if (jpg->magic_number == 6){
        bitstream_write_bits(jpg->donnees, 0xffdb, 16, true);  // En-tête section
        bitstream_write_bits(jpg->donnees, 0x0043, 16, true);  // Longueur section
        bitstream_write_bits(jpg->donnees, 1, 8, true);
    
        for (uint8_t i = 0; i < 64; i++){
            bitstream_write_bits(jpg->donnees, jpg->quantification_table[Cb][i], 8, true);
        }
    } 
}


/* Ecrit dans l'en-tête la section SOFx */
void ecrit_SOFx(struct jpeg *jpg)
{
    bitstream_write_bits(jpg->donnees, 0xffc0, 16, true);  // En-tête section
    
    uint8_t N;
    if (jpg->magic_number == 5){
        N = 1;
    } else {
        N = 3;
    }

    bitstream_write_bits(jpg->donnees, 3*N + 8, 16, true);  // Longueur section
    bitstream_write_bits(jpg->donnees, 8, 8, true);
    bitstream_write_bits(jpg->donnees, jpg->image_height, 16, true);
    bitstream_write_bits(jpg->donnees, jpg->image_width, 16, true);    

    bitstream_write_bits(jpg->donnees, N, 8, true);
    for (uint8_t i = 0; i < N; i++){
        bitstream_write_bits(jpg->donnees, i + 1, 8, true);
        bitstream_write_bits(jpg->donnees, jpg->sampling_factor[i][H], 4, true);
        bitstream_write_bits(jpg->donnees, jpg->sampling_factor[i][V], 4, true);
        bitstream_write_bits(jpg->donnees, (i > 0), 8, true);
    }
}


/* Ecrit dans l'en-tête la section DHT d'une table de huffman */
void ecrit_DHT_table(struct jpeg *jpg, uint8_t acdc, uint8_t cc)
{
    bitstream_write_bits(jpg->donnees, 0xffc4, 16, true);  // En-tête section

    uint8_t *longueur_symbols = huffman_table_get_length_vector(jpg->huffman_tables[acdc][cc]);
    uint32_t nb_symbols = 0;
    for (uint8_t i = 0; i < 16; i++){
        nb_symbols += longueur_symbols[i];
    }

    bitstream_write_bits(jpg->donnees, 19 + nb_symbols, 16, true);  // Longueur section
    
    bitstream_write_bits(jpg->donnees, 0, 3, true);
    bitstream_write_bits(jpg->donnees, acdc, 1, true);
    bitstream_write_bits(jpg->donnees, cc, 4, true);

    for (uint8_t i = 0; i < 16; i++){
        bitstream_write_bits(jpg->donnees, longueur_symbols[i], 8, true);
    }

    uint8_t *symbols = huffman_table_get_symbols(jpg->huffman_tables[acdc][cc]);
    for(uint32_t i = 0; i < nb_symbols; i++){
        bitstream_write_bits(jpg->donnees, symbols[i], 8, true);
    }
}


/* Ecrit dans l'en-tête la section DHT */
void ecrit_DHT(struct jpeg *jpg)
{
    uint8_t nb_cc;
    if (jpg->magic_number == 5){
        nb_cc = 1;
    } else {
        nb_cc = 2;
    }

    for(uint8_t cc = 0; cc < nb_cc; cc++){
        for(uint8_t acdc = 0; acdc < 2; acdc++){
            ecrit_DHT_table(jpg, acdc, cc);
        }
    }
}


/* Ecrit dans l'en-tête la section SOS */
void ecrit_SOS(struct jpeg *jpg)
{
    bitstream_write_bits(jpg->donnees, 0xffda, 16, true);  // Marqueur SOS
    
    uint8_t N;
    if (jpg->magic_number == 5){
        N = 1;
    } else {
        N = 3;
    }
    
    bitstream_write_bits(jpg->donnees, 2*N + 6, 16, true);  // Longueur section
    bitstream_write_bits(jpg->donnees, N, 8, true);

    for (uint8_t i = 0; i < N; i++){
        bitstream_write_bits(jpg->donnees, i + 1, 8, true);
        bitstream_write_bits(jpg->donnees, (i > 0), 4, true);
        bitstream_write_bits(jpg->donnees, (i > 0), 4, true);
    }

        bitstream_write_bits(jpg->donnees, 0, 8, true);
        bitstream_write_bits(jpg->donnees, 63, 8, true);
        bitstream_write_bits(jpg->donnees, 0, 8, true);
}


/*
    Ecrit tout l'en-tête JPEG dans le fichier de sortie à partir des
    informations contenues dans la structure jpeg passée en paramètre. 
    En sortie, le bitstream est positionné juste après l'écriture de 
    l'en-tête SOS, à l'emplacement du premier octet de données brutes à écrire.
*/
extern void jpeg_write_header(struct jpeg *jpg)
{
    jpg->donnees = bitstream_create(jpg->jpg_filename);

    // Start of Image
    bitstream_write_bits(jpg->donnees, 0xffd8, 16, true);

    // Section APP0
    ecrit_APPx(jpg->donnees);

    // Section commentaire
    ecrit_com(jpg->donnees);

    // Section DQT
    ecrit_DQT(jpg);

    // Section SOFx
    ecrit_SOFx(jpg);

    // Section DHT
    ecrit_DHT(jpg);

    // Section SOS
    ecrit_SOS(jpg);

}


/*
    Retourne le bitstream associé au fichier de sortie enregistré 
    dans la structure jpeg.
*/
extern struct bitstream *jpeg_get_bitstream(struct jpeg *jpg)
{
    return jpg->donnees;
}



/* Ecrit le nom de fichier PPM ppm_filename dans la structure jpeg. */
extern void jpeg_set_ppm_filename(struct jpeg *jpg,
                                  const char *ppm_filename)
{
    jpg->ppm_filename = malloc((strlen(ppm_filename) + 1) * sizeof(char));
    strcpy(jpg->ppm_filename, ppm_filename);
    if (ppm_filename[strlen(ppm_filename) - 2] == 'g'){
        jpg->magic_number = 5;
    } else {
        jpg->magic_number = 6;
    }
}


/* Retourne le nom de fichier PPM lu dans la structure jpeg. */
extern char *jpeg_get_ppm_filename(struct jpeg *jpg)
{
    return jpg->ppm_filename;
}


/* Ecrit le nom du fichier de sortie jpeg_filename dans la structure jpeg. */
extern void jpeg_set_jpeg_filename(struct jpeg *jpg,
                                   const char *jpeg_filename)
{
    jpg->jpg_filename = malloc((strlen(jpeg_filename) + 1) * sizeof(char));
    strcpy(jpg->jpg_filename, jpeg_filename);
}


/* Retourne le nom du fichier de sortie lu depuis la structure jpeg. */
extern char *jpeg_get_jpeg_filename(struct jpeg *jpg)
{
    return jpg->jpg_filename;
}


/*
    Ecrit la hauteur de l'image traitée, en nombre de pixels,
    dans la structure jpeg.
*/
extern void jpeg_set_image_height(struct jpeg *jpg,
                                  uint32_t image_height)
{
    jpg->image_height = image_height;
}


/*
    Retourne la hauteur de l'image traitée, en nombre de pixels,
    lue dans la structure jpeg.
*/
extern uint32_t jpeg_get_image_height(struct jpeg *jpg)
{
    return jpg->image_height;
}


/*
    Ecrit la largeur de l'image traitée, en nombre de pixels,
    dans la structure jpeg.
*/
extern void jpeg_set_image_width(struct jpeg *jpg,
                                 uint32_t image_width)
{
    jpg->image_width = image_width;
}


/*
    Retourne la largeur de l'image traitée, en nombre de pixels,
    lue dans la structure jpeg.
*/
extern uint32_t jpeg_get_image_width(struct jpeg *jpg)
{
    return jpg->image_width;
}


/*
    Ecrit le nombre de composantes de couleur de l'image traitée
    dans la structure jpeg.
*/
extern void jpeg_set_nb_components(struct jpeg *jpg,
                                   uint8_t nb_components)
{
    jpg->nb_components = nb_components;
}


/*
    Retourne le nombre de composantes de couleur de l'image traitée 
    lu dans la structure jpeg.
*/
extern uint8_t jpeg_get_nb_components(struct jpeg *jpg)
{
    return jpg->nb_components;
}


extern void jpeg_set_sampling_factor(struct jpeg *jpg,
                                     enum color_component cc,
                                     enum direction dir,
                                     uint8_t sampling_factor)
{
    jpg->sampling_factor[cc][dir] = sampling_factor;
}


/*
    Retourne le facteur d'échantillonnage utilisé pour la composante 
    de couleur cc et la direction dir, lu dans la structure jpeg.
*/
extern uint8_t jpeg_get_sampling_factor(struct jpeg *jpg,
                                        enum color_component cc,
                                        enum direction dir)
{
    return jpg->sampling_factor[cc][dir];
}


/*
    Ecrit dans la structure jpeg la table de Huffman huff_table à utiliser
    pour encoder les données de la composante fréquentielle acdc, pour la
    composante de couleur cc.
*/
extern void jpeg_set_huffman_table(struct jpeg *jpg,
                                   enum sample_type acdc,
                                   enum color_component cc,
                                   struct huff_table *htable)
{
    jpg->huffman_tables[acdc][cc] = htable;
}



/*
    Retourne un pointeur vers la table de Huffman utilisée pour encoder
    les données de la composante fréquentielle acdc pour la composante 
    de couleur cc, lue dans la structure jpeg.
*/
extern struct huff_table *jpeg_get_huffman_table(struct jpeg *jpg,
                                                 enum sample_type acdc,
                                                 enum color_component cc)
{
    return jpg->huffman_tables[acdc][cc];
}



/*
    Ecrit dans la structure jpeg la table de quantification à utiliser
    pour compresser les coefficients de la composante de couleur cc.
*/
extern void jpeg_set_quantization_table(struct jpeg *jpg,
                                        enum color_component cc,
                                        uint8_t *qtable)
{
    jpg->quantification_table[cc] = qtable;
}


/*
    Retourne un pointeur vers la table de quantification associée à la 
    composante de couleur cc, lue dans a structure jpeg.
*/
extern uint8_t *jpeg_get_quantization_table(struct jpeg *jpg,
                                            enum color_component cc)
{
    return jpg->quantification_table[cc];
}


// int main(void)
// {
//     // On crée l'image
//     struct jpeg *image = jpeg_create();

//     // Création de la table de Huffman pour coder les magnitudes des DC de Y
//     uint8_t *nb_symb_per_lengths_DC_Y = htables_nb_symb_per_lengths[DC][Y];
//     uint8_t *symbols_DC_Y = htables_symbols[DC][Y];
//     uint8_t nb_symbols_DC_Y = htables_nb_symbols[DC][Y];
//     struct huff_table *table_huff_DC_Y = huffman_table_build(nb_symb_per_lengths_DC_Y, symbols_DC_Y, nb_symbols_DC_Y);
    
//     // Création de la table de Huffman pour coder les AC de Y
//     uint8_t *nb_symb_per_lengths_AC_Y = htables_nb_symb_per_lengths[AC][Y];
//     uint8_t *symbols_AC_Y = htables_symbols[AC][Y];
//     uint8_t nb_symbols_AC_Y = htables_nb_symbols[AC][Y];
//     struct huff_table *table_huff_AC_Y = huffman_table_build(nb_symb_per_lengths_AC_Y, symbols_AC_Y, nb_symbols_AC_Y);

//     // Création de la table de Huffman pour coder les magnitudes des DC de Cb
//     uint8_t *nb_symb_per_lengths_DC_Cb = htables_nb_symb_per_lengths[DC][Cb];
//     uint8_t *symbols_DC_Cb = htables_symbols[DC][Cb];
//     uint8_t nb_symbols_DC_Cb = htables_nb_symbols[DC][Cb];
//     struct huff_table *table_huff_DC_Cb = huffman_table_build(nb_symb_per_lengths_DC_Cb, symbols_DC_Cb, nb_symbols_DC_Cb);
    
//     // Création de la table de Huffman pour coder les AC de Cb
//     uint8_t *nb_symb_per_lengths_AC_Cb = htables_nb_symb_per_lengths[AC][Cb];
//     uint8_t *symbols_AC_Cb = htables_symbols[AC][Cb];
//     uint8_t nb_symbols_AC_Cb = htables_nb_symbols[AC][Cb];
//     struct huff_table *table_huff_AC_Cb = huffman_table_build(nb_symb_per_lengths_AC_Cb, symbols_AC_Cb, nb_symbols_AC_Cb);
    
//     // Création de la table de Huffman pour coder les magnitudes des DC de Cr
//     uint8_t *nb_symb_per_lengths_DC_Cr = htables_nb_symb_per_lengths[DC][Cr];
//     uint8_t *symbols_DC_Cr = htables_symbols[DC][Cr];
//     uint8_t nb_symbols_DC_Cr = htables_nb_symbols[DC][Cr];
//     struct huff_table *table_huff_DC_Cr = huffman_table_build(nb_symb_per_lengths_DC_Cr, symbols_DC_Cr, nb_symbols_DC_Cr);
    
//     // Création de la table de Huffman pour coder les AC de Cr
//     uint8_t *nb_symb_per_lengths_AC_Cr = htables_nb_symb_per_lengths[AC][Cr];
//     uint8_t *symbols_AC_Cr = htables_symbols[AC][Cr];
//     uint8_t nb_symbols_AC_Cr = htables_nb_symbols[AC][Cr];
//     struct huff_table *table_huff_AC_Cr = huffman_table_build(nb_symb_per_lengths_AC_Cr, symbols_AC_Cr, nb_symbols_AC_Cr);
    
//     // On indique les paramètres de l'image
//     char *chemin = "images/invadered.ppm";
//     jpeg_set_ppm_filename(image, chemin);
//     char *chemin_jpg = cree_chemin_jpg(chemin);
//     jpeg_set_jpeg_filename(image, chemin_jpg);
//     free(chemin_jpg);
//     jpeg_set_quantization_table(image, Y, quantification_table_Y);
//     jpeg_set_quantization_table(image, Cb, quantification_table_CbCr);
//     jpeg_set_quantization_table(image, Cr, quantification_table_CbCr);
//     jpeg_set_image_width(image, 8);   
//     jpeg_set_image_height(image, 8);
//     jpeg_set_nb_components(image, 1);
//     jpeg_set_sampling_factor(image, Y, H, 1);
//     jpeg_set_sampling_factor(image, Y, V, 1);
//     jpeg_set_sampling_factor(image, Cb, H, 1);
//     jpeg_set_sampling_factor(image, Cb, V, 1);
//     jpeg_set_sampling_factor(image, Cr, H, 1);
//     jpeg_set_sampling_factor(image, Cr, V, 1);
//     jpeg_set_huffman_table(image, DC, Y, table_huff_DC_Y);
//     jpeg_set_huffman_table(image, AC, Y, table_huff_AC_Y);
//     jpeg_set_huffman_table(image, DC, Cb, table_huff_DC_Cb);
//     jpeg_set_huffman_table(image, AC, Cb, table_huff_AC_Cb);
//     jpeg_set_huffman_table(image, DC, Cr, table_huff_DC_Cr);
//     jpeg_set_huffman_table(image, AC, Cr, table_huff_AC_Cr);
//     jpeg_set_quantization_table(image, Y, quantification_table_Y);

//     // Ecrit l'en-tête
//     jpeg_write_header(image);

//     // Marque la fin de l'image
//     jpeg_write_footer(image);

//     jpeg_destroy(image);
//     return 0;
// }