#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define COTE_BLOC 8


/**************************************************************/
/* Module de découpage de l'image couleur en MCUs et en blocs */
/**************************************************************/


/* Défini un pixel en RGB */
struct Pixel_RGB {
  uint8_t R;
  uint8_t G;
  uint8_t B;
};


/* Défini un bloc */
struct Bloc_RGB {
    struct Pixel_RGB **pixels;      // un Bloc_RGB = une matrice de pixels
};


/* Défini une MCU */
struct MCU_RGB {
    uint8_t h1;
    uint8_t v1;
    uint8_t h2;
    uint8_t v2;
    uint8_t h3;
    uint8_t v3;
    struct Pixel_RGB **pixels;      // une MCU = une matrice de pixels
    struct Bloc_RGB **blocs;            // une MCU = une matrice de blocs
};


/* Calcule le nombre de MCUs dans l'image */
uint32_t *calcul_dimensions_MCUs_RGB(uint32_t largeur_image,
                                     uint32_t hauteur_image,
                                     uint8_t h1,
                                     uint8_t v1)
{
    uint32_t *dimensions = malloc(2 * sizeof(uint32_t));
    if (largeur_image % (h1 * COTE_BLOC)) {
        dimensions[0] = largeur_image/(h1 * COTE_BLOC) + 1;
    } else {
        dimensions[0] = largeur_image/(h1 * COTE_BLOC);
    }
    if (hauteur_image % (v1 * COTE_BLOC)) {
        dimensions[1] = hauteur_image/(v1 * COTE_BLOC) + 1;
    } else {
        dimensions[1] = hauteur_image/(v1 * COTE_BLOC);
    }
    return dimensions;
}


/* Libère la mémoire alloué pour les pixels de l'image */
void free_pixel_image_RGB(struct Pixel_RGB **pixels,
                          uint32_t hauteur_image)
{
    for (int32_t hauteur_pix = hauteur_image - 1; hauteur_pix >= 0; hauteur_pix--){
        free(pixels[hauteur_pix]);
    }
    free(pixels);
}

/* Découpe l'image en nb_MCUs_largeur x nb_MCUs_hauteur MCUs */
struct MCU_RGB ***decoupage_MCUs(FILE *fichier,
                                 uint32_t largeur_image,
                                 uint32_t hauteur_image,
                                 uint32_t nb_MCUs_largeur,
                                 uint32_t nb_MCUs_hauteur,
                                 uint8_t h1,
                                 uint8_t v1,
                                 uint8_t h2,
                                 uint8_t v2,
                                 uint8_t h3,
                                 uint8_t v3)
{
    // On calcule le nombre de lignes et colonnes à copier
    uint8_t duplique_colonne = (nb_MCUs_largeur * COTE_BLOC * h1) - largeur_image % (nb_MCUs_largeur * COTE_BLOC * h1);
    uint8_t duplique_ligne = (nb_MCUs_hauteur * COTE_BLOC * v1) - hauteur_image % (nb_MCUs_hauteur * COTE_BLOC * v1);

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

    // On découpe tout d'abord en MCUs
    struct MCU_RGB ***MCUs = malloc(nb_MCUs_hauteur * sizeof(struct MCU_RGB**));
    for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur; hauteur++){
        // matrice de MCU
        struct MCU_RGB **ligne_MCUs = malloc(nb_MCUs_largeur * sizeof(struct MCU_RGB));
        
        for (uint32_t largeur = 0; largeur < nb_MCUs_largeur; largeur++){
            // tableau de MCU
            struct MCU_RGB *MCU = malloc(sizeof(struct MCU_RGB));
            MCU->h1 = h1;
            MCU->v1 = v1;
            MCU->h2 = h2;
            MCU->v2 = v2;
            MCU->h3 = h3;
            MCU->v3 = v3;

            // On recopie les pixels
            // matrice de pixels qui est dans la structure d'une MCU
            struct Pixel_RGB **pixels_MCU = malloc(v1 * COTE_BLOC * sizeof(struct Pixel_RGB *));

            for (uint8_t hauteur_pix = 0; hauteur_pix < v1 * COTE_BLOC; hauteur_pix++){
                struct Pixel_RGB *ligne_pixels = malloc(h1 * COTE_BLOC * sizeof(struct Pixel_RGB));
            
                for (uint8_t largeur_pix = 0; largeur_pix < h1 * COTE_BLOC; largeur_pix++){
                    ligne_pixels[largeur_pix] = pixels_image[hauteur * COTE_BLOC * v1 + hauteur_pix][largeur * COTE_BLOC * h1 + largeur_pix];
                }
                pixels_MCU[hauteur_pix] = ligne_pixels;
            }
            MCU->pixels = pixels_MCU;

            ligne_MCUs[largeur] = MCU;
        }
        MCUs[hauteur] = ligne_MCUs;
    }

    free_pixel_image_RGB(pixels_image, hauteur_image);

    return MCUs;
}


