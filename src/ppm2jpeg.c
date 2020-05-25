#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../include/decoupe_tout.h"
#include "../include/encodage.h"
#include "../include/recup_paras.h"
#include "../include/ecriture.h"
#include "../include/ecriture_couleur.h"


/* Convertit le fichier pgm en jpg dans le cas classique */
void ppm2jpeg_niveau_de_gris(FILE *fichier, char *chemin, char *chemin_jpg, uint32_t *param)
{
    uint32_t largeur_image = param[1], hauteur_image = param[2];

    // Découpage en MCUs
    uint32_t *dimensions_MCUs = calcul_dimensions_MCUs(largeur_image, hauteur_image, 8, 8);
    uint32_t largeur_MCUs = dimensions_MCUs[0], hauteur_MCUs = dimensions_MCUs[1];
    struct MCU ***MCUs = decoupage(fichier, largeur_MCUs, hauteur_MCUs, largeur_image, hauteur_image);

    // DCT
    struct MCU_freq ***MCUs_freq = transf_cos(MCUs, largeur_MCUs, hauteur_MCUs);

    // Réorganisation zigzag
    struct MCU_zigzag ***MCUs_zigzag = zigzag(MCUs_freq, largeur_MCUs, hauteur_MCUs);
    
    // Quantification
    quantification(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);

    // Codage AC-DC
    cree_image(MCUs_zigzag, largeur_MCUs, hauteur_MCUs, largeur_image, hauteur_image, chemin, chemin_jpg);

    free_MCUs_zigzag(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);
    free_MCUs_freq(MCUs_freq, largeur_MCUs, hauteur_MCUs);
    free_MCUs(MCUs, dimensions_MCUs);
    free(param); 
    fermer_fichier(fichier);   
}


/* Convertit le fichier pgm en jpg dans le cas classique */
void ppm2jpeg_couleur(FILE *fichier,
                      char *chemin,
                      char *chemin_jpg,
                      uint32_t *param,
                      uint8_t h1,
                      uint8_t v1,
                      uint8_t h2,
                      uint8_t v2,
                      uint8_t h3,
                      uint8_t v3)
{
    uint32_t largeur_image = param[1], hauteur_image = param[2];

    // Découpage en MCUs
    uint32_t *dimensions_MCUs = calcul_dimensions_MCUs_RGB(largeur_image, hauteur_image, 1, 1);
    uint32_t largeur_MCUs = dimensions_MCUs[0], hauteur_MCUs = dimensions_MCUs[1];
    struct MCU_RGB ***MCUs = decoupage_MCUs(fichier, largeur_image, hauteur_image,
                                            largeur_MCUs, hauteur_MCUs, h1, v1, h2, v2, h3, v3);
    MCUs = decoupage_MCUs_en_blocs(MCUs, largeur_MCUs, hauteur_MCUs, 1, 1);

    // Conversion YCbCr
    struct MCU_YCbCr ***MCUs_YCbCr = conversion_matrice_MCUs(MCUs, largeur_MCUs, hauteur_MCUs);
    
    // DCT
    struct MCU_freq_Y ***MCUs_freq = transf_cos_Y(MCUs_YCbCr, largeur_MCUs, hauteur_MCUs);

    // Réorganisation zigzag
    struct MCU_zigzag_Y ***MCUs_zigzag = zigzag_Y(MCUs_freq, largeur_MCUs, hauteur_MCUs);
    
    // Quantification
    quantification_couleur(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);

    // Codage AC-DC
    cree_image_couleur(MCUs_zigzag, largeur_MCUs, hauteur_MCUs,
                       largeur_image, hauteur_image, chemin, chemin_jpg);

    free(param);
    free_MCUs_YCbCr(MCUs_YCbCr, dimensions_MCUs);
    free_MCUs_zigzag_Y(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);
    free_MCUs_freq_Y(MCUs_freq, largeur_MCUs, hauteur_MCUs);
    free_MCUs_dims_RGB(MCUs, dimensions_MCUs);
    fermer_fichier(fichier);
}



int main(uint8_t argc, char **argv)
{

    char *parametres = paras_optionnels(argc, argv);
    
    // Aucun paramètre optionnel
    if (parametres[0] == 'r') {
        // char *chemin = chemin_fichier(argv[1]);
        FILE *fichier = ouvrir_fichier(argv[1], "r");
        // Si on ne connait pas le fichier    
        if (fichier == NULL) {
            printf("Fichier \"%s\" non connu\n", argv[1]);
            affichage_erreur();
            return EXIT_FAILURE;
        }

        uint32_t *param = paras(fichier);
        char *chemin_jpg = cree_chemin_jpg(argv[1]);
        if (param[0] == 5){
            ppm2jpeg_niveau_de_gris(fichier, argv[1], chemin_jpg, param);
        } else {
            ppm2jpeg_couleur(fichier, argv[1], chemin_jpg, param, 1, 1, 1, 1, 1, 1);
        }
    }
    
    // Option renommer le fichier
    else if (parametres[0] == 'o') {
        FILE *fichier = ouvrir_fichier(argv[2], "r");
        // Si on ne connait pas le fichier    
        if (fichier == NULL) {
            printf("Fichier \"%s\" non connu\n", argv[2]);
            affichage_erreur();
            return EXIT_FAILURE;
        }
        
        char *chemin_jpg = recup_nom(argv[1], 10);
        uint32_t *param = paras(fichier);
        if (param[0] == 5){
            ppm2jpeg_niveau_de_gris(fichier, argv[2], chemin_jpg, param);
        } else {
            ppm2jpeg_couleur(fichier, argv[2], chemin_jpg, param, 1, 1, 1, 1, 1, 1);
        }
    }

    else if (parametres[0] == 'h') {
        affichage_help();
    }

    // else if (parametres[0] == 's') {
    //     char *chemin = chemin_fichier(argv[2]);
    //     FILE *fichier = ouvrir_fichier(chemin, "r");
    //     if (fichier == NULL) {
    //         printf("Fichier non connu\n");
    //     }
    //     else {
    //         uint32_t *param;
    //         param = paras(fichier);
    //         printf("%u, %u, %u, %u\n", param[0], param[1], param[2], param[3]);
    //         free(param);
    //         fermer_fichier(fichier);
    //     }
    //         free(chemin);
    // }
    // else if (parametres[0] == 'd') {
    //     char *chemin = chemin_fichier(argv[3]);
    //     FILE *fichier = ouvrir_fichier(chemin, "r");
    //     if (fichier == NULL) {
    //         printf("Fichier non connu/Chemin non connu\n");
    //     }
    //     else {
    //         uint32_t *param;
    //         param = paras(fichier);
    //         printf("%u, %u, %u, %u\n", param[0], param[1], param[2], param[3]);
    //         free(param);
    //         fermer_fichier(fichier);
    //     }
    //         free(chemin);
    // }
    
    else if (parametres[0] == 'e') {
      affichage_erreur();
    }

    free(parametres);
    
    return EXIT_SUCCESS;
}