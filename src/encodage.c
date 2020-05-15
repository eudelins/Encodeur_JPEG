#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "../include/decoupe.h"
#include "../include/qtables.h"

#define M_PI 3.14159265358979323846



/******************************/
/* Module d'encodage des MCUs */
/******************************/


/************************************************************/
/* Partie consacrée à la transformation en cosinus discrète */
/************************************************************/


/* MCU après transformée en cosinus discrète */
struct MCU_freq{
    uint8_t largeur;
    uint8_t hauteur;
    int16_t **pixels_freq;
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
    for (int32_t hauteur = hauteur_MCUs - 1; hauteur >= 0; hauteur--){
        for (int32_t largeur = largeur_MCUs - 1; largeur >= 0; largeur--){
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



/************************************/
/* Partie consacrée au codage AC/DC */
/************************************/


// int8_t *codage_AC_DC(int16_t *pixels)
// {
//     int8_t pixels_AC_DC = malloc(64 * sizeof(int8_t));

//     // Codage AC
//     uint8_t compteur_0;
//     for(uint8_t indice = 1; indice < 64; indice++){
//         if (pixels[indice] == 0 and compteur_0 < 16) {
//             compteur_0 += 1;
//         } else if (pixels[indice] == 0 and compteur_0 < 16) {
//             compteur_0 = 0;
//             pixels_AC_DC[indice] = 0xF0;
//         } else {
//             // On décale le nombre de 0 de 4 bits vers la gauche (bits de poids forts)
//             compteur = compteur_0 << 4;

//             // On calcule la magnitude du pixel
//             int8_t magnitude = 1;
//             while (!(-(2**magnitude) < pixels[indice] < 2**magnitude)){
//                 magnitude += 1;
//             }

//             pixels_AC_DC[indice] = compteur_0 + magnitude;

//             // On remet le compteur à 0
//             compteur_0 = 0;
//         }

//     }
// }


/**********************************************/
/* Fin de la partie consacrée au codage AC/DC */
/**********************************************/



// int main(void)
// {    
//     FILE *fichier = ouvrir_fichier("../images/gris.pgm", "r");
    
//     struct MCU ***MCUs = decoupage(fichier, 8, 8);
//     uint32_t *dimensions_MCUs = calcul_dimensions_MCUs(320, 320, 8, 8);
//     print_MCU(MCUs[dimensions_MCUs[0] - 1][dimensions_MCUs[1] - 1]);
//     printf("\n");

//     uint32_t largeur_MCUs = dimensions_MCUs[0], hauteur_MCUs = dimensions_MCUs[1];
//     struct MCU_freq ***MCUs_freq = transf_cos(MCUs, largeur_MCUs, hauteur_MCUs);
//     print_MCU_freq(MCUs_freq[hauteur_MCUs - 1][largeur_MCUs - 1]);
//     printf("\n");
    
//     struct MCU_zigzag ***MCUs_zigzag = zigzag(MCUs_freq, largeur_MCUs, hauteur_MCUs);
//     print_MCU_zigzag(MCUs_zigzag[hauteur_MCUs - 1][largeur_MCUs - 1]);
//     printf("\n");
    
//     quantification(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);
//     print_MCU_zigzag(MCUs_zigzag[hauteur_MCUs - 1][largeur_MCUs - 1]);
    
//     free_MCUs_zigzag(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);
//     free_MCUs_freq(MCUs_freq, largeur_MCUs, hauteur_MCUs);
//     free_MCUs(MCUs, dimensions_MCUs);
//     fermer_fichier(fichier);
//     return 0;
// }

    

    
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
    // printf("\n");
    // struct MCU_zigzag *MCU_zigzag = zigzag_MCU(MCU_freq);
    // print_MCU_zigzag(MCU_zigzag);
    
    // return 0;