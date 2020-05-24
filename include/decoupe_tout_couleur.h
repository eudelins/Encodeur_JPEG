#ifndef DECOUPE_TOUT_COULEUR_H
#define DECOUPE_TOUT_COULEUR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define COTE_BLOC 8


/* Ouvre le fichier filename avec le mode d'accès mode. Retourne le FILE *
 * correspondant. */
FILE *ouvrir_fichier(const char *filename,
                     const char *mode);


/* Ferme le fichier passé en paramètre. */
void fermer_fichier(FILE *fichier);


/*Défini un pixel en RGB*/
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
    uint8_t largeur;
    uint8_t hauteur;
    struct Pixel_RGB **pixels;
    struct Bloc_RGB **blocs;
};


/* Calcule le nombre de MCUs dans l'image */
uint32_t *calcul_dimensions_MCUs_RGB(uint32_t largeur_image,
                                 uint32_t hauteur_image,
                                 uint8_t largeur_MCU,
                                 uint8_t hauteur_MCU);


/* Libère la mémoire alloué pour les pixels de l'image */
void free_pixel_image_RGB(struct Pixel_RGB **pixels,
                      uint32_t hauteur_image);


/* Découpe l'image en MCUs : nb_MCUs_largeur x nb_MCUs_hauteur */
struct MCU_RGB ***decoupage_MCUs(FILE *fichier,
                             uint32_t largeur_image,
                             uint32_t hauteur_image,
                             uint32_t nb_MCUs_largeur,
                             uint32_t nb_MCUs_hauteur,
                             uint32_t largeur_MCU,
                             uint32_t hauteur_MCU);


/* Découpe une MCU en blocs pour une MCU de taille largeur_MCU x hauteur_MCU */
void decoupage_blocs(struct MCU_RGB *MCU,
                     uint8_t largeur_MCU,
                     uint8_t hauteur_MCU);


/* Découpe toutes les MCU en blocs de tailles largeur_MCU x hauteur_MCU */
struct MCU_RGB ***decoupage_MCUs_en_blocs(struct MCU_RGB ***MCUs,
                                      uint32_t nb_MCUs_largeur,
                                      uint32_t nb_MCUs_hauteur,
                                      uint8_t largeur_MCU,
                                      uint8_t hauteur_MCU);


/* Libère la mémoire allouée pour les pixels RGB d'une MCU (donc par la matrice de pixels) */
void free_pixel_RGB(struct Pixel_RGB **pixels);


/* Libère la mémoire allouée aux bloc */
void free_blocs_RGB(struct Bloc_RGB **blocs,
                uint8_t largeur_MCU,
                uint8_t hauteur_MCU);


/* Libère la mémoire allouée aux MCUs */
void free_MCUs_dims_RGB(struct MCU_RGB ***MCUs,
                    uint32_t* dimensions_MCUs,
                    uint8_t largeur_MCU,
                    uint8_t hauteur_MCU);


/* Affiche un pixel */
void print_pixel_RGB(struct Pixel_RGB pixel);


/* Affiche un bloc */
void print_bloc_RGB(struct Bloc_RGB bloc);


/* Affiche une MCU */
void print_MCU_RGB(struct MCU_RGB *MCU,
               uint8_t largeur_MCU,
               uint8_t hauteur_MCU);


/* Affiche les MCUs */
void print_MCUs_RGB(struct MCU_RGB ***MCUs,
                uint32_t *dimensions_MCUs,
                uint8_t largeur_MCU,
                uint8_t hauteur_MCU);

#endif /* DECOUPE_TOUT_COULEUR_H */
