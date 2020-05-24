#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../include/decoupe_tout_couleur.h"


/* Fonction qui prend en entrée un pixel RGB et qui renvoie la valeur de la luminance Y */
int16_t conversion_Y(struct Pixel_RGB pixelRGB)
{
  float calcul_Y = 0.299 * pixelRGB.R + 0.587 * pixelRGB.G + 0.114 * pixelRGB.B;
  int16_t valeur_Y = (int16_t) calcul_Y;
  return valeur_Y;
}


/* Fonction qui prend en entrée un pixel RGB et qui renvoie la valeur de la chrominance Cb */
int16_t conversion_Cb(struct Pixel_RGB pixelRGB)
{
    float calcul_Cb = 128 - 0.1687 * pixelRGB.R - 0.3313 * pixelRGB.G + 0.5 * pixelRGB.B;
    int16_t valeur_Cb = (int16_t) calcul_Cb;
    return valeur_Cb;
}


/* Fonction qui prend en entrée un pixel RGB et qui renvoie la valeur de la luminance Cr */
int16_t conversion_Cr(struct Pixel_RGB pixelRGB)
{
    float calcul_Cr = 128 + 0.5 * pixelRGB.R - 0.4187 * pixelRGB.G - 0.0813 * pixelRGB.B;
    int16_t valeur_Cr = (int16_t) calcul_Cr;
    return valeur_Cr;
}


/* Structure d'un bloc après conversion YCbCr */
struct Bloc_YCbCr {
    int16_t **pixels;
};


/* Structure d'une MCU_YCbCr */
struct MCU_YCbCr {
    uint8_t h1;
    uint8_t v1;
    uint8_t h2;
    uint8_t v2;
    uint8_t h3;
    uint8_t v3;
    struct Bloc_YCbCr **blocs_Y;
    struct Bloc_YCbCr **blocs_Cb;
    struct Bloc_YCbCr **blocs_Cr;
};


/* Fonction qui prend en entrée une MCU qui a pour arguments :
 * - largeur de la MCU
 * - hauteur de la MCU
 * - matrice de pixels RGB
 * - matrice de blocs de matrices de pixels RGB
 * Elle retourne une MCU qui a pour arguments :
 * - largeur de la MCU
 * - hauteur de la MCU
 * - matrice de blocs de matrices de pixels Y
 * - matrice de blocs de matrices de pixels Cb
 * - matrice de blocs de matrices de pixels Cr */
struct MCU_YCbCr *conversion_MCU(struct MCU_RGB *MCU)
{

    struct MCU_YCbCr *nouvelle_MCU = malloc(sizeof(struct MCU_YCbCr));
    nouvelle_MCU->h1 = MCU->h1;
    nouvelle_MCU->v1 = MCU->v1;

    nouvelle_MCU->h2 = MCU->h2;
    nouvelle_MCU->v2 = MCU->v2;

    nouvelle_MCU->h3 = MCU->h3;
    nouvelle_MCU->v3 = MCU->v3;

    struct Bloc_YCbCr **blocs_Y = malloc(MCU->v1 * sizeof(struct Bloc_YCbCr*));
    struct Bloc_YCbCr **blocs_Cb = malloc(MCU->v1 * sizeof(struct Bloc_YCbCr*));
    struct Bloc_YCbCr **blocs_Cr = malloc(MCU->v1 * sizeof(struct Bloc_YCbCr*));