/* Découpe une MCU en blocs (on obtient alors une MCU de taille MCU->h1 x MCU->v1) */
void decoupage_blocs(struct MCU_RGB *MCU)
{
    uint8_t h1 = MCU->h1;
    uint8_t v1 = MCU->v1;

    // On découpe en blocs
    struct Bloc_RGB **blocs = malloc(v1 * sizeof(struct Bloc_RGB*));
    for (uint32_t hauteur = 0; hauteur < v1; hauteur++){
        struct Bloc_RGB *ligne_blocs = malloc(h1 * sizeof(struct Bloc_RGB));
        
        for (uint32_t largeur = 0; largeur < h1; largeur++){
            // On recopie les pixels
            struct Pixel_RGB **pixels = malloc(COTE_BLOC * sizeof(struct Pixel_RGB *));
            
            for (uint8_t hauteur_pix = 0; hauteur_pix < COTE_BLOC; hauteur_pix++){
                struct Pixel_RGB *ligne_pixels = malloc(COTE_BLOC * sizeof(struct Pixel_RGB));
                
                for (uint8_t largeur_pix = 0; largeur_pix < COTE_BLOC; largeur_pix++){
                    ligne_pixels[largeur_pix] = MCU->pixels[hauteur * COTE_BLOC + hauteur_pix][largeur * COTE_BLOC + largeur_pix];
                }
                pixels[hauteur_pix] = ligne_pixels;
            }
            ligne_blocs[largeur].pixels = pixels;
        }
        blocs[hauteur] = ligne_blocs;
    }
    MCU->blocs = blocs;
}


/* Découpe toutes les MCU en blocs (de tailles MCU->h1 x MCU->v1) */
struct MCU_RGB ***decoupage_MCUs_en_blocs(struct MCU_RGB ***MCUs,
                                          uint32_t nb_MCUs_largeur,
                                          uint32_t nb_MCUs_hauteur)
{
    for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur; hauteur++) {
        
        for (uint32_t largeur = 0; largeur < nb_MCUs_largeur; largeur++) {
            decoupage_blocs(MCUs[hauteur][largeur]);
        }
    }
    return MCUs;
}


/* Libère la mémoire allouée pour les pixels RGB d'une MCU (donc par la matrice de pixels d'une MCU) */
void free_pixel_RGB(struct Pixel_RGB **pixels,
                    uint8_t v1)
{
    for (int8_t hauteur_pix = COTE_BLOC * v1 - 1; hauteur_pix >= 0; hauteur_pix--){
        free(pixels[hauteur_pix]);
    }
    free(pixels);
}


/* Libère la mémoire allouée pour les blocs d'une MCU (donc par la matrice de blocs d'une MCU) */
void free_blocs_RGB(struct Bloc_RGB **blocs,
                    uint8_t h1,
                    uint8_t v1)
{
    for (int32_t hauteur = v1 - 1; hauteur >= 0; hauteur--){
        
        for (int32_t largeur = h1 - 1; largeur >= 0; largeur--){
            free_pixel_RGB(blocs[hauteur][largeur].pixels, 1);
        }
        free(blocs[hauteur]);
    }
    free(blocs);
}


/* Libère la mémoire allouée aux MCUs (donc par la matrice de MCUs) */
void free_MCUs_dims_RGB(struct MCU_RGB ***MCUs,
                        uint32_t* dimensions_MCUs)
{
   uint32_t nb_MCUs_hauteur, nb_MCUs_largeur;
    nb_MCUs_largeur = dimensions_MCUs[0];
    nb_MCUs_hauteur = dimensions_MCUs[1];
    for (int32_t hauteur = nb_MCUs_hauteur - 1; hauteur >= 0; hauteur--){
        
        for (int32_t largeur = nb_MCUs_largeur - 1; largeur >= 0; largeur--){
            uint8_t h1 = MCUs[hauteur][largeur]->h1;
            uint8_t v1 = MCUs[hauteur][largeur]->v1;
            free_pixel_RGB(MCUs[hauteur][largeur]->pixels, v1);
            free_blocs_RGB(MCUs[hauteur][largeur]->blocs, h1, v1);
            free(MCUs[hauteur][largeur]);
        }
        free(MCUs[hauteur]);
    }
    free(MCUs);
    free(dimensions_MCUs);
}


