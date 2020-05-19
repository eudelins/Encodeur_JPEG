#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


/* Ouvre le fichier filename avec le mode d'accès mode. Retourne le FILE *
 * correspondant. */
// FILE *ouvrir_fichier(const char *filename, const char *mode) {
//     FILE *fichier = fopen(filename, mode);
// }


// /* Ferme le fichier passé en paramètre. */
// void fermer_fichier(FILE *fichier) {
//      int ret = fclose(fichier);
// }


/* Prend un fichier ppm en argument et renvoie l'en-tête de ce fichier */
uint32_t *paras(FILE *fichier) {

    char ligne[10];
    uint32_t number;
    char dimensions[30];
    uint32_t largeur;
    uint32_t hauteur;
    char nb_vals[10];
    uint32_t vals;
    char *chaine_trouvee;

    uint32_t *en_tete;
    en_tete = malloc(4*sizeof(uint32_t));

    /* On récupère le magic number du fichier */
    fgets(ligne, 10, fichier);
    sscanf(ligne, "P%u", &number);
    en_tete[0] = number;

    /* On récupère la largeur et la hauteur du fichier */
    fgets(dimensions, 30, fichier);
    sscanf(dimensions, "%u %u", &largeur, &hauteur);
    en_tete[1] = largeur;
    en_tete[2] = hauteur;

    /* On récupère le nombre de valeurs d'une composante
     * Pour faire face à un problème qui survient dans certains fichiers,
     * on se sert de la fonction strpbrk qui renvoie un pointeur vers la première
     * occurence d'une certaine chaîne de caractères
     * Ici, ce qui peut nous embêter c'est un saut de ligne "en trop" c'est-à-dire
     * en tout début de ligne
     * On regarde donc si ce pointeur est le même que notre pointeur vers
     * le premier caractère. Si c'est le cas, on reécupère la ligne d'après */
    fgets(nb_vals, 10, fichier);
    chaine_trouvee = strpbrk(nb_vals, "\n");
    if (chaine_trouvee == nb_vals) {
        fgets(nb_vals, 10, fichier);
    }
    sscanf(nb_vals, "%u", &vals);
    en_tete[3] = vals;

    return en_tete;

   }


/* Récupère le nom de l'image à encoder en ligne de commande et renvoie le chemin d'accès en chaine de caractères */
char *chemin_fichier(char *nom_fichier) {
    if (nom_fichier == NULL) {
        char *chemin = malloc(14 * sizeof(char));
        chemin = "pas de fichier";
        return chemin;
    }
    else {
        char *chemin =  malloc((11 + strlen(nom_fichier)) * sizeof(char));
        strcpy(chemin, "../images/");
        strcat(chemin, nom_fichier);
        return chemin;
    }
}


/* Récupère les paramètes optionnels
 * Renvoie "h" si help, "o" si outfile, "s" si sample, "e" si erreur*/
char *paras_optionnels(uint8_t argc, char **argv) {

    char *paras = malloc(sizeof(char));

    // on vérifie qu'on a bien des paras optionnels
    if (argc <= 1) {
        // "r" => rien
        paras[0] = 'r';
        return paras;
    }

    // si on a que 2 arguemnts : 2 choix possibles :
    // on peut avoir --help
    // on peut avoir le nom du fichier à encoder
    else if (argc == 2) {
        char *chaine_help = "--help";
        if (strcmp(argv[1], chaine_help) == 0) {
            paras[0] = 'h';
            printf("\nUtilisation : ./ppm2jpeg [FICHIER] [OPTION] \n"
            "Encoder l'image PPM en une image JPEG \n \n \n"
            "--help                         afficher l'aide et quitter \n \n"
            "--outfile=sortie.jpg           redéfinir le nom du fichier de sortie \n \n"
            "--sample=h1xv1,h2xv2,h3xv3     définir les facteurs d'échantillonnage hxv des trois composantes de couleur \n \n \n"
            "Exemples : \n"
            "ppm2jpeg invaders.ppm          encode l'image 'invaders' en une image jpeg \n \n");
            return paras;
        }
        else {
            // "r" => rien
            paras[0] = 'r';
            return paras;
        }
    }

    // on regarde quels paras on a
    bool outfile = false;
    uint16_t indice_outfile; // servira pour plus tard, pour récupérer ce qu'il t a après le =

    bool sample = false;
    uint16_t indice_sample;

    uint16_t longueur = 0;

    for (uint8_t i = 1; i < argc; i++) {

        char *chaine_help = "--help";
        if (strcmp(argv[i], chaine_help) == 0) {
            // Si on a au moins un --help -> prime sur les autres
            paras[0] = 'h';
            printf("\nUtilisation : ./ppm2jpeg [IMAGE] [--OPTION] \n"
            "Encoder l'image PPM en une image JPEG \n \n \n"
            "--help                         afficher l'aide et quitter \n \n"
            "--outfile=sortie.jpg           redéfinir le nom du fichier de sortie \n \n"
            "--sample=h1xv1,h2xv2,h3xv3     définir les facteurs d'échantillonnage hxv des trois composantes de couleur \n \n \n"
            "Exemples : \n"
            "ppm2jpeg invaders.ppm          encode l'image 'invaders' en une image jpeg \n \n");
            return paras;
        }
        else if (strncmp(argv[i], "--samplexx", 6) == 0) {
            sample = true;
            longueur += 1;
            indice_sample = i;
        }
        else if (strncmp(argv[i], "--outfilexx", 7) == 0) {
            outfile = true;
            longueur += 1;
            indice_outfile = i;
        }
        else {
            longueur += 1;
        }
    }

    if ((longueur >= 4) || (longueur == 1)) {
        // "e" => erreur
        paras[0] = 'e';
        return paras;
    }
    else if (longueur == 2) {
        if (outfile == true) {
           // "o" => outfile
           paras[0] = 'o';
           return paras;
        }
        if (sample == true) {
            // "s" => sample
            paras[0] = 's';
            return paras;
        }
    }
    else if (longueur == 3) {
        // on vérifie qu'on a bien outfile et sample
        if ((sample == true) && (outfile == true)) {
            // "d" => deux => on a outfile ET sample
            paras[0] = 'd';
            return paras;
        }
        else {
            // "e" => erreur (ici 2 fois le même argument)
            paras[0] = 'e';
            return paras;
        }
    }
}


