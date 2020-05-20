#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define COTE_BLOC 8


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
struct Pixel_RGB {
  uint8_t R;
  uint8_t G;
  uint8_t B;
};


/* Défini un bloc */
struct Bloc {
    struct Pixel_RGB **pixels;      // un Bloc = une matrice de pixels
};


/* Défini une MCU */
struct MCU {
    uint8_t largeur;
    uint8_t hauteur;
    struct Pixel_RGB **pixels;      // une MCU = une matrice de pixels
    struct Bloc **blocs;            // une MCU = une matrice de blocs
};


/* Calcule le nombre de MCUs dans l'image */
uint32_t *calcul_dimensions_MCUs(uint32_t largeur_image,
                                 uint32_t hauteur_image,
                                 uint8_t largeur_MCU,
                                 uint8_t hauteur_MCU)
{
    uint32_t *dimensions = malloc(2 * sizeof(uint32_t));
    if (largeur_image % (largeur_MCU * COTE_BLOC)) {
        dimensions[0] = largeur_image/(largeur_MCU * COTE_BLOC) + 1;
    } else {
        dimensions[0] = largeur_image/(largeur_MCU * COTE_BLOC);
    }
    if (hauteur_image % (hauteur_MCU * COTE_BLOC)) {
        dimensions[1] = hauteur_image/(hauteur_MCU * COTE_BLOC) + 1;
    } else {
        dimensions[1] = hauteur_image/(hauteur_MCU * COTE_BLOC);
    }
    return dimensions;
}


/* Libère la mémoire alloué pour les pixels RGB d'une MCU (donc par la matrice de pixels */
void free_pixel(struct Pixel_RGB **pixels)
{
    for (int8_t hauteur_pix = 7; hauteur_pix >= 0; hauteur_pix--){
        free(pixels[hauteur_pix]);
    }
    free(pixels);
}


/* Libère la mémoire alloué pour les pixels de l'image */
void free_pixel_image(struct Pixel_RGB **pixels,
                      uint32_t hauteur_image)
{
    for (int32_t hauteur_pix = hauteur_image - 1; hauteur_pix >= 0; hauteur_pix--){
        free(pixels[hauteur_pix]);
    }
    free(pixels);
}

/* Découpe l'image en MCUs : nb_MCUs_largeur x nb_MCUs_hauteur */
struct MCU ***decoupage(FILE *fichier,
                        uint32_t largeur_image,
                        uint32_t hauteur_image,
                        uint32_t nb_MCUs_largeur,
                        uint32_t nb_MCUs_hauteur,
                        uint32_t largeur_MCU,
                        uint32_t hauteur_MCU)
{
    // On calcule le nombre de lignes et colonnes à copier
    uint8_t duplique_colonne = (nb_MCUs_largeur * COTE_BLOC) - largeur_image % (nb_MCUs_largeur * COTE_BLOC);
    uint8_t duplique_ligne = (nb_MCUs_hauteur * COTE_BLOC) - hauteur_image % (nb_MCUs_hauteur * COTE_BLOC);

    // On saute une ligne
    char couleurs_max[10];
    fgets(couleurs_max, 10, fichier);

    // On récupère les pixels du fichier dans une matrice
    // matrice de pixels
    struct Pixel_RGB **pixels_image = malloc((hauteur_image + duplique_ligne) * sizeof(struct Pixel_RGB *));
    for (uint32_t hauteur = 0; hauteur < hauteur_image; hauteur++){
        struct Pixel_RGB *ligne_pixels_image = malloc((largeur_image + duplique_colonne) * sizeof(struct Pixel_RGB*));
        // On parcourt l'image avec fgetc et on rempli ligne_pixels_image pour ensuite remplir pixels_image
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

    // D'abord : on découpe en MCUs
    // pointeur vers une matrice de MCU
    struct MCU ***MCUs = malloc(nb_MCUs_hauteur * sizeof(struct MCU**));
    for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur; hauteur++){
        // matrice de MCU
        struct MCU **ligne_MCUs = malloc(nb_MCUs_largeur * sizeof(struct MCU));
        for (uint32_t largeur = 0; largeur < nb_MCUs_largeur; largeur++){
            // tableau de MCU
            struct MCU *MCU = malloc(sizeof(struct MCU));
            MCU->largeur = largeur_MCU;
            MCU->hauteur = hauteur_MCU;

            // On recopie les pixels
            // matrice de pixels qui est dans la structure d'une MCU
            struct Pixel_RGB **pixels_MCU = malloc(largeur_MCU * COTE_BLOC * sizeof(struct Pixel_RGB *));

            for (uint8_t hauteur_pix = 0; hauteur_pix < hauteur_MCU * COTE_BLOC; hauteur_pix++){
                struct Pixel_RGB *ligne_pixels = malloc(hauteur_MCU * COTE_BLOC * sizeof(struct Pixel_RGB));
                for (uint8_t largeur_pix = 0; largeur_pix < largeur_MCU * COTE_BLOC; largeur_pix++){
                    ligne_pixels[largeur_pix] = pixels_image[hauteur * COTE_BLOC + hauteur_pix][largeur * COTE_BLOC + largeur_pix];
                }
                pixels_MCU[hauteur_pix] = ligne_pixels;
            }
            MCU->pixels = pixels_MCU;

            ligne_MCUs[largeur] = MCU;
        }
        MCUs[hauteur] = ligne_MCUs;
    }

    free_pixel_image(pixels_image, hauteur_image);

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
            free_pixel(MCUs[hauteur][largeur]->pixels);
            free(MCUs[hauteur][largeur]);
        }
        free(MCUs[hauteur]);
    }
    free(MCUs);
    free(dimensions_MCUs);
}

