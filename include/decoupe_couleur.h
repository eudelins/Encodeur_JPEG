#ifndef DECOUPE_COULEUR_H
#define DECOUPE_COULEUR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define COTE_BLOC 8


/**************************************************************/
/* Module de découpage de l'image couleur en MCUs et en blocs */
/**************************************************************/


/* Défini un pixel en RGB */
struct Pixel_RGB {
  uint8_t R;
  uint8_t G;
  uint8_t B;
};


/* Défini un bloc */
struct Bloc_RGB {
    struct Pixel_RGB **pixels;
};


/* Défini une MCU */
struct MCU_RGB {
    uint8_t h1;
    uint8_t v1;
    uint8_t h2;
    uint8_t v2;
    uint8_t h3;
    uint8_t v3;
    struct Pixel_RGB **pixels;
    struct Bloc_RGB **blocs;
};


/* Calcule le nombre de MCUs dans l'image */
uint32_t *calcul_dimensions_MCUs_RGB(uint32_t largeur_image,
                                     uint32_t hauteur_image,
                                     uint8_t h1,
                                     uint8_t v1);


/* Libère la mémoire alloué pour les pixels de l'image */
void free_pixel_image_RGB(struct Pixel_RGB **pixels,
                          uint32_t hauteur_image);


/* Découpe l'image en nb_MCUs_largeur x nb_MCUs_hauteur MCUs*/
struct MCU_RGB ***decoupage_MCUs(FILE *fichier,
                                 uint32_t largeur_image,
                                 uint32_t hauteur_image,
                                 uint32_t nb_MCUs_largeur,
                                 uint32_t nb_MCUs_hauteur,
                                 uint8_t h1,
                                 uint8_t v1,
                                 uint8_t h2,
                                 uint8_t v2,
                                 uint8_t h3,
                                 uint8_t v3);


/* Découpe une MCU en blocs (on obtient alors une MCU de taille MCU->h1 x MCU->v1) */
void decoupage_blocs(struct MCU_RGB *MCU);

/* Découpe toutes les MCU en blocs (de tailles MCU->h1 x MCU->v1) */
struct MCU_RGB ***decoupage_MCUs_en_blocs(struct MCU_RGB ***MCUs,
                                          uint32_t nb_MCUs_largeur,
                                          uint32_t nb_MCUs_hauteur);


/* Libère la mémoire allouée pour les pixels RGB d'une MCU (donc par la matrice de pixels d'une MCU) */
void free_pixel_RGB(struct Pixel_RGB **pixels,
                    uint8_t v1);


/* Libère la mémoire allouée pour les blocs d'une MCU (donc par la matrice de blocs d'une MCU) */
void free_blocs_RGB(struct Bloc_RGB **blocs,
                    uint8_t h1,
                    uint8_t v1);


/* Libère la mémoire allouée aux MCUs (donc par la matrice de MCUs) */
void free_MCUs_dims_RGB(struct MCU_RGB ***MCUs,
                        uint32_t* dimensions_MCUs);


/* Affiche un pixel */
void print_pixel_RGB(struct Pixel_RGB pixel);


/* Affiche un bloc */
void print_bloc_RGB(struct Bloc_RGB bloc);


/* Affiche une MCU */
void print_MCU_RGB(struct MCU_RGB *MCU);


/* Affiche les MCUs */
void print_MCUs_RGB(struct MCU_RGB ***MCUs,
                    uint32_t *dimensions_MCUs);

#endif /* DECOUPE_COULEUR_H */