    for (uint8_t hauteur = 0; hauteur < MCU->v1; hauteur++) {
        struct Bloc_YCbCr *ligne_blocs_Y = malloc(MCU->h1 * sizeof(struct Bloc_YCbCr));
        struct Bloc_YCbCr *ligne_blocs_Cb = malloc(MCU->h1 * sizeof(struct Bloc_YCbCr));
        struct Bloc_YCbCr *ligne_blocs_Cr = malloc(MCU->h1 * sizeof(struct Bloc_YCbCr));

        for (uint8_t largeur = 0; largeur < MCU->h1; largeur++) {
            int16_t **pixels_Y = malloc(COTE_BLOC * sizeof(int16_t*));
            int16_t **pixels_Cb = malloc(COTE_BLOC * sizeof(int16_t*));
            int16_t **pixels_Cr = malloc(COTE_BLOC * sizeof(int16_t*));

            for (uint8_t hauteur_pix = 0; hauteur_pix < COTE_BLOC; hauteur_pix++) {
                int16_t *ligne_pixels_Y = malloc(COTE_BLOC * sizeof(int16_t));
                int16_t *ligne_pixels_Cb = malloc(COTE_BLOC * sizeof(int16_t));
                int16_t *ligne_pixels_Cr = malloc(COTE_BLOC * sizeof(int16_t));

                for (uint8_t largeur_pix = 0; largeur_pix < COTE_BLOC; largeur_pix++) {
                    ligne_pixels_Y[largeur_pix] = conversion_Y(MCU->blocs[hauteur][largeur].pixels[hauteur_pix][largeur_pix]);
                    ligne_pixels_Cb[largeur_pix] = conversion_Cb(MCU->blocs[hauteur][largeur].pixels[hauteur_pix][largeur_pix]);
                    ligne_pixels_Cr[largeur_pix] = conversion_Cr(MCU->blocs[hauteur][largeur].pixels[hauteur_pix][largeur_pix]);
                }
                pixels_Y[hauteur_pix] = ligne_pixels_Y;
                pixels_Cb[hauteur_pix] = ligne_pixels_Cb;
                pixels_Cr[hauteur_pix] = ligne_pixels_Cr;
            }
            ligne_blocs_Y[largeur].pixels = pixels_Y;
            ligne_blocs_Cb[largeur].pixels = pixels_Cb;
            ligne_blocs_Cr[largeur].pixels = pixels_Cr;
        }
        blocs_Y[hauteur] = ligne_blocs_Y;
        blocs_Cb[hauteur] = ligne_blocs_Cb;
        blocs_Cr[hauteur] = ligne_blocs_Cr;
    }
    nouvelle_MCU->blocs_Y = blocs_Y;
    nouvelle_MCU->blocs_Cb = blocs_Cb;
    nouvelle_MCU->blocs_Cr = blocs_Cr;

    return nouvelle_MCU;
}


/* Fonction qui fait la conversion d'une matrice de MCU_RGB et une matrice de MCU_YCbCr */
struct MCU_YCbCr ***conversion_matrice_MCUs(struct MCU_RGB ***MCU_a_convertir,
                                            uint32_t nb_MCUs_largeur,
                                            uint32_t nb_MCUs_hauteur)
{
    struct MCU_YCbCr ***matrice_MCUs_converti = malloc(nb_MCUs_hauteur * sizeof(struct MCU_YCbCr**));
    for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur; hauteur++) {
        struct MCU_YCbCr **ligne_MCUs_converti = malloc(nb_MCUs_largeur * sizeof(struct MCU_YCbCr*));
        for (uint32_t largeur = 0; largeur < nb_MCUs_largeur; largeur++) {
            ligne_MCUs_converti[largeur] = conversion_MCU(MCU_a_convertir[hauteur][largeur]);
        }
        matrice_MCUs_converti[hauteur] = ligne_MCUs_converti;
    }
    return matrice_MCUs_converti;
}


/* Affiche un bloc d'entiers */
void print_bloc_entiers(struct Bloc_YCbCr *bloc_a_afficher)
{
    for (uint8_t hauteur = 0; hauteur < COTE_BLOC; hauteur++) {
        for (uint8_t largeur = 0; largeur < COTE_BLOC; largeur++) {
            printf("%i ", bloc_a_afficher->pixels[hauteur][largeur]);
        }
        printf("\n");
    }
}


/* Affiche une MCU_YCbCr */
void print_MCU_YCbCr(struct MCU_YCbCr *MCU_YCbCr_a_afficher)
{
    uint8_t h1 = MCU_YCbCr_a_afficher->h1;
    uint8_t v1 = MCU_YCbCr_a_afficher->v1;
    for (uint8_t hauteur = 0; hauteur < v1; hauteur++) {
        for (uint8_t largeur = 0; largeur < h1; largeur++) {
            printf("Bloc Y n°%u\n", largeur + hauteur);
            print_bloc_entiers(&MCU_YCbCr_a_afficher->blocs_Y[hauteur][largeur]);
            printf("\n");
            printf("Bloc Cb n°%u\n", largeur + hauteur);
            print_bloc_entiers(&MCU_YCbCr_a_afficher->blocs_Cb[hauteur][largeur]);
            printf("\n");
            printf("Bloc Cr n°%u\n", largeur + hauteur);
            print_bloc_entiers(&MCU_YCbCr_a_afficher->blocs_Cr[hauteur][largeur]);
            printf("\n");
        }
    }
}


