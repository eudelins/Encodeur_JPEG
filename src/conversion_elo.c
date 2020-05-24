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
    uint8_t largeur_MCU = MCU->h1;
    uint8_t hauteur_MCU = MCU->v1;

    struct MCU_YCbCr *nouvelle_MCU = malloc(sizeof(struct MCU_YCbCr));
    nouvelle_MCU->h1 = largeur_MCU;
    nouvelle_MCU->v1 = hauteur_MCU;

    struct Bloc_YCbCr **blocs_Y = malloc(hauteur_MCU * sizeof(struct Bloc_YCbCr*));
    struct Bloc_YCbCr **blocs_Cb = malloc(hauteur_MCU * sizeof(struct Bloc_YCbCr*));
    struct Bloc_YCbCr **blocs_Cr = malloc(hauteur_MCU * sizeof(struct Bloc_YCbCr*));

    for (uint8_t hauteur = 0; hauteur < hauteur_MCU; hauteur++) {
        struct Bloc_YCbCr *ligne_blocs_Y = malloc(largeur_MCU * sizeof(struct Bloc_YCbCr));
        struct Bloc_YCbCr *ligne_blocs_Cb = malloc(largeur_MCU * sizeof(struct Bloc_YCbCr));
        struct Bloc_YCbCr *ligne_blocs_Cr = malloc(largeur_MCU * sizeof(struct Bloc_YCbCr));

        for (uint8_t largeur = 0; largeur < largeur_MCU; largeur++) {
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
    uint8_t largeur_MCU = MCU_YCbCr_a_afficher->h1;
    uint8_t hauteur_MCU = MCU_YCbCr_a_afficher->v1;
    for (uint8_t hauteur = 0; hauteur < hauteur_MCU; hauteur++) {
        for (uint8_t largeur = 0; largeur < largeur_MCU; largeur++) {
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
                     uint8_t largeur_MCU,
                     uint8_t hauteur_MCU)
{
    for (uint8_t hauteur = 0; hauteur < hauteur_MCU; hauteur++) {
        for (uint8_t largeur = 0; largeur < largeur_MCU; largeur++) {
                free_pixels_YCbCr(blocs[hauteur][largeur].pixels);
        }
        free(blocs[hauteur]);
    }
    free(blocs);
}



/* Libère la mémoire allouée par une matrice de MCUs YCbCr */
void free_MCUs_YCbCr(struct MCU_YCbCr ***matrice_MCUs_converti,
                     uint32_t *dimensions_MCUs,
                     uint8_t largeur_MCU,
                     uint8_t hauteur_MCU)
{
    uint32_t nb_MCUs_largeur = dimensions_MCUs[0];
    uint32_t nb_MCUs_hauteur = dimensions_MCUs[1];

    for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur; hauteur++) {
        for (uint32_t largeur = 0; largeur < nb_MCUs_largeur; largeur++) {
            free_bloc_YCbCR(matrice_MCUs_converti[hauteur][largeur]->blocs_Y, largeur_MCU, hauteur_MCU);
            free_bloc_YCbCR(matrice_MCUs_converti[hauteur][largeur]->blocs_Cb, largeur_MCU, hauteur_MCU);
            free_bloc_YCbCR(matrice_MCUs_converti[hauteur][largeur]->blocs_Cr, largeur_MCU, hauteur_MCU);

            free(matrice_MCUs_converti[hauteur][largeur]);
        }
        free(matrice_MCUs_converti[hauteur]);
    }
    free(matrice_MCUs_converti);
}




int main()
{
    FILE *fichier = ouvrir_fichier("images/invadered.ppm", "r");

    // On récupère l'en-tête (P5 ou P6)
    char en_tete[10];
    fgets(en_tete, 10, fichier);

    // On récupère les dimensions de l'image
    char dimensions[30];
    uint32_t largeur_image, hauteur_image;
    fgets(dimensions, 30, fichier);
    sscanf(dimensions, "%u %u", &largeur_image, &hauteur_image);

    // On calcule les dimensions des MCUs
    uint8_t largeur_MCU = 1;
    uint8_t hauteur_MCU = 1;
    uint32_t *dimensions_MCUs = calcul_dimensions_MCUs(largeur_image, hauteur_image, largeur_MCU, hauteur_MCU);
    uint32_t nb_MCUs_hauteur, nb_MCUs_largeur;
    nb_MCUs_largeur = dimensions_MCUs[0];
    nb_MCUs_hauteur = dimensions_MCUs[1];

    struct MCU_RGB ***MCUs = decoupage_MCUs(fichier, largeur_image, hauteur_image, nb_MCUs_largeur, nb_MCUs_hauteur, largeur_MCU, hauteur_MCU);
    MCUs = decoupage_MCUs_en_blocs(MCUs, nb_MCUs_largeur, nb_MCUs_hauteur, largeur_MCU, hauteur_MCU);
    struct MCU_YCbCr ***matrice_MCUs_converti = conversion_matrice_MCUs(MCUs, nb_MCUs_largeur, nb_MCUs_hauteur);

    print_MCUs(MCUs, dimensions_MCUs, largeur_MCU, hauteur_MCU);
    print_matrice_MCU_YCbCr(matrice_MCUs_converti, nb_MCUs_largeur, nb_MCUs_hauteur);

    free_MCUs_YCbCr(matrice_MCUs_converti, dimensions_MCUs, largeur_MCU, hauteur_MCU);
    free_MCUs_dims(MCUs, dimensions_MCUs, largeur_MCU, hauteur_MCU);

    fermer_fichier(fichier);
    return 0;

//    uint8_t MCU_RGB [0][0] = {  {0xb8a092, 0xb19b8d, 0xa79787, 0xa29785, 0xa19a88, 0xa19a88, 0x9e9586, 0x9e8d83},
//                                {0xb29b8d, 0xb49e90, 0xaa9a8a, 0x988d7b, 0x8f8876, 0x968f7d, 0x9d9485, 0x9f9184},
//                                {0xb6a092, 0xa69384, 0xa39384, 0xab9f8f, 0xa69f8d, 0x97907e, 0x979080, 0xa99b8e},
//                                {0xb7a194, 0xa99688, 0x9c8f7f, 0x99907f, 0x999282, 0x979080, 0x979080, 0x9e9083},
//                                {0xdac4b7, 0xe8d5c7, 0xd3c5b8, 0xa09788, 0x8b8575, 0x9e9888, 0xa9a194, 0x9f9387},
//                                {0xffebe0, 0xfff3e7, 0xfff9ec, 0xf2e9da, 0xc5bfaf, 0x9e9888, 0x969082, 0xa79b8f},
//                                {0xf8e4d9, 0xefddd1, 0xfdf1e3, 0xffffef, 0xf8f2e4, 0xb5afa1, 0x908a7c, 0x9a8e82},
//                                {0xf5e1d8, 0xffeee4, 0xfbefe3, 0xf4ecdf, 0xfef8ea, 0xfffbed, 0xd6d0c2, 0x9f9387}  };
//
}
