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
    fclose(fichier);
}


/* Défini une MCU */
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


/* Libère la mémoire alloué pour les pixels d'une MCU */
void free_pixel(uint8_t **pixels, uint32_t hauteur_pix)
{
    for (int32_t hauteur = hauteur_pix - 1; hauteur >= 0; hauteur--){
        free(pixels[hauteur]);
    }
    free(pixels);
}


/* Découpe l'image en MCUs */
struct MCU ***decoupage(FILE *fichier, 
                        uint32_t largeur_MCUs, 
                        uint32_t hauteur_MCUs)
{
    uint32_t hauteur_image = hauteur_MCUs * 8;
    uint32_t largeur_image = largeur_MCUs * 8;

    // On récupère les pixels dans une matrice
    uint8_t **pixels_image = malloc(hauteur_image * sizeof(uint8_t *));
    for (uint32_t hauteur = 0; hauteur < hauteur_image; hauteur++){
        uint8_t *ligne_pixels_image = malloc(largeur_image * sizeof(uint8_t*)); 
        for (uint32_t largeur = 0; largeur < largeur_image; largeur++){
            ligne_pixels_image[largeur] = fgetc(fichier);
        }
        pixels_image[hauteur] = ligne_pixels_image;   
    }


    // On découpe en MCUs
    struct MCU ***MCUs = malloc(hauteur_MCUs * sizeof(struct MCU**));
    for (uint32_t hauteur = 0; hauteur < hauteur_MCUs; hauteur++){
        struct MCU **ligne_MCUs = malloc(largeur_MCUs * sizeof(struct MCU));
        for (uint32_t largeur = 0; largeur < largeur_MCUs; largeur++){
            struct MCU *MCU = malloc(sizeof(struct MCU));
            MCU->largeur = 8;
            MCU->hauteur = 8;

            // On recopie les pixels
            uint8_t **pixels = malloc(MCU->hauteur * sizeof(uint8_t *));
            for (uint8_t hauteur_pix = 0; hauteur_pix < MCU->hauteur; hauteur_pix++){
                uint8_t *ligne_pixels = malloc(MCU->largeur * sizeof(uint8_t)); 
                for (uint8_t largeur_pix = 0; largeur_pix < MCU->largeur; largeur_pix++){
                    ligne_pixels[largeur_pix] = pixels_image[hauteur * MCU->hauteur + hauteur_pix][largeur * MCU->largeur + largeur_pix];
                }
                pixels[hauteur_pix] = ligne_pixels;   
            }
            MCU->pixels = pixels;

            ligne_MCUs[largeur] = MCU;       
        }
        MCUs[hauteur] = ligne_MCUs;  
    }

    free_pixel(pixels_image, hauteur_image);

    return MCUs;
}


/* Libère la mémoire allouée aux MCUs */
void free_MCUs(struct MCU ***MCUs, uint32_t* dimensions_MCUs)
{
    uint32_t hauteur_MCUs, largeur_MCUs;
    largeur_MCUs = dimensions_MCUs[0];
    hauteur_MCUs = dimensions_MCUs[1];
    for (int32_t hauteur = hauteur_MCUs - 1; hauteur >= 0; hauteur--){
        for (int32_t largeur = largeur_MCUs - 1; largeur >= 0; largeur--){
            free_pixel(MCUs[hauteur][largeur]->pixels, 8);
            free(MCUs[hauteur][largeur]);
        }
        free(MCUs[hauteur]);
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
            printf("%hhx\t", pixels[hauteur_pix][largeur_pix]);
        }
        printf("\n");        
    }
}


/* Affiche les MCUs */
void print_MCUs(struct MCU ***MCUs, uint32_t *dimensions_MCUs)
{
    uint32_t hauteur_MCUs, largeur_MCUs;
    largeur_MCUs = dimensions_MCUs[0];
    hauteur_MCUs = dimensions_MCUs[1];
    for (uint32_t hauteur = 0; hauteur < hauteur_MCUs ; hauteur++){
        for (uint32_t largeur = 0; largeur < largeur_MCUs; largeur++){
            printf("MCU %d:\n", hauteur * largeur_MCUs + largeur);
            print_MCU(MCUs[hauteur][largeur]);
            printf("\n\n");
        }
    }
}


// int main(void)
// {
//     FILE *fichier = ouvrir_fichier("../images/gris.pgm", "r");
//     struct MCU ***MCUs = decoupage(fichier, 8, 8);
//     uint32_t *dimensions_MCUs = calcul_dimensions_MCUs(320, 320, 8, 8);
//     print_MCUs(MCUs, dimensions_MCUs);
//     free_MCUs(MCUs, dimensions_MCUs);

//     fermer_fichier(fichier);
//     return 0;
// }



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