/* Affiche la matrice de MCU_YCbCr */
void print_matrice_MCU_YCbCr(struct MCU_YCbCr ***MCUs_YCbCr_a_afficher,
                             uint32_t nb_MCUs_largeur,
                             uint32_t nb_MCUs_hauteur)
{
    for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur; hauteur++) {
        for (uint32_t largeur = 0; largeur < nb_MCUs_largeur; largeur++) {
            printf("MCU n°%u\n\n", largeur + hauteur);
            print_MCU_YCbCr(MCUs_YCbCr_a_afficher[hauteur][largeur]);
            printf("\n");
        }
    }
}


/* Libère la mémoire allouée par une matrice d'entiers */
void free_pixels_YCbCr(int16_t **pixels)
{
    for (uint8_t hauteur_pix = 0; hauteur_pix < COTE_BLOC; hauteur_pix++) {
        free(pixels[hauteur_pix]);
    }
    free(pixels);
}

/* Libère la mémoire allouée par une matrice de blocs YCbCr */
void free_bloc_YCbCR(struct Bloc_YCbCr **blocs,
                     uint8_t h1,
                     uint8_t v1)
{
    for (uint8_t hauteur = 0; hauteur < v1; hauteur++) {
        for (uint8_t largeur = 0; largeur < h1; largeur++) {
                free_pixels_YCbCr(blocs[hauteur][largeur].pixels);
        }
        free(blocs[hauteur]);
    }
    free(blocs);
}



/* Libère la mémoire allouée par une matrice de MCUs YCbCr */
void free_MCUs_YCbCr(struct MCU_YCbCr ***matrice_MCUs_converti,
                     uint32_t *dimensions_MCUs)
{
    uint32_t nb_MCUs_largeur = dimensions_MCUs[0];
    uint32_t nb_MCUs_hauteur = dimensions_MCUs[1];

    for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur; hauteur++) {
        for (uint32_t largeur = 0; largeur < nb_MCUs_largeur; largeur++) {
            uint8_t h1 = matrice_MCUs_converti[hauteur][largeur]->h1;
            uint8_t v1 = matrice_MCUs_converti[hauteur][largeur]->v1;
            free_bloc_YCbCR(matrice_MCUs_converti[hauteur][largeur]->blocs_Y, h1, v1);
            free_bloc_YCbCR(matrice_MCUs_converti[hauteur][largeur]->blocs_Cb, h1, v1);
            free_bloc_YCbCR(matrice_MCUs_converti[hauteur][largeur]->blocs_Cr, h1, v1);

            free(matrice_MCUs_converti[hauteur][largeur]);
        }
        free(matrice_MCUs_converti[hauteur]);
    }
    free(matrice_MCUs_converti);
}



int main()
{
    FILE *fichier = fopen("images/invadered.ppm", "r");

    // On récupère l'en-tête (P5 ou P6)
    char en_tete[10];
    fgets(en_tete, 10, fichier);

    // On récupère les dimensions de l'image
    char dimensions[30];
    uint32_t largeur_image, hauteur_image;
    fgets(dimensions, 30, fichier);
    sscanf(dimensions, "%u %u", &largeur_image, &hauteur_image);

    // On saute une ligne
    char couleurs_max[10];
    fgets(couleurs_max, 10, fichier);

    // On calcule les dimensions des MCUs
    uint8_t h1 = 1;
    uint8_t v1 = 1;
    uint8_t h2 = 1;
    uint8_t v2 = 1;
    uint8_t h3 = 1;
    uint8_t v3 = 1;

    uint32_t *dimensions_MCUs = calcul_dimensions_MCUs_RGB(largeur_image, hauteur_image, h1, v1);
    uint32_t nb_MCUs_hauteur, nb_MCUs_largeur;
    nb_MCUs_largeur = dimensions_MCUs[0];
    nb_MCUs_hauteur = dimensions_MCUs[1];

    struct MCU_RGB ***MCUs = decoupage_MCUs(fichier, largeur_image, hauteur_image, nb_MCUs_largeur, nb_MCUs_hauteur, h1, v1, h2, v2, h3, v3);
    MCUs = decoupage_MCUs_en_blocs(MCUs, nb_MCUs_largeur, nb_MCUs_hauteur, h1, v1);
    struct MCU_YCbCr ***matrice_MCUs_converti = conversion_matrice_MCUs(MCUs, nb_MCUs_largeur, nb_MCUs_hauteur);

    print_MCUs_RGB(MCUs, dimensions_MCUs);
    print_matrice_MCU_YCbCr(matrice_MCUs_converti, nb_MCUs_largeur, nb_MCUs_hauteur);

    free_MCUs_YCbCr(matrice_MCUs_converti, dimensions_MCUs);
    free_MCUs_dims_RGB(MCUs, dimensions_MCUs);

    fclose(fichier);
    return 0;
}