/* Affiche un pixel */
void print_pixel(struct Pixel_RGB pixel)
{
  printf("%x %x %x \t", pixel.R, pixel.G, pixel.B);
}


/* Affiche une MCU */
void print_MCU(struct MCU *MCU, uint8_t largeur_MCU, uint8_t hauteur_MCU)
{
    struct Pixel_RGB **pixels = MCU->pixels;
    for (uint8_t hauteur_pix = 0; hauteur_pix < hauteur_MCU; hauteur_pix++){
        for (uint8_t largeur_pix = 0; largeur_pix < largeur_MCU; largeur_pix++){
            printf("%x\t", pixels[hauteur_pix][largeur_pix]);
        }
        printf("\n");
    }
}


/* Affiche les MCUs */
void print_MCUs(struct MCU ***MCUs, uint32_t *dimensions_MCUs, uint8_t largeur_MCU, uint8_t hauteur_MCU)
{
    uint32_t nb_MCUs_largeur, nb_MCUs_hauteur;;
    nb_MCUs_largeur = dimensions_MCUs[0];
    nb_MCUs_hauteur = dimensions_MCUs[1];
    for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur ; hauteur++){
        for (uint32_t largeur = 0; largeur < nb_MCUs_largeur; largeur++){
            printf("MCU %d:\n", hauteur * nb_MCUs_largeur + largeur);
            print_MCU(MCUs[hauteur][largeur], largeur_MCU * COTE_BLOC, hauteur_MCU * COTE_BLOC);
            printf("\n\n");
        }
    }
}

int main(void)
{
    FILE *fichier = ouvrir_fichier("../images/shaun_the_sheep.ppm", "r");

    // On récupère l'en-tête (P5 ou P6)
    char en_tete[10];
    fgets(en_tete, 10, fichier);

    // On récupère les dimensions de l'image
    char dimensions[30];
    uint32_t largeur_image, hauteur_image;
    fgets(dimensions, 30, fichier);
    sscanf(dimensions, "%u %u", &largeur_image, &hauteur_image);

    // On calcule les dimensions des MCUs
    uint32_t *dimensions_MCUs = calcul_dimensions_MCUs(largeur_image, hauteur_image, 1, 1); // hauteur_MCU et largeur_MCU valent 1
    uint32_t nb_MCUs_hauteur, nb_MCUs_largeur;
    nb_MCUs_largeur = dimensions_MCUs[0];
    nb_MCUs_hauteur = dimensions_MCUs[1];

    struct MCU ***MCUs = decoupage(fichier, largeur_image, hauteur_image, nb_MCUs_largeur, nb_MCUs_hauteur, 1, 1);
    print_MCUs(MCUs, dimensions_MCUs, 1, 1);    // largeur_MCU = hauteur_MCU = 1
    free_MCUs(MCUs, dimensions_MCUs);
    fermer_fichier(fichier);
    return 0;
}