/* Affiche un pixel */
void print_pixel_RGB(struct Pixel_RGB pixel)
{
  printf("%x %x %x \t", pixel.R, pixel.G, pixel.B);
}


/* Affiche un bloc */
void print_bloc_RGB(struct Bloc_RGB bloc)
{
    struct Pixel_RGB **pixels = bloc.pixels;
    for (uint8_t hauteur_pix = 0; hauteur_pix < COTE_BLOC; hauteur_pix++){
        
        for (uint8_t largeur_pix = 0; largeur_pix < COTE_BLOC; largeur_pix++){
            print_pixel_RGB(pixels[hauteur_pix][largeur_pix]);
        }
        printf("\n");
    }
}


/* Affiche une MCU */
void print_MCU_RGB(struct MCU_RGB *MCU)
{
    uint8_t h1 = MCU->h1;
    uint8_t v1 = MCU->v1;

    struct Bloc_RGB **blocs = MCU->blocs;
    for (uint8_t hauteur_blocs = 0; hauteur_blocs < v1; hauteur_blocs++){
        
        for (uint8_t largeur_blocs = 0; largeur_blocs < h1; largeur_blocs++){
            printf("Bloc_RGB %d:\n", hauteur_blocs * h1 + largeur_blocs);
            print_bloc_RGB(blocs[hauteur_blocs][largeur_blocs]);
        }
        printf("\n");
    }
}


/* Affiche les MCUs */
void print_MCUs_RGB(struct MCU_RGB ***MCUs,
                    uint32_t *dimensions_MCUs)
{
    uint32_t nb_MCUs_largeur, nb_MCUs_hauteur;;
    nb_MCUs_largeur = dimensions_MCUs[0];
    nb_MCUs_hauteur = dimensions_MCUs[1];
    for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur ; hauteur++){
        
        for (uint32_t largeur = 0; largeur < nb_MCUs_largeur; largeur++){
            printf("MCU %d:\n", hauteur * nb_MCUs_largeur + largeur);
            print_MCU_RGB(MCUs[hauteur][largeur]);
            printf("\n\n");
        }
    }
}


 // main qui a permis de tester les fonctions
 //int main(void)
 //{
 //    FILE *fichier = fopen("../images/shaun_the_sheep.ppm", "r");

 //    // On récupère l'en-tête (P5 ou P6)
 //    char en_tete[10];
 //    fgets(en_tete, 10, fichier);

 //    // On récupère les dimensions de l'image
 //    char dimensions[30];
 //    uint32_t largeur_image, hauteur_image;
 //    fgets(dimensions, 30, fichier);
 //    sscanf(dimensions, "%u %u", &largeur_image, &hauteur_image);

 //    // On calcule les dimensions des MCUs
 //    uint8_t h1 = 2;
 //    uint8_t v1 = 2;
 //    uint8_t h2 = 1;
 //    uint8_t v2 = 1;
 //    uint8_t h3 = 1;
 //    uint8_t v3 = 1;

 //    uint32_t *dimensions_MCUs = calcul_dimensions_MCUs_RGB(largeur_image, hauteur_image, h1, v1);
 //    uint32_t nb_MCUs_hauteur, nb_MCUs_largeur;
 //    nb_MCUs_largeur = dimensions_MCUs[0];
 //    nb_MCUs_hauteur = dimensions_MCUs[1];

 //    struct MCU_RGB ***MCUs = decoupage_MCUs(fichier, largeur_image, hauteur_image, nb_MCUs_largeur, nb_MCUs_hauteur, h1, v1, h2, v2, h3, v3);
 //    MCUs = decoupage_MCUs_en_blocs(MCUs, nb_MCUs_largeur, nb_MCUs_hauteur);
 //    print_MCUs_RGB(MCUs, dimensions_MCUs);
 //    free_MCUs_dims_RGB(MCUs, dimensions_MCUs);
 //    fclose(fichier);
 //    return 0;
 //}
