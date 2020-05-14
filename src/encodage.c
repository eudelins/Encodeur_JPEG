#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "../include/decoupe.h"

#define M_PI 3.14159265358979323846



/******************************/
/* Module d'encodage des MCUs */
/******************************/


// struct MCU_encodee
// {
//     uint8_t largeur;
//     uint8_t hauteur;
//     uint8_t **pixels_encodes;
// };


/* Convertit les pixels des MCUs de RGB à YCbCr */
// struct MCU_encodee ***conversion_rgb_ycbcr(***struct MCU MCUs)
// {

// }

/* MCU après transformée en cosinus discrète */
struct MCU_freq{
    uint8_t largeur;
    uint8_t hauteur;
    uint16_t **pixels_freq;
};


/* Calcul C(i) */
double C(uint8_t i)
{
    if (i == 0) {
        return 0.7071067812;
    } else {
        return 1.0;
    }
}


/* Affiche une MCU transformée */
void print_MCU_freq(struct MCU_freq *MCU_freq)
{
    uint8_t largeur_MCU = 8, hauteur_MCU = 8;
    uint16_t **pixels_freq = MCU_freq->pixels_freq;
    for (uint8_t hauteur_pix = 0; hauteur_pix < hauteur_MCU; hauteur_pix++){
        for (uint8_t largeur_pix = 0; largeur_pix < largeur_MCU; largeur_pix++){
            printf("%x\t", pixels_freq[hauteur_pix][largeur_pix]);
        }
        printf("\n");        
    }
}


/* Affiche les MCUs transformées */
void print_MCUs_freq(struct MCU_freq ***MCUs_freq, uint32_t largeur_MCUs, uint32_t hauteur_MCUs)
{
    for (int32_t hauteur = hauteur_MCUs - 1; hauteur >= 0; hauteur--){
        for (int32_t largeur = largeur_MCUs - 1; largeur >= 0; largeur--){
            print_MCU_freq(MCUs_freq[hauteur][largeur]);
            printf("\n");
        }
    }
}



/* Applique la transformée en cosinus discrète à une MCU */
struct MCU_freq *transf_cos_MCU(struct MCU *MCU)
{    
    struct MCU_freq *MCU_freq = malloc(sizeof(struct MCU_freq));
    MCU_freq->largeur = MCU->largeur;
    MCU_freq->hauteur = MCU->hauteur;

    uint16_t **pixels_freq = malloc(MCU_freq->hauteur * sizeof(uint16_t *));
    for (uint8_t i = 0; i < MCU_freq->hauteur; i++){
        uint16_t *ligne_pixels_freq = malloc(MCU_freq->largeur * sizeof(uint16_t));
        
        for (uint8_t j = 0; j < MCU_freq->largeur; j++){

            // Calcul de la coordonnées (i, j) du domaine frequentiel
            double freq = 0.0;
            for (uint8_t x = 0; x < MCU_freq->hauteur; x++){        
                for (uint8_t y = 0; y < MCU_freq->largeur; y++){
                    freq += (MCU->pixels[x][y] - 128) * cos((2*x + 1)*i*M_PI/16) * cos((2*y + 1)*j*M_PI/16);
                }
            }
            freq = C(i) * C(j) * freq/4.0;

            ligne_pixels_freq[j] = (uint16_t) freq;
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


/* Libère la mémoire alloué pour les pixels d'une MCU fréquentielle */
void free_pixel_freq(uint16_t **pixels, uint32_t hauteur_pix)
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



int main(void)
{
    // struct MCU *MCU = malloc(sizeof(struct MCU));
    // MCU->hauteur = 8, MCU->largeur = 8;
    
    // uint8_t matrice [8][8] = {  {0xa6, 0xa0, 0x9a, 0x98, 0x9a, 0x9a, 0x96, 0x91},
    //                             {0xa0, 0xa3, 0x9d, 0x8e, 0x88, 0x8f, 0x95, 0x94},
    //                             {0xa5, 0x97, 0x96, 0xa1, 0x9f, 0x90, 0x90, 0x9e},
    //                             {0xa6, 0x9a, 0x91, 0x91, 0x92, 0x90, 0x90, 0x93},
    //                             {0xc9, 0xd9, 0xc8, 0x98, 0x85, 0x98, 0xa2, 0x95},
    //                             {0xf0, 0xf5, 0xf9, 0xea, 0xbf, 0x98, 0x90, 0x9d},
    //                             {0xe9, 0xe1, 0xf3, 0xfd, 0xf2, 0xaf, 0x8a, 0x90},
    //                             {0xe6, 0xf2, 0xf1, 0xed, 0xf8, 0xfb, 0xd0, 0x95}};

    // uint8_t **pixels = malloc(MCU->hauteur * sizeof(uint8_t *));
    // for (uint8_t i = 0; i < MCU->hauteur; i++){
    //     uint8_t *ligne_pixels = malloc(MCU->largeur * sizeof(uint8_t));
    //     for (uint8_t j = 0; j < MCU->largeur; j++){
    //         ligne_pixels[j] = matrice[i][j];
    //     }
    //     pixels[i] = ligne_pixels;
    // }
    // MCU->pixels = pixels;
    
    // struct MCU_freq *MCU_freq = transf_cos_MCU(MCU);
    // print_MCU_freq(MCU_freq);
    
    // return 0;
    
    FILE *fichier = ouvrir_fichier("../images/gris.pgm", "r");
    
    struct MCU ***MCUs = decoupage(fichier, 8, 8);
    uint32_t *dimensions_MCUs = calcul_dimensions_MCUs(320, 320, 8, 8);
    print_MCU(MCUs[dimensions_MCUs[0] - 1][dimensions_MCUs[1] - 1]);
    printf("\n");

    uint32_t largeur_MCUs = dimensions_MCUs[0], hauteur_MCUs = dimensions_MCUs[1];
    struct MCU_freq ***MCUs_freq = transf_cos(MCUs, largeur_MCUs, hauteur_MCUs);
    print_MCU_freq(MCUs_freq[hauteur_MCUs - 1][largeur_MCUs - 1]);
    
    free_MCUs_freq(MCUs_freq, largeur_MCUs, hauteur_MCUs);
    free_MCUs(MCUs, dimensions_MCUs);
    fermer_fichier(fichier);
    return 0;
}

    

    
