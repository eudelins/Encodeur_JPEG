#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


/************
 * A enlever
 ***********/


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
struct bloc {
    uint8_t largeur;
    uint8_t hauteur;
    struct Pixel_RGB **pixels;
};


/* Défini une MCU */
struct MCU {
    uint8_t largeur;
    uint8_t hauteur;
    struct bloc **blocs;


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

        // On saute une ligne
        char couleurs_max[10];
        fgets(couleurs_max, 10, fichier);

        // On récupère les pixels dans une matrice
        Pixel_RGB **pixels_image = malloc(hauteur_image * sizeof(Pixel_RGB *));
        for (uint32_t hauteur = 0; hauteur < hauteur_image; hauteur++){
            Pixel_RGB *ligne_pixels_image = malloc(largeur_image * sizeof(Pixel_RGB*));
            for (uint32_t largeur = 0; largeur < largeur_image; largeur++){
                ligne_pixels_image[largeur] = *converti_entier_pixel(fgetc(fichier));
            }
            pixels_image[hauteur] = ligne_pixels_image;
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
      printf("%u %u %u \t", pixel.R, pixel.G, pixel.B);
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


/******************
 * Fin d'à enlever
*****************/


/* Vérifie que la valeur de chaque facteur h ou v être comprise entre 1 et 4 : renvoie true si ok, false sinon
 * Fonction à utiliser avant somme_produits_valeurs et diviser_valeurs car on vérifie ici que les valeur h et v sont bien des uint8_t */

bool encadrement_valeurs(uint32_t h1,
                         uint32_t v1,
                         uint32_t h2,
                         uint32_t v2,
                         uint32_t h3,
                         uint32_t v3)
{
    bool encadrement = true;

    if (h1 < 1 || h1 > 4) {
        encadrement = false;
    }
    else if (v1 < 1 || v1 > 4) {
        encadrement = false;
    }
    else if (h2 < 1 || h2 > 4) {
        encadrement = false;
    }
    else if (v2 < 1 || v2 > 4) {
        encadrement = false;
    }
    else if (h3 < 1 || h3 > 4) {
        encadrement = false;
    }
    else if (v3 < 1 || v3 > 4) {
        encadrement = false;
    }

    return encadrement;
}


/* Vérifie que la somme des produits hi x vi est inférieure à 10 : renvoie true si ok, false sinon */

bool somme_produits_valeurs(uint8_t h1,
                            uint8_t v1,
                            uint8_t h2,
                            uint8_t v2,
                            uint8_t h3,
                            uint8_t v3)
{
    bool somme_produits = true;

    if (h1 * v1 + h2 * v2 + h3 * v3 > 10) {
        somme_produits = false;
    }

    return somme_produits;
}


/* Vérifie que les facteurs d'échantillonnage des chrominances divisent ceux de la luminance : renvoie true si ok, false sinon */

bool diviser_valeurs(uint8_t h1,
                     uint8_t v1,
                     uint8_t h2,
                     uint8_t v2,
                     uint8_t h3,
                     uint8_t v3)
{
    bool division = true;

    if (h1 % h2 != 0 || h1 % h3 != 0) {
        division = false;
    }
    else if (v1 % v2 != 0 || v1 % v3 != 0) {
        division = false;
    }

    return division;
}


/* Vérifie que toutes les conditions sur h et v sont vérifiées : renvoie true si ok, false sinon */
bool verif_conditions(uint32_t h1,
                      uint32_t v1,
                      uint32_t h2,
                      uint32_t v2,
                      uint32_t h3,
                      uint32_t v3)
{
    bool conditions = true;

    bool encadrement = encadrement_valeurs(h1, v1, h2, v2, h3, v3);
    if (encadrement == false) {
        printf("Pas le bon encadrement\n");
        conditions = false;
    }
    else {
        bool somme_produits = somme_produits_valeurs(h1, v1, h2, v2, h3, v3);
        if (somme_produits == false) {
            printf("La somme des produits est supérieure à 10\n");
            conditions = false;
        }
        else {
            bool division = diviser_valeurs(h1, v1, h2, v2, h3, v3);
            if (division == false) {
                printf("Les facteurs d'échantillonnage des chrominances ne divisent pas ceux de la luminance\n");
                conditions = false;
            }
            else {
                printf("Tout est ok\n");
            }
        }
    }

    return conditions;
}


/* Partitionne l'image en MCUs de taille h1 x v1 */



int main()
{
    uint8_t h1 = 4;
    uint8_t v1 = 2;
    uint8_t h2 = 1;
    uint8_t v2 = 2;
    uint8_t h3 = 1;
    uint8_t v3 = 1;

    bool conditions = verif_conditions(h1, v1, h2, v2, h3, v3);
    return 0;
}
