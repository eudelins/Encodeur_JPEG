    #include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../include/recup_paras.h"
#include "../include/decoupe.h"
#include "../include/encodage.h"
#include "../include/sous_echantillonnage.h"
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
                      uint32_t *param)
{
    uint32_t largeur_image = param[1], hauteur_image = param[2];

    // Découpage en MCUs
    uint32_t *dimensions_MCUs = calcul_dimensions_MCUs_RGB(largeur_image, hauteur_image, 1, 1);
    uint32_t largeur_MCUs = dimensions_MCUs[0], hauteur_MCUs = dimensions_MCUs[1];
    struct MCU_RGB ***MCUs = decoupage_MCUs(fichier, largeur_image, hauteur_image,
                                            largeur_MCUs, hauteur_MCUs, 1, 1, 1, 1, 1, 1);
    MCUs = decoupage_MCUs_en_blocs(MCUs, largeur_MCUs, hauteur_MCUs);

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


/* Convertit le fichier pgm en jpg dans le cas classique */
void ppm2jpeg_couleur_sous_echantillonnage(FILE *fichier,
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
;
    // Découpage en MCUs
    uint32_t *dimensions_MCUs = calcul_dimensions_MCUs_RGB(largeur_image, hauteur_image, h1, v1);
    uint32_t largeur_MCUs = dimensions_MCUs[0], hauteur_MCUs = dimensions_MCUs[1];
    struct MCU_RGB ***MCUs = decoupage_MCUs(fichier, largeur_image, hauteur_image,
                                            largeur_MCUs, hauteur_MCUs, h1, v1, h2, v2, h3, v3);
    MCUs = decoupage_MCUs_en_blocs(MCUs, largeur_MCUs, hauteur_MCUs);

    // Conversion YCbCr
    struct MCU_YCbCr ***MCUs_YCbCr = conversion_matrice_MCUs(MCUs, largeur_MCUs, hauteur_MCUs);

    // Sous-échantillonage
    struct MCU_YCbCr ***MCUs_YCbCr_sous_echantillone = sous_echantillonne(MCUs_YCbCr, largeur_MCUs, hauteur_MCUs);

    // DCT
    struct MCU_freq_Y ***MCUs_freq = transf_cos_Y(MCUs_YCbCr_sous_echantillone, largeur_MCUs, hauteur_MCUs);

    // Réorganisation zigzag
    struct MCU_zigzag_Y ***MCUs_zigzag = zigzag_Y(MCUs_freq, largeur_MCUs, hauteur_MCUs);

    // Quantification
    quantification_couleur(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);

    // Codage AC-DC
    cree_image_couleur(MCUs_zigzag, largeur_MCUs, hauteur_MCUs,
                       largeur_image, hauteur_image, chemin, chemin_jpg);

    free(param);
    free_MCUs_YCbCr(MCUs_YCbCr, dimensions_MCUs);
    free_MCUs_YCbCr_val(MCUs_YCbCr_sous_echantillone, dimensions_MCUs);
    free_MCUs_zigzag_Y(MCUs_zigzag, largeur_MCUs, hauteur_MCUs);
    free_MCUs_freq_Y(MCUs_freq, largeur_MCUs, hauteur_MCUs);
    free_MCUs_dims_RGB(MCUs, dimensions_MCUs);
    fermer_fichier(fichier);
}


int main(int argc, char **argv)
{

    // Pas de nom de fichier rentré, ni de paramètre
    if (argc == 1) {
        printf("Pas de nom de fichier ni de paramètre rentré\n");
        affichage_erreur();
        return EXIT_FAILURE;
    }

    char *parametres = paras_optionnels(argc, argv);

    // Aucun paramètre optionnel connu
    if (parametres[0] == 'r') {
        FILE *fichier = ouvrir_fichier(argv[1], "r");
        // Si on ne connait pas le fichier
        if (fichier == NULL) {
            if (strncmp(argv[1], "--xx", 2) == 0) {
                printf("Paramètre \"%s\" non connu\n", argv[1]);
                affichage_erreur();
            }
            else {
                printf("Fichier \"%s\" non connu\n", argv[1]);
                affichage_erreur();
            }
            free(parametres);
            return EXIT_FAILURE;
        }

        uint32_t *param = paras(fichier);
        char *chemin_jpg = cree_chemin_jpg(argv[1]);
        if (param[0] == 5){
            ppm2jpeg_niveau_de_gris(fichier, argv[1], chemin_jpg, param);
        } else {
            ppm2jpeg_couleur(fichier, argv[1], chemin_jpg, param);
        }
    }

    // Option renommer le fichier
    else if (parametres[0] == 'o') {
        FILE *fichier = ouvrir_fichier(argv[2], "r");
        // Si on ne connait pas le fichier
        if (fichier == NULL) {
            printf("Fichier \"%s\" non connu\n", argv[2]);
            affichage_erreur();
            free(parametres);
            return EXIT_FAILURE;
        }

        char *chemin_jpg = recup_nom(argv[1], 10);
        uint32_t *param = paras(fichier);
        if (param[0] == 5){
            ppm2jpeg_niveau_de_gris(fichier, argv[2], chemin_jpg, param);
        } else {
            ppm2jpeg_couleur(fichier, argv[2], chemin_jpg, param);
        }
    }

    // option help
    else if (parametres[0] == 'h') {
        affichage_help();
    }

    // option échantillonage
    else if (parametres[0] == 's') {
        FILE *fichier = ouvrir_fichier(argv[2], "r");
        // Si on ne connait pas le fichier
        if (fichier == NULL) {
            printf("Fichier \"%s\" non connu\n", argv[2]);
            affichage_erreur();
            free(parametres);
            return EXIT_FAILURE;
        }

        if (strlen(argv[1]) < 20){
            fprintf(stderr, "Trop peu de facteurs de sous-échantillonage\n");
            affichage_erreur();
            free(parametres);
            return EXIT_FAILURE;
        } else if (strlen(argv[1]) > 20){
            fprintf(stderr, "Trop de facteurs de sous-échantillonage\n");
            affichage_erreur();
            free(parametres);
            return EXIT_FAILURE;
        }

        uint32_t *param = paras(fichier);
        uint32_t h1, v1, h2, v2, h3, v3;
        sscanf(argv[1], "--sample=%ux%u,%ux%u,%ux%u", &h1, &v1, &h2, &v2, &h3, &v3);

        // si on n'a pas de sous_échantillonnage avec les paramètres rentrés
        if (h1 == h2 && h1 == h3 && v1 == v2 && v1 == v3) {
            char *chemin_jpg = cree_chemin_jpg(argv[2]);
            if (param[0] == 5){
                ppm2jpeg_niveau_de_gris(fichier, argv[2], chemin_jpg, param);
            } else {
                ppm2jpeg_couleur(fichier, argv[2], chemin_jpg, param);
            }
        } else {
            bool conditions = verif_conditions(h1, v1, h2, v2, h3, v3);
            if (conditions == false) {
                fprintf(stderr, "\nVérifiez les valeurs entrées pour h1, v1, h2, v2, h3 et v3 : elles ne respectent pas les conditions requises\n\n");
                free(parametres);
                return EXIT_FAILURE;
            }

            char *chemin_jpg = cree_chemin_jpg(argv[2]);
            if (param[0] == 5){
                ppm2jpeg_niveau_de_gris(fichier, argv[2], chemin_jpg, param);
            } else {
                ppm2jpeg_couleur_sous_echantillonnage(fichier, argv[2], chemin_jpg, param, h1, v1, h2, v2, h3, v3);
            }
        }
    }

    // Cas où outfile puis sample
    else if (parametres[0] == 'b') {
       FILE *fichier = ouvrir_fichier(argv[3], "r");
        // Si on ne connait pas le fichier
        if (fichier == NULL) {
            printf("Fichier \"%s\" non connu\n", argv[3]);
            affichage_erreur();
            free(parametres);
            return EXIT_FAILURE;
        }

        uint32_t *param = paras(fichier);
        uint32_t h1, v1, h2, v2, h3, v3;

        if (strlen(argv[2]) < 20){
            fprintf(stderr, "Trop peu de facteurs de sous-échantillonage\n");
            affichage_erreur();
            free(parametres);
            return EXIT_FAILURE;
        } else if (strlen(argv[2]) > 20){
            fprintf(stderr, "Trop de facteurs de sous-échantillonage\n");
            affichage_erreur();
            free(parametres);
            return EXIT_FAILURE;
        }

        sscanf(argv[2], "--sample=%ux%u,%ux%u,%ux%u", &h1, &v1, &h2, &v2, &h3, &v3);

         // si on n'a pas de sous_échantillonnage avec les paramètres drentrés
        if (h1 == h2 && h1 == h3 && v1 == v2 && v1 == v3) {
            char *chemin_jpg = recup_nom(argv[1], 10);
            if (param[0] == 5){
                ppm2jpeg_niveau_de_gris(fichier, argv[3], chemin_jpg, param);
            } else {
                ppm2jpeg_couleur(fichier, argv[3], chemin_jpg, param);
            }
        } else {
            bool conditions = verif_conditions(h1, v1, h2, v2, h3, v3);
            if (conditions == false) {
                fprintf(stderr, "\nVérifiez les valeurs entrées pour h1, v1, h2, v2, h3 et v3 : elles ne respectent pas les conditions requises\n\n");
                free(parametres);
                return EXIT_FAILURE;
            }

            char *chemin_jpg = recup_nom(argv[1], 10);
            if (param[0] == 5){
                ppm2jpeg_niveau_de_gris(fichier, argv[3], chemin_jpg, param);
            } else {
                ppm2jpeg_couleur_sous_echantillonnage(fichier, argv[3], chemin_jpg, param, h1, v1, h2, v2, h3, v3);
            }
        }
    }

    // Cas où sample puis outfile
    else if (parametres[0] == 'q') {
        FILE *fichier = ouvrir_fichier(argv[3], "r");
        // Si on ne connait pas le fichier
        if (fichier == NULL) {
            printf("Fichier \"%s\" non connu\n", argv[3]);
            affichage_erreur();
            free(parametres);
            return EXIT_FAILURE;
        }

        if (strlen(argv[1]) < 20){
            fprintf(stderr, "Trop peu de facteurs de sous-échantillonage\n");
            affichage_erreur();
            free(parametres);
            return EXIT_FAILURE;
        } else if (strlen(argv[1]) > 20){
            fprintf(stderr, "Trop de facteurs de sous-échantillonage\n");
            affichage_erreur();
            free(parametres);
            return EXIT_FAILURE;
        }

        uint32_t *param = paras(fichier);
        uint32_t h1, v1, h2, v2, h3, v3;
        sscanf(argv[1], "--sample=%ux%u,%ux%u,%ux%u", &h1, &v1, &h2, &v2, &h3, &v3);

        // si on n'a pas de sous_échantillonnage avec les paramètres drentrés
        if (h1 == h2 && h1 == h3 && v1 == v2 && v1 == v3) {
            char *chemin_jpg = recup_nom(argv[2], 10);
            if (param[0] == 5){
                ppm2jpeg_niveau_de_gris(fichier, argv[3], chemin_jpg, param);
            } else {
                ppm2jpeg_couleur(fichier, argv[3], chemin_jpg, param);
            }
        } else {
            bool conditions = verif_conditions(h1, v1, h2, v2, h3, v3);
            if (conditions == false) {
                fprintf(stderr, "\nVérifiez les valeurs entrées pour h1, v1, h2, v2, h3 et v3 : elles ne respectent pas les conditions requises\n\n");
                free(parametres);
                return EXIT_FAILURE;
            }

            char *chemin_jpg = recup_nom(argv[2], 10);
            if (param[0] == 5){
                ppm2jpeg_niveau_de_gris(fichier, argv[3], chemin_jpg, param);
            } else {
                ppm2jpeg_couleur_sous_echantillonnage(fichier, argv[3], chemin_jpg, param, h1, v1, h2, v2, h3, v3);
            }
        }
    }

    else if (parametres[0] == 'e') {
        affichage_erreur();
    }

    free(parametres);

    return EXIT_SUCCESS;
}