/* à mettre dans le main
 * cas 1 : si c'est outfile 'o' */
// char *sortie;
// char *poubelle;
// sscanf(argv[2], "%s=%s", poubelle, sortie);
// sortie correspond au chemin + nom du nouveau fichier jpg


/* à mettre dans le main
 * cas 2 : si c'est sample 's' */
// char *valeurs;
// sscanf(argv[2], "%s=%s", poublle, valeurs);
// sortie correspond à h1xv1...



// int main(uint16_t argc, char **argv) {

//     char *parametres = paras_optionnels(argc, argv);
//     // printf("%c\n", *parametres);
//     if (parametres[0] == 'r') {
//         char *chemin = chemin_fichier(argv[1]);
//         FILE *fichier = ouvrir_fichier(chemin, "r");
//         if (fichier == NULL) {
//             printf("Fichier non connu\n");
//         }
//         else {
//             uint32_t *param;
//             param = paras(fichier);
//             printf("%u, %u, %u, %u\n", param[0], param[1], param[2], param[3]);
//             free(param);
//             fermer_fichier(fichier);
//         }
//             free(chemin);
//     }
//     else if (parametres[0] == 's') {
//         char *chemin = chemin_fichier(argv[2]);
//         FILE *fichier = ouvrir_fichier(chemin, "r");
//         if (fichier == NULL) {
//             printf("Fichier non connu\n");
//         }
//         else {
//             uint32_t *param;
//             param = paras(fichier);
//             printf("%u, %u, %u, %u\n", param[0], param[1], param[2], param[3]);
//             free(param);
//             fermer_fichier(fichier);
//         }
//             free(chemin);
//     }
//     else if (parametres[0] == 'o') {
//         FILE *fichier = ouvrir_fichier(argv[2], "r");
//         if (fichier == NULL) {
//             printf("Fichier non connu/Chemin non connu\n");
//         }
//         else {
//             uint32_t *param;
//             param = paras(fichier);
//             printf("%u, %u, %u, %u\n", param[0], param[1], param[2], param[3]);
//             free(param);
//             fermer_fichier(fichier);
//         }

//     }
//     else if (parametres[0] == 'd') {
//         char *chemin = chemin_fichier(argv[3]);
//         FILE *fichier = ouvrir_fichier(chemin, "r");
//         if (fichier == NULL) {
//             printf("Fichier non connu/Chemin non connu\n");
//         }
//         else {
//             uint32_t *param;
//             param = paras(fichier);
//             printf("%u, %u, %u, %u\n", param[0], param[1], param[2], param[3]);
//             free(param);
//             fermer_fichier(fichier);
//         }
//             free(chemin);
//     }
//     else if (parametres[0] == 'e') {
//         printf("\nUtilisez ./ppm2jpeg [FICHIER] [--OPTION] \n"
//         "ou \n"
//         "./ppm2jpeg --help pour ouvrir l'aide \n \n");
//     }
//     free(parametres);

//     return 0;
// }
