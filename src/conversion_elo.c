#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


/* Fonction qui prend en entrée un pixel RGB et qui renvoie la valeur de la luminance Y */
int8_t *conversion_Y(struct Pixel_RGB *pixelRGB)
{
  int8_t *valeur_Y = malloc(sizeof(int8_t));
  valeur_Y = 0.299 * pixelRGB->R + 0.587 * pixelRGB->G + 0.114 * pixelRGB->B;
  return valeur_Y;
}


/* Fonction qui prend en entrée un pixel RGB et qui renvoie la valeur de la chrominance Cb */
int8_t *conversion_Cb(struct Pixel_RGB *pixelRGB)
{
    int8_t *valeur_Cb = malloc(sizeof(int8_t));
    float calcul_Cb = 128 - 0.1687 * pixelRGB->R - 0.3313 * pixelRGB->G + 0.5 * pixelRGB->B;
    valeur_Cb = (int8_t) calcul;  // cas où R=0, G=0, B=255 -> Cb = 255.5
    return valeur_Cb;
}


/* Fonction qui prend en entrée un pixel RGB et qui renvoie la valeur de la luminance Cr */
int8_t *conversion_Cr(struct Pixel_RGB *pixelRGB)
{
    int8_t *valeur_Cr(malloc(sizeof(int8_t)));
    float calcul = 128 + 0.5 * pixelRGB->R - 0.4187 * pixelRGB->G - 0.0813 * pixelRGB->B;  // cas où R=0, G=0, B=255 -> Cr = 255.5
    valeur_Cr = (int8_t) calcul;
    return valeur_Cr;

}


/* Structure d'un bloc après conversion YCbCr */
struct Bloc_converti {
    int8_t **pixels;
}


/* Structure d'une MCU_YCbCr */
struct MCU_YCbCr {
    uint8_t largeur;
    uint8_t hauteur;
    struct Bloc_converti **blocs_Y;
    struct Bloc_converti **blocs_Cb;
    struct Bloc_converti **blocs_Cr;
}


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
struct MCU_YCbCr *conversion_MCU(struct MCU ***MCU)
{
    largeur_MCU = MCU->largeur;
    hauteur_MCU = MCU->hauteur;
    struct MCU_YCbCr *nouvelle_MCU = malloc(sizeof(struct MCU_YCbCr));
    nouvelle_MCU->largeur = largeur_MCU;
    nouvelle_MCU->hauteur = hauteur_MCU;

    struct Bloc_converti ***blocs_Y = malloc(sizeof(Bloc_converti**));
    struct Bloc_converti ***blocs_Cb = malloc(sizeof(Bloc_converti**));
    struct Bloc_converti ***blocs_Cr = malloc(sizeof(Bloc_converti**));

    for (uint8_t hauteur = 0; hauteur < hauteur_MCU; hauteur++) {
        struct Bloc_converti **ligne_blocs_Y = malloc(COTE_BLOC * sizeof(struct Bloc_converti));
        struct Bloc_converti **ligne_blocs_Cb = malloc(COTE_BLOC * sizeof(struct Bloc_converti));
        struct Bloc_converti **ligne_blocs_Cr = malloc(COTE_BLOC * sizeof(struct Bloc_converti));

        for (uint8_t largeur = 0; largeur < largeur_MCU; largeur++) {
            struct Bloc_converti *bloc_Y = malloc(sizeof(struct Bloc_converti));
            struct Bloc_converti *bloc_Cb = malloc(sizeof(struct Bloc_converti));
            struct Bloc_converti *bloc_Cr = malloc(sizeof(struct Bloc_converti));
            
            uint8_t **pixels_Y = malloc(COTE_BLOC * sizeof(uint8_t));
            uint8_t **pixels_Cb = malloc(COTE_BLOC * sizeof(uint8_t));
            uint8_t **pixels_Cr = malloc(COTE_BLOC * sizeof(uint8_t));

            for (uint8_t hauteur_pix = 0; hauteur_pix < COTE_BLOC; hauteur_pix++) {
                struct uint8_t *ligne_pixels_Y = malloc(sizeof(uint8_t));
                struct uint8_t *ligne_pixels_Cb = malloc(sizeof(uint8_t));
                struct uint8_t *ligne_pixels_Cr = malloc(sizeof(uint8_t));

                for (uint8_t largeur_pix = 0; largeur_pix < COTE_BLOC; largeur_pix++) {
                    ligne_pixels_Y[largeur_pix] = conversion_Y(MCU->blocs[hauteur][largeur].pixels[hauteur_pix][largeur_pix]);
                    ligne_pixels_Cb[largeur_pix] = conversion_Cb(MCU->blocs[hauteur][largeur].pixels[hauteur_pix][largeur_pix]);
                    ligne_pixels_Cr[largeur_pix] = conversion_Cr(MCU->blocs[hauteur][largeur].pixels[hauteur_pix][largeur_pix]);
                }
                pixels_Y[hauteur_pix] = ligne_blocs_Y;
                pixels_Cb[hauteur_pix] = ligne_blocs_Cb;
                pixels_Cr[hauteur_pix] = ligne_blocs_Cr;
            }
            bloc_Y->pixels_Y = pixels_Y;
            bloc_Cb->pixels_Cb = pixels_Cb;
            bloc_Cr->pixels_Cr = pixels_Cr;

            ligne_blocs_Y[largeur] = bloc_Y;
            ligne_blocs_Cb[largeur] = bloc_Cb;
            ligne_blocs_Cr[largeur] = bloc_Cr;
        }
        blocs_Y[hauteur] = ligne_blocs_Y;
        blocs_Cb[hauteur] = ligne_blocs_Cb;
        blocs_Cr[hauteur] = ligne_blocs_Cr;
    }
    nouvelle_MCU->blocs_Y = blocs_Y;
    nouvelle_MCU->blocs_Cb = blocs_Cb;
    nouvelle_MCU->blocs_Cr = blocs_Cr;
}


int main() 
{
    struct MCU ***MCU = malloc(2 * COTE_BLOC * sizeof(struct MCU));
    MCU->largeur = 2;
    MCU->hauteur = 2;
    
}
