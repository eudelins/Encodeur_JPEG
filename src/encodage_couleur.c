#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "../include/decoupe_tout.h"
#include "../include/qtables.h"

#define M_PI 3.14159265358979323846



/******************************/
/* Module d'encodage des MCUs */
/******************************/


/* Défini un bloc */
struct Bloc {
    uint8_t **pixels;      // un Bloc = une matrice de pixels
};


/* Défini une MCU */
struct MCU {
    uint8_t largeur;
    uint8_t hauteur;
    struct Pixel_RGB **pixels;      // une MCU = une matrice de pixels
    struct Bloc **blocs;            // une MCU = une matrice de blocs
};


/* MCU après conversion en YCbCr */
struct MCU_Y{
    uint8_t largeur;
    uint8_t hauteur;
    struct Bloc **blocs_Y;
    struct Bloc **blocs_Cb;
    struct Bloc **blocs_Cr;
};


/*******************************************/
/* Partie consacrée au sous-echantillonage */
/*******************************************/


struct MCU_Y *sous_echantillonage_horizontal(struct MCU_Y *MCU_Y,
                                  uint8_t h1,
                                  uint8_t v1,
                                  uint8_t h2,
                                  uint8_t v2,
                                  uint8_t h3,
                                  uint8_t v3)
{
  struct Bloc **sous_blocs_Cb = malloc(v2 * sizeof(struct Bloc*));
  for (uint8_t hauteur = 0; hauteur < v2; hauteur++) {
    struct Bloc *sous_ligne_Cb = malloc(h2 * sizeof(struct Bloc));
    for (uint8_t largeur = 0; largeur < h2; largeur++) {



      /* On fait la moyenne des pixels */
      uint8_t **pixels_Cb = malloc(8 * sizeof(uint8_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur++) {
        uint8_t *ligne_pixels_Cb = malloc(8 * sizeof(uint8_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          ligne_pixels_Cb[largeur_pix] = (MCU_Y->blocs_Cb[hauteur][largeur_pix/4]->pixels[hauteur_pix][2 * largeur_pix] + MCU_Y->blocs_Cb[hauteur][largeur_pix/4]->pixels[hauteur_pix][2 * largeur_pix + 1])/2;
        }
        pixels_Cb[hauteur_pix] = ligne_pixels_Cb;
      }
      
    }
  }
}


/* Calcul C(i) */
double C(uint8_t i)
{
    if (i == 0) {
        return 0.707106781186547;
    } else {
        return 1.0;
    }
}


/* Applique la transformée en cosinus discrète à une MCU */
struct MCU_freq *transf_cos_MCU(struct MCU *MCU)
{
    struct MCU_freq *MCU_freq = malloc(sizeof(struct MCU_freq));
    MCU_freq->largeur = MCU->largeur;
    MCU_freq->hauteur = MCU->hauteur;

    int16_t **pixels_freq = malloc(MCU_freq->hauteur * sizeof(int16_t *));
    for (uint8_t i = 0; i < MCU_freq->hauteur; i++){
        int16_t *ligne_pixels_freq = malloc(MCU_freq->largeur * sizeof(int16_t));

        for (uint8_t j = 0; j < MCU_freq->largeur; j++){

            // Calcul de la coordonnées (i, j) du domaine frequentiel
            double freq = 0.0;
            for (uint8_t x = 0; x < MCU_freq->hauteur; x++){
                for (uint8_t y = 0; y < MCU_freq->largeur; y++){
                    freq += (MCU->pixels[x][y] - 128) * cos((2*x + 1)*i*M_PI/16) * cos((2*y + 1)*j*M_PI/16);
                }
            }
            freq = C(i) * C(j) * freq/4.0;

            ligne_pixels_freq[j] = (int16_t) freq;
        }
        pixels_freq[i] = ligne_pixels_freq;
    }
    MCU_freq->pixels_freq = pixels_freq;

    return MCU_freq;
}


/* Applique la transformée en cosinus discrète aux MCUs */
struct MCU_freq ***transf_cos(struct MCU ***MCUs, uint32_t largeur_MCUs, uint32_t hauteur_MCUs)
{
    struct MCU_freq ***MCUs_freq = malloc(hauteur_MCUs * sizeof(struct MCU_freq**));
    for (uint32_t hauteur = 0; hauteur < hauteur_MCUs; hauteur++){
        struct MCU_freq **ligne_MCUs_freq = malloc(largeur_MCUs * sizeof(struct MCU_freq*));
        for (uint32_t largeur = 0; largeur < largeur_MCUs; largeur++){
            struct MCU_freq *MCU_freq = transf_cos_MCU(MCUs[hauteur][largeur]);
            ligne_MCUs_freq[largeur] = MCU_freq;
        }
        MCUs_freq[hauteur] = ligne_MCUs_freq;
    }
    return MCUs_freq;
}


/* Affiche une MCU transformée */
void print_MCU_freq(struct MCU_freq *MCU_freq)
{
    uint8_t largeur_MCU = 8, hauteur_MCU = 8;
    int16_t **pixels_freq = MCU_freq->pixels_freq;
    for (uint8_t hauteur_pix = 0; hauteur_pix < hauteur_MCU; hauteur_pix++){
        for (uint8_t largeur_pix = 0; largeur_pix < largeur_MCU; largeur_pix++){
            printf("%hx\t", pixels_freq[hauteur_pix][largeur_pix]);
        }
        printf("\n");
    }
}


/* Affiche les MCUs transformées */
void print_MCUs_freq(struct MCU_freq ***MCUs_freq, uint32_t largeur_MCUs, uint32_t hauteur_MCUs)
{
    for (uint32_t hauteur = 0; hauteur < hauteur_MCUs; hauteur++){
        for (uint32_t largeur =  0; largeur < largeur_MCUs; largeur++){
            printf("MCU %d:\n", hauteur * largeur_MCUs + largeur);
            print_MCU_freq(MCUs_freq[hauteur][largeur]);
            printf("\n");
        }
    }
}



/* Libère la mémoire alloué pour les pixels d'une MCU fréquentielle */
void free_pixel_freq(int16_t **pixels, uint32_t hauteur_pix)
{
    for (int32_t hauteur = hauteur_pix - 1; hauteur >= 0; hauteur--){
        free(pixels[hauteur]);
    }
    free(pixels);
}


/* Libère la mémoire allouée aux MCUs fréquentielles */
void free_MCUs_freq(struct MCU_freq ***MCUs_freq, uint32_t largeur_MCUs, uint32_t hauteur_MCUs)
{
    for (int32_t hauteur = hauteur_MCUs - 1; hauteur >= 0; hauteur--){
        for (int32_t largeur = largeur_MCUs - 1; largeur >= 0; largeur--){
            free_pixel_freq(MCUs_freq[hauteur][largeur]->pixels_freq, 8);
            free(MCUs_freq[hauteur][largeur]);
        }
        free(MCUs_freq[hauteur]);
    }
    free(MCUs_freq);
}


/**********************************************************************/
/* Fin de la partie consacrée à la transformation en cosinus discrète */
/**********************************************************************/


/**************************************************/
/* Partie consacrée à la réorganisation en zigzag */
/**************************************************/

/* MCU après réorganisation en zigzag */
struct MCU_zigzag{
    uint8_t largeur;
    uint8_t hauteur;
    int16_t *pixels_zigzag;
};


/* Calcul le chemin en zigzag d'une MCU */
struct MCU_zigzag *zigzag_MCU(struct MCU_freq *MCU_freq)
{
    struct MCU_zigzag *MCU_zigzag = malloc(sizeof(struct MCU_zigzag));
    MCU_zigzag->largeur = MCU_freq->largeur;
    MCU_zigzag->hauteur = MCU_freq->hauteur;

    int16_t *zigzag = malloc(MCU_freq->largeur * MCU_freq->hauteur * sizeof(int16_t));
    uint8_t compteur_elem = 0;

    // Première partie du zigzag
    for (uint8_t i = 0; i < MCU_freq->largeur; i++){
        for (uint8_t j = 0; j <= i; j++){
            if (i % 2 == 0){
                zigzag[compteur_elem] = MCU_freq->pixels_freq[i - j][j];
                compteur_elem += 1;
            } else {
                zigzag[compteur_elem] = MCU_freq->pixels_freq[j][i - j];
                compteur_elem += 1;
            }
        }
    }

    // Deuxième partie
    for (uint8_t i = 0; i < MCU_freq->largeur; i++){
        for (uint8_t j = i + 1; j <= 7; j++){
            if (i % 2 == 0){
                zigzag[compteur_elem] = MCU_freq->pixels_freq[8 + i - j][j];
                compteur_elem += 1;
            } else {
                zigzag[compteur_elem] = MCU_freq->pixels_freq[j][8 + i - j];
                compteur_elem += 1;
            }
        }
    }
    MCU_zigzag->pixels_zigzag = zigzag;

    return MCU_zigzag;
}


/* Applique la réorganisation en zigzag aux MCUs */
struct MCU_zigzag ***zigzag(struct MCU_freq ***MCUs_freq, uint32_t largeur_MCUs, uint32_t hauteur_MCUs)
{
    struct MCU_zigzag ***MCUs_zigzag = malloc(hauteur_MCUs * sizeof(struct MCU_zigzag**));
    for (uint32_t hauteur = 0; hauteur < hauteur_MCUs; hauteur++){
        struct MCU_zigzag **ligne_MCUs_zigzag = malloc(largeur_MCUs * sizeof(struct MCU_zigzag*));
        for (uint32_t largeur = 0; largeur < largeur_MCUs; largeur++){
            struct MCU_zigzag *MCU_zigzag = zigzag_MCU(MCUs_freq[hauteur][largeur]);
            ligne_MCUs_zigzag[largeur] = MCU_zigzag;
        }
        MCUs_zigzag[hauteur] = ligne_MCUs_zigzag;
    }
    return MCUs_zigzag;
}


/* Affiche une MCU réorgansée en zigzag */
void print_MCU_zigzag(struct MCU_zigzag *MCU_zigzag)
{
    uint8_t largeur_MCU = 8, hauteur_MCU = 8;
    int16_t *pixels_zigzag = MCU_zigzag->pixels_zigzag;
    for (uint8_t hauteur_pix = 0; hauteur_pix < hauteur_MCU; hauteur_pix++){
        for (uint8_t largeur_pix = 0; largeur_pix < largeur_MCU; largeur_pix++){
            printf("%hx\t", pixels_zigzag[hauteur_pix * 8 + largeur_pix]);
        }
        printf("\n");
    }
}


/* Affiche les MCUs réorgansées en zigzag */
void print_MCUs_zigzag(struct MCU_zigzag ***MCUs_zigzag, uint32_t largeur_MCUs, uint32_t hauteur_MCUs)
{
    for (uint32_t hauteur = 0; hauteur < hauteur_MCUs; hauteur++){
        for (uint32_t largeur = 0; largeur < largeur_MCUs; largeur++){
            printf("MCU %d:\n", hauteur * largeur_MCUs + largeur);
            print_MCU_zigzag(MCUs_zigzag[hauteur][largeur]);
            printf("\n");
        }
    }
}


/* Libère la mémoire allouée aux MCUs en zigzag */
void free_MCUs_zigzag(struct MCU_zigzag ***MCUs_zigzag, uint32_t largeur_MCUs, uint32_t hauteur_MCUs)
{
    for (int32_t hauteur = hauteur_MCUs - 1; hauteur >= 0; hauteur--){
        for (int32_t largeur = largeur_MCUs - 1; largeur >= 0; largeur--){
            free(MCUs_zigzag[hauteur][largeur]->pixels_zigzag);
            free(MCUs_zigzag[hauteur][largeur]);
        }
        free(MCUs_zigzag[hauteur]);
    }
    free(MCUs_zigzag);
}


/************************************************************/
/* Fin de la partie consacrée à la réorganisation en zigzag */
/************************************************************/


/****************************************/
/* Partie consacrée à la quantification */
/****************************************/

/* Applique la quantification à une MCU */
void quantification_MCU(struct MCU_zigzag *MCU_zigzag)
{
    for (uint8_t indice = 0; indice < MCU_zigzag->hauteur * MCU_zigzag->largeur; indice++){
        MCU_zigzag->pixels_zigzag[indice] = MCU_zigzag->pixels_zigzag[indice]/quantification_table_Y[indice];
    }
}


/* Applique la quantification aux MCUs */
void quantification(struct MCU_zigzag ***MCUs_zigzag, uint32_t largeur_MCUs, uint32_t hauteur_MCUs)
{
    for (uint32_t hauteur = 0; hauteur < hauteur_MCUs; hauteur++){
        for (uint32_t largeur = 0; largeur < largeur_MCUs; largeur++){
            quantification_MCU(MCUs_zigzag[hauteur][largeur]);
        }
    }
}


/**************************************************/
/* Fin de la partie consacrée à la quantification */
/**************************************************/

struct MCU *MCU = malloc(sizeof(struct MCU));
MCU->hauteur = 8, MCU->largeur = 8;

uint8_t matrice_Y_g [8][8] = {  {0xa6, 0xa0, 0x9a, 0x98, 0x9a, 0x9a, 0x96, 0x91},
                            {0xa0, 0xa3, 0x9d, 0x8e, 0x88, 0x8f, 0x95, 0x94},
                            {0xa5, 0x97, 0x96, 0xa1, 0x9f, 0x90, 0x90, 0x9e},
                            {0xa6, 0x9a, 0x91, 0x91, 0x92, 0x90, 0x90, 0x93},
                            {0xc9, 0xd9, 0xc8, 0x98, 0x85, 0x98, 0xa2, 0x95},
                            {0xf0, 0xf5, 0xf9, 0xea, 0xbf, 0x98, 0x90, 0x9d},
                            {0xe9, 0xe1, 0xf3, 0xfd, 0xf2, 0xaf, 0x8a, 0x90},
                            {0xe6, 0xf2, 0xf1, 0xed, 0xf8, 0xfb, 0xd0, 0x95}};

uint8_t matrice_Y_d [8][8] = { {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
                              {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
                              {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa6, 0xa7},
                              {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
                              {0x9d, 0xa0, 0xa3, 0xa4, 0xa3, 0xa3, 0xa4, 0xa6},
                              {0x9b, 0x9e, 0xa1, 0xa2, 0xa1, 0xa1, 0xa3, 0xa4},
                              {0x99, 0x9c, 0x9f, 0xa0, 0x9f, 0x9f, 0xa1, 0xa2},
                              {0x98, 0x9b, 0x9e, 0x9f, 0x9e, 0x9e, 0xa0, 0xa1}};

uint8_t matrice_Cb_g [8][8] = { {0x75, 0x75, 0x75, 0x75, 0x76, 0x76, 0x77, 0x78},
                                {0x75, 0x75, 0x75, 0x75, 0x76, 0x76, 0x77, 0x77},
                                {0x75, 0x75, 0x76, 0x76, 0x76, 0x76, 0x77, 0x77},
                                {0x76, 0x76, 0x76, 0x76, 0x77, 0x77, 0x77, 0x77},
                                {0x76, 0x76, 0x77, 0x77, 0x77, 0x77, 0x78, 0x78},
                                {0x77, 0x78, 0x78, 0x77, 0x77, 0x77, 0x78, 0x78},
                                {0x77, 0x77, 0x77, 0x78, 0x78, 0x78, 0x78, 0x78},
                                {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78}};

uint8_t matrice_Cb_d [8][8] = { {0x78, 0x79, 0x7a, 0x7a, 0x7b, 0x7b, 0x7b, 0x7b},
                                {0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7b, 0x7b},
                                {0x78, 0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7a},
                                {0x78, 0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7a},
                                {0x78, 0x78, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79},
                                {0x78, 0x78, 0x78, 0x78, 0x79, 0x79, 0x79, 0x79},
                                {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78},
                                {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78}};

uint8_t matrice_Cr_g [8][8] = { {0x8d, 0x8c, 0x89, 0x87, 0x85, 0x85, 0x86, 0x89},
                                {0x8d, 0x8c, 0x89, 0x87, 0x85, 0x85, 0x86, 0x88},
                                {0x8c, 0x8b, 0x89, 0x87, 0x85, 0x85, 0x85, 0x88},
                                {0x8c, 0x8b, 0x88, 0x86, 0x85, 0x85, 0x85, 0x88},
                                {0x8c, 0x8b, 0x88, 0x86, 0x84, 0x84, 0x85, 0x87},
                                {0x8b, 0x87, 0x84, 0x86, 0x84, 0x84, 0x84, 0x87},
                                {0x8b, 0x8a, 0x87, 0x81, 0x84, 0x84, 0x84, 0x87},
                                {0x8b, 0x89, 0x87, 0x85, 0x84, 0x83, 0x84, 0x87}};

uint8_t matrice_Cb_d [8][8] = { {0x8b, 0x8e, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x88},
                                {0x8b, 0x8d, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x87},
                                {0x8a, 0x8d, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x87},
                                {0x8a, 0x8d, 0x8d, 0x8d, 0x8c, 0x8a, 0x87, 0x86},
                                {0x8a, 0x8c, 0x8d, 0x8d, 0x8b, 0x89, 0x86, 0x85},
                                {0x89, 0x8c, 0x8d, 0x8d, 0x8b, 0x89, 0x86, 0x86},
                                {0x89, 0x8c, 0x8c, 0x8c, 0x8b, 0x89, 0x86, 0x86},
                                {0x89, 0x8c, 0x8c, 0x8c, 0x8b, 0x89, 0x86, 0x86}};


uint8_t **pixels = malloc(MCU->hauteur * sizeof(uint8_t *));
for (uint8_t i = 0; i < MCU->hauteur; i++){
    uint8_t *ligne_pixels = malloc(MCU->largeur * sizeof(uint8_t));
    for (uint8_t j = 0; j < MCU->largeur; j++){
        ligne_pixels[j] = matrice[i][j];
    }
    pixels[i] = ligne_pixels;
}
MCU->pixels = pixels;
