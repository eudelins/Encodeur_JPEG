#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../include/htables.h"


/* Type représentant un arbre de Huffman. */
struct huff_table{
    uint8_t *nb_symb_per_lengths;
    uint8_t *symbols;
    uint8_t nb_symbols;
    uint8_t *longueurs_chemins;
    uint32_t *chemins;
};


/* Calcule le chemin du ième élément de la table de Huffman */
void complete_ieme_chemin(struct huff_table *ht, uint8_t indice)
{
    // On cherche le nb de bits codant value
    uint8_t nb_elem = 0;
    for (uint8_t i_nb_bits = 0; i_nb_bits < 16; i_nb_bits++){
        nb_elem += ht->nb_symb_per_lengths[i_nb_bits];
        if (indice < nb_elem){
            ht->longueurs_chemins[indice] = i_nb_bits + 1;
            break;
        }
    }

    if (indice == 0){
        ht->chemins[indice] = 0;
        return;
    }

    // On calcule la différence de taille des chemins entre le chemin courant et le chemin précédent
    uint8_t diff_taille_prec = ht->longueurs_chemins[indice] - ht->longueurs_chemins[indice - 1];

    if (diff_taille_prec == 0){
        ht->chemins[indice] = ht->chemins[indice - 1] + 1;
        return;
    }

    // On cherche le premier 0 dans le chemin précédent
    uint32_t chemin = ht->chemins[indice - 1];
    uint8_t nb_remonte = 0;
    while ((chemin % 2) != 0){
        chemin = chemin / 2;
        nb_remonte += 1;
    }
    chemin += 1;  // On remplace le 0 par un 1

    // On complète la fin du chemin par des 0
    for (uint8_t i = 0; i < diff_taille_prec + nb_remonte; i++){
        chemin = chemin << 1;
    }
    ht->chemins[indice] = chemin;
}


/* Remplie les tableaux chemins et longueurs_chemins de ht */
void complete_chemins(struct huff_table *ht)
{
    for (uint8_t i = 0; i < ht->nb_symbols; i++){
        complete_ieme_chemin(ht, i);
    }
}



/*
    Construit un arbre de Huffman à partir d'une table
    de symboles comme présenté en section 2.10.1 du sujet.
    nb_symb_per_lengths est un tableau contenant le nombre
    de symboles pour chaque longueur de 1 à 16,
    symbols est le tableau  des symboles ordonnés,
    et nb_symbols représente la taille du tableau symbols.
*/
extern struct huff_table *huffman_table_build(uint8_t *nb_symb_per_lengths,
                                              uint8_t *symbols,
                                              uint8_t nb_symbols)
{
    struct huff_table *new_table = malloc(sizeof(struct huff_table));
    new_table->nb_symb_per_lengths = nb_symb_per_lengths;
    new_table->symbols = symbols;
    new_table->nb_symbols = nb_symbols;

    uint8_t *longueurs_chemins = malloc(nb_symbols * sizeof(uint8_t));
    uint32_t *chemins = malloc(nb_symbols * sizeof(uint32_t));
    new_table->longueurs_chemins = longueurs_chemins;
    new_table->chemins = chemins;
    complete_chemins(new_table);
    
    return new_table;
}


/*
    Détruit l'arbre de Huffman passé en paramètre et libère
    toute la mémoire qui lui est associée.
*/
extern void huffman_table_destroy(struct huff_table *ht)
{
    free(ht->chemins);
    free(ht->longueurs_chemins);
    free(ht);
}


/*
    Retourne le chemin dans l'arbre ht permettant d'atteindre
    la feuille de valeur value. nb_bits est un paramètre de sortie
    permettant de stocker la longueur du chemin retourné.
*/
extern uint32_t huffman_table_get_path(struct huff_table *ht,
                                       uint8_t value,
                                       uint8_t *nb_bits)
{
    // On calcule l'indice de value dans la table de Huffman
    uint8_t indice_value = ht->nb_symbols + 1;
    for (uint8_t indice = 0; indice < ht->nb_symbols; indice++){
        if (value == ht->symbols[indice]){
            indice_value = indice;
            break;
        }
    }

    if (indice_value == ht->nb_symbols + 1){
        fprintf(stderr, "Erreur, élément non présent dans la table de Huffman\n");
        return EXIT_FAILURE;
    }

    *nb_bits = ht->longueurs_chemins[indice_value];
    return ht->chemins[indice_value];
}


/*
   Retourne le tableau des symboles associé à l'arbre de
   Huffman passé en paramètre.
*/
extern uint8_t *huffman_table_get_symbols(struct huff_table *ht)
{
    return ht->symbols;
}


/*
    Retourne le tableau du nombre de symboles de chaque longueur
    associé à l'arbre de Huffman passé en paramètre.
*/
extern uint8_t *huffman_table_get_length_vector(struct huff_table *ht)
{
    return ht->nb_symb_per_lengths;
}


void print_table(struct huff_table *ht)
{
    printf("Nb de symbols: %u\n\n", ht->nb_symbols);
    for (uint8_t i = 0; i < ht->nb_symbols; i++){
        printf("Elem numero %u: %u\n", i, ht->symbols[i]);
    }
    printf("\n");
    for (uint8_t i = 0; i < 16; i++){
        printf("Nb elem de taille %u: %u\n", i + 1, ht->nb_symb_per_lengths[i]);
    }
    printf("\n");
}


// int main(void)
// {
//     // Création de la table de Huffman pour coder les magnitudes des DC de Y
//     uint8_t *nb_symb_per_lengths_DC_Y = htables_nb_symb_per_lengths[DC][Y];
//     uint8_t *symbols_DC_Y = htables_symbols[DC][Y];
//     uint8_t nb_symbols_DC_Y = htables_nb_symbols[DC][Y];
//     struct huff_table *table_huff_DC_Y = huffman_table_build(nb_symb_per_lengths_DC_Y, symbols_DC_Y, nb_symbols_DC_Y);
//     print_table(table_huff_DC_Y);

//     // Création de la table de Huffman pour coder les AC de Y
//     uint8_t *nb_symb_per_lengths_AC_Y = htables_nb_symb_per_lengths[AC][Y];
//     uint8_t *symbols_AC_Y = htables_symbols[AC][Y];
//     uint8_t nb_symbols_AC_Y = htables_nb_symbols[AC][Y];
//     struct huff_table *table_huff_AC_Y = huffman_table_build(nb_symb_per_lengths_AC_Y, symbols_AC_Y, nb_symbols_AC_Y);
//     // print_table(table_huff_AC_Y);

//     uint8_t *nb_bits = malloc(sizeof(uint8_t));
    
//     // for (uint8_t test = 0; test < table_huff_DC_Y->nb_symbols; test++){
//     //     uint32_t res1 = huffman_table_get_path(table_huff_DC_Y, test, nb_bits);
//     //     printf("%u\t%u\n", res1, *nb_bits);
//     // }

//     uint8_t test1 = 39, test2 = 21;
//     uint32_t res1 = huffman_table_get_path(table_huff_AC_Y, test1, nb_bits);
//     printf("%u\t%u\n", res1, *nb_bits);
//     uint32_t res2 = huffman_table_get_path(table_huff_AC_Y, test2, nb_bits);
//     printf("%u\t%u\n", res2, *nb_bits);

//     free(nb_bits);
//     huffman_table_destroy(table_huff_DC_Y);
//     huffman_table_destroy(table_huff_AC_Y);

//     return 0;
// }
