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

// Pour l'insntant, pixel = uint8_t, mais après, définir une struct RGB et YCbCr


/* Découpe l'image en MCUs */
struct MCU **decoupage(FILE *fichier)
{
    char en_tete[10];
    fgets(en_tete, 10, fichier);
    printf("%s", en_tete);

    char dimensions[10];
    uint32_t largeur_image, hauteur_image;
    fgets(dimensions, 10, fichier);
    printf("%s", dimensions);
    sscanf(dimensions, "%u %u", &largeur_image, &hauteur_image);
    printf("%u %u\n", largeur_image, hauteur_image);

    struct MCU **MCUs;
    return MCUs;
}


int main(void)
{
    FILE *fichier = ouvrir_fichier("../images/invader.pgm", "r");
    decoupage(fichier);


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
