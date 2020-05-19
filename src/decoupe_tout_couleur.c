#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/**************************************************/
/* Module de découpage de l'image couleur en MCUs */
/**************************************************/


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


/*Défini un pixel en RGB*/
typedef struct Pixel_RGB Pixel_RGB;
struct Pixel_RGB {
  uint8_t R;
  uint8_t G;
  uint8_t B;
};


/* Défini une MCU */
typedef struct MCU MCU;
struct MCU {
    uint8_t largeur;
    uint8_t hauteur;
    Pixel_RGB **pixels;
};


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



/* Converti l'entier lu dans un fichier en pixel RGB */
Pixel_RGB *converti_entier_pixel(uint32_t entier)
{
  Pixel_RGB *pixel = malloc(sizeof(Pixel_RGB*));
  uint8_t quotient = 0;
  uint8_t reste = entier;
  quotient = reste / 0x10000;
  reste = reste % 0x10000;
  pixel->R = quotient;
  quotient = reste / 0x100;
  reste = reste % 0x100;
  pixel->G = quotient;
  pixel->B = reste;
  return pixel;
}


/* Libère la mémoire alloué pour les pixels RGB d'une MCU */
void free_pixel(Pixel_RGB **pixels)
{
    for (int8_t hauteur_pix = 7; hauteur_pix >= 0; hauteur_pix--){
        free(pixels[hauteur_pix]);
    }
    free(pixels);
}


/* Découpe l'image en MCUs */
MCU ***decoupage(FILE *fichier, uint8_t largeur_MCU, uint8_t hauteur_MCU)
{
    // On récupère l'en-tête (P5 ou P6)
    char en_tete[10];
    fgets(en_tete, 10, fichier);

    // On récupère les dimensions de l'image
    char dimensions[10];
    uint32_t largeur_image, hauteur_image;
    fgets(dimensions, 10, fichier);
    sscanf(dimensions, "%u %u", &largeur_image, &hauteur_image);

    // On calcule le nombre de lignes et colonnes à copier
    uint8_t duplique_colonne = largeur_MCU - largeur_image % largeur_MCU;
    uint8_t duplique_ligne = hauteur_MCU - hauteur_image % hauteur_MCU;

    // On saute une ligne
    char couleurs_max[10];
    fgets(couleurs_max, 10, fichier);

    // On récupère les pixels dans une matrice
    Pixel_RGB **pixels_image = malloc((hauteur_image + duplique_ligne) * sizeof(Pixel_RGB *));
    for (uint32_t hauteur = 0; hauteur < hauteur_image; hauteur++){
        Pixel_RGB *ligne_pixels_image = malloc((largeur_image + duplique_colonne) * sizeof(Pixel_RGB*));
        for (uint32_t largeur = 0; largeur < largeur_image; largeur++){
          ligne_pixels_image[largeur].R = fgetc(fichier);
          ligne_pixels_image[largeur].G = fgetc(fichier);
          ligne_pixels_image[largeur].B = fgetc(fichier);
        }
        for (uint32_t largeur = largeur_image; largeur < largeur_image + duplique_colonne; largeur++){
          ligne_pixels_image[largeur] = ligne_pixels_image[largeur_image - 1];
        }
        pixels_image[hauteur] = ligne_pixels_image;
    }
    for (uint32_t hauteur = hauteur_image; hauteur < hauteur_image + duplique_ligne; hauteur++){
      pixels_image[hauteur] = pixels_image[hauteur_image - 1];
    }

    // On calcule les dimensions des MCUs
    uint32_t *dimensions_MCUs = calcul_dimensions_MCUs(largeur_image, hauteur_image, largeur_MCU, hauteur_MCU);
    uint32_t hauteur_MCUs, largeur_MCUs;
    largeur_MCUs = dimensions_MCUs[0];
    hauteur_MCUs = dimensions_MCUs[1];
    free(dimensions_MCUs);

    // On découpe en MCUs
    MCU ***MCUs = malloc(hauteur_MCUs * sizeof(MCU**));
    for (uint32_t hauteur = 0; hauteur < hauteur_MCUs; hauteur++){
        MCU **ligne_MCUs = malloc(largeur_MCUs * sizeof(MCU));
        for (uint32_t largeur = 0; largeur < largeur_MCUs; largeur++){
            MCU *MCU = malloc(sizeof(MCU));
            MCU->largeur = largeur_MCU;
            MCU->hauteur = hauteur_MCU;

            // On recopie les pixels
            Pixel_RGB **pixels = malloc(largeur_MCU * sizeof(Pixel_RGB *));
            for (uint8_t hauteur_pix = 0; hauteur_pix < hauteur_MCU; hauteur_pix++){
                Pixel_RGB *ligne_pixels = malloc(hauteur_MCU * sizeof(Pixel_RGB));
                for (uint8_t largeur_pix = 0; largeur_pix < largeur_MCU; largeur_pix++){
                    ligne_pixels[largeur_pix] = pixels_image[hauteur * 8 + hauteur_pix][largeur * 8 + largeur_pix];
                }
                pixels[hauteur_pix] = ligne_pixels;
            }
            MCU->pixels = pixels;

            ligne_MCUs[largeur] = MCU;
        }
        MCUs[hauteur] = ligne_MCUs;
    }

    free_pixel(pixels_image);

    return MCUs;
}


