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


/* Calcule le nombre de MCUs dans l'image */
uint32_t *calcul_dimensions_MCUs(uint32_t largeur_image,
                                 uint32_t hauteur_image,
                                 uint8_t largeur_MCU,
                                 uint8_t hauteur_MCU);


/* Libère la mémoire alloué pour les pixels de l'image */
void free_pixel_image(struct Pixel_RGB **pixels,
                      uint32_t hauteur_image);


/* Découpe l'image en MCUs : nb_MCUs_largeur x nb_MCUs_hauteur */
struct MCU ***decoupage_MCUs(FILE *fichier,
                             uint32_t largeur_image,
                             uint32_t hauteur_image,
                             uint32_t nb_MCUs_largeur,
                             uint32_t nb_MCUs_hauteur,
                             uint32_t largeur_MCU,
                             uint32_t hauteur_MCU);


/* Découpe une MCU en blocs pour une MCU de taille largeur_MCU x hauteur_MCU */
void decoupage_blocs(struct MCU *MCU,
                     uint8_t largeur_MCU,
                     uint8_t hauteur_MCU);


/* Découpe toutes les MCU en blocs de tailles largeur_MCU x hauteur_MCU */
struct MCU ***decoupage_MCUs_en_blocs(struct MCU ***MCUs,
                                      uint32_t nb_MCUs_largeur,
                                      uint32_t nb_MCUs_hauteur,
                                      uint8_t largeur_MCU,
                                      uint8_t hauteur_MCU);


/* Libère la mémoire allouée pour les pixels RGB d'une MCU (donc par la matrice de pixels) */
void free_pixel(struct Pixel_RGB **pixels);


/* Libère la mémoire allouée aux bloc */
void free_blocs(struct Bloc **blocs,
                uint8_t largeur_MCU,
                uint8_t hauteur_MCU);


/* Libère la mémoire allouée aux MCUs */
void free_MCUs_dims(struct MCU ***MCUs,
                    uint32_t* dimensions_MCUs,
                    uint8_t largeur_MCU,
                    uint8_t hauteur_MCU);


/* Affiche un pixel */
void print_pixel(struct Pixel_RGB pixel);


/* Affiche un bloc */
void print_bloc(struct Bloc bloc);


/* Affiche une MCU */
void print_MCU(struct MCU *MCU,
               uint8_t largeur_MCU,
               uint8_t hauteur_MCU);


/* Affiche les MCUs */
void print_MCUs(struct MCU ***MCUs,
                uint32_t *dimensions_MCUs,
                uint8_t largeur_MCU,
                uint8_t hauteur_MCU);


