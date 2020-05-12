#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/******************************************/
/* Module de découpage de l'image en MCUs */
/******************************************/


/* Ouvre le fichier filename avec le mode d'accès mode. Retourne le FILE *
 * correspondant. */
FILE *ouvrir_fichier(const char *filename, const char *mode) {
     FILE *fichier = fopen(filename, mode);
     return fichier;
}


/* Ferme le fichier passé en paramètre. */
void fermer_fichier(FILE *fichier) {
     int ret = fclose(fichier);
}

struct MCU {
    uint8_t largeur;
    uint8_t hauteur;
    uint8_t **pixels;
};

// Pour l'instant, pixel = uint8_t, mais après, définir une struct RGB et YCbCr


/* Calcule le nombre de MCUs dans l'image */
uint32_t *calcul_dimensions_MCUs(uint32_t largeur_image, uint32_t hauteur_image,
                                 uint8_t largeur_MCU, uint8_t hauteur_MCU)
{
    uint32_t *dimensions = malloc(2 * sizeof(uint32_t));
    if (largeur_image % largeur_MCU) {
        dimensions[0] = largeur_image/largeur_MCU + 1;
    } else {
        dimensions[0] = largeur_image/largeur_MCU;
    }
    if (hauteur_image % hauteur_MCU) {
        dimensions[1] = hauteur_image/hauteur_MCU + 1;
    } else {
        dimensions[1] = hauteur_image/hauteur_MCU;
    }
    return dimensions;
}


/* Découpe l'image en MCUs */
struct MCU **decoupage(FILE *fichier, uint8_t largeur_MCU, uint8_t hauteur_MCU)
{
    // On récupère l'en-tête (P5 ou P6)
    char en_tete[10];
    fgets(en_tete, 10, fichier);

    // On récupère les dimensions de l'image
    char dimensions[10];
    uint32_t largeur_image, hauteur_image;
    fgets(dimensions, 10, fichier);
    sscanf(dimensions, "%u %u", &largeur_image, &hauteur_image);
    
    char couleurs_max[10];
    fgets(couleurs_max, 10, fichier);

    uint32_t *dimensions_MCUs = calcul_dimensions_MCUs(largeur_image, hauteur_image, largeur_MCU, hauteur_MCU);
    uint32_t hauteur_MCUs, largeur_MCUs;
    largeur_MCUs = dimensions_MCUs[0];
    hauteur_MCUs = dimensions_MCUs[1];
    free(dimensions_MCUs);

    // On découpe en MCUs
    struct MCU **MCUs = malloc(largeur_MCUs * hauteur_MCUs * sizeof(struct MCU));
    for (uint32_t hauteur = 0; hauteur < hauteur_MCUs; hauteur++){
        for (uint32_t largeur = 0; largeur < largeur_MCUs; largeur++){
            struct MCU *MCU = malloc(sizeof(struct MCU));
            MCU->largeur = largeur_MCU;
            MCU->hauteur = hauteur_MCU;

            // On recopie les pixels
            uint8_t **pixels = malloc(largeur_MCU * sizeof(uint8_t *));
            for (uint8_t hauteur_pix = 0; hauteur_pix < hauteur_MCU; hauteur_pix++){
                uint8_t *ligne_pixels = malloc(hauteur_MCU * sizeof(uint8_t)); 
                for (uint8_t largeur_pix = 0; largeur_pix < largeur_MCU; largeur_pix++){
                    ligne_pixels[largeur_pix] = fgetc(fichier);
                }
                pixels[hauteur_pix] = ligne_pixels;   
            }
            MCU->pixels = pixels;

            MCUs[hauteur * largeur_MCUs + largeur] = MCU;       
        }   
    }
    return MCUs;
}


/* Libère la mémoire alloué pour les pixels d'une MCU */
void free_pixel(uint8_t **pixels)
{
    for (int8_t hauteur_pix = 7; hauteur_pix >= 0; hauteur_pix--){
        free(pixels[hauteur_pix]);
    }
    free(pixels);
}


/* Libère la mémoire allouée aux MCUs */
void free_MCUs(struct MCU **MCUs, uint32_t* dimensions_MCUs)
{
    uint32_t hauteur_MCUs, largeur_MCUs;
    largeur_MCUs = dimensions_MCUs[0];
    hauteur_MCUs = dimensions_MCUs[1];
    for (int32_t hauteur = hauteur_MCUs - 1; hauteur >= 0; hauteur--){
        for (int32_t largeur = largeur_MCUs - 1; largeur >= 0; largeur--){
            free_pixel(MCUs[hauteur * largeur_MCUs + largeur]->pixels);
            free(MCUs[hauteur * largeur_MCUs + largeur]);
        }
    }
    free(MCUs);
    free(dimensions_MCUs);
}

/* Affiche une MCU */
void print_MCU(struct MCU *MCU)
{
    uint8_t largeur_MCU = 8, hauteur_MCU = 8;
    uint8_t **pixels = MCU->pixels;
    for (uint8_t hauteur_pix = 0; hauteur_pix < hauteur_MCU; hauteur_pix++){
        for (uint8_t largeur_pix = 0; largeur_pix < largeur_MCU; largeur_pix++){
            printf("%u\t", pixels[hauteur_pix][largeur_pix]);
        }
        printf("\n");        
    }
}


/* Affiche les MCUs */
void print_MCUs(struct MCU **MCUs, uint32_t *dimensions_MCUs)
{
    uint32_t hauteur_MCUs, largeur_MCUs;
    largeur_MCUs = dimensions_MCUs[0];
    hauteur_MCUs = dimensions_MCUs[1];
    for (int32_t hauteur = 0; hauteur < hauteur_MCUs ; hauteur++){
        for (int32_t largeur = 0; largeur < largeur_MCUs; largeur++){
            print_MCU(MCUs[hauteur * largeur_MCUs + largeur]);
        }
    }
}


int main(void)
{
    FILE *fichier = ouvrir_fichier("../images/invader.pgm", "r");
    struct MCU **MCUs = decoupage(fichier, 8, 8);
    uint32_t *dimensions_MCUs = calcul_dimensions_MCUs(8, 8, 8, 8);
    print_MCUs(MCUs, dimensions_MCUs);
    free_MCUs(MCUs, dimensions_MCUs);

    // char ligne[10];
    // for (uint8_t i = 0; i < 3; i++){ // Lecture de l'en-tête
    //     fgets(ligne, 10, fichier);
    //     printf("%s", ligne);
    // }

    // uint8_t caractere_lu = fgetc(fichier);
    // while(caractere_lu != EOF){
    //     printf("%u ", caractere_lu);
    //     caractere_lu = fgetc(fichier);
    // }
    // printf("\n");

    fermer_fichier(fichier);
    return 0;
}