/* Libère la mémoire allouée aux MCUs */
void free_MCUs(MCU ***MCUs, uint32_t* dimensions_MCUs)
{
    uint32_t hauteur_MCUs, largeur_MCUs;
    largeur_MCUs = dimensions_MCUs[0];
    hauteur_MCUs = dimensions_MCUs[1];
    for (int32_t hauteur = hauteur_MCUs - 1; hauteur >= 0; hauteur--){
        for (int32_t largeur = largeur_MCUs - 1; largeur >= 0; largeur--){
            free_pixel(MCUs[hauteur][largeur]->pixels);
            free(MCUs[hauteur][largeur]);
        }
        free(MCUs[hauteur]);
    }
    free(MCUs);
    free(dimensions_MCUs);
}


/* Affiche un pixel */
void print_pixel(Pixel_RGB pixel)
{
  printf("%x %x %x \t", pixel.R, pixel.G, pixel.B);
}


/* Affiche une MCU */
void print_MCU(MCU *MCU)
{
    uint8_t largeur_MCU = 8, hauteur_MCU = 8;
    Pixel_RGB **pixels = MCU->pixels;
    for (uint8_t hauteur_pix = 0; hauteur_pix < hauteur_MCU; hauteur_pix++){
        for (uint8_t largeur_pix = 0; largeur_pix < largeur_MCU; largeur_pix++){
            print_pixel(pixels[hauteur_pix][largeur_pix]);
        }
        printf("\n");
    }
}


/* Affiche les MCUs */
void print_MCUs(MCU ***MCUs, uint32_t *dimensions_MCUs)
{
    uint32_t hauteur_MCUs, largeur_MCUs;
    largeur_MCUs = dimensions_MCUs[0];
    hauteur_MCUs = dimensions_MCUs[1];
    for (uint32_t hauteur = 0; hauteur < hauteur_MCUs ; hauteur++){
        for (uint32_t largeur = 0; largeur < largeur_MCUs; largeur++){
            print_MCU(MCUs[hauteur][largeur]);
            printf("\n\n");
        }
    }
}


int main(void)
{
    FILE *fichier = ouvrir_fichier("../images/shaun_the_sheep.ppm", "r");
    MCU ***MCUs = decoupage(fichier, 8, 8);
    uint32_t *dimensions_MCUs = calcul_dimensions_MCUs(300, 225, 8, 8);
    print_MCUs(MCUs, dimensions_MCUs);
    free_MCUs(MCUs, dimensions_MCUs);

    fermer_fichier(fichier);
    return 0;
}


    //
    // char ligne[10];
    // for (uint8_t i = 0; i < 3; i++){ // Lecture de l'en-tête
    //     fgets(ligne, 10, fichier);
    //     printf("%s", ligne);
    // }
    //
    // uint8_t caractere_lu = fgetc(fichier);
    // while(caractere_lu != EOF){
    //     printf("%u ", caractere_lu);
    //     caractere_lu = fgetc(fichier);
    // }
    // printf("\n");
