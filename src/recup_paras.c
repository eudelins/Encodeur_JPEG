#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


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
    if (argc <= 2) {
        // "r" => rien
        paras[0] = 'r';
        return paras;
    }

    // on regarde quels paras on a
    bool outfile = false;
    uint16_t indice_outfile; // servira pour plus tard, pour récupérer ce qu'il t a après le =

    bool sample = false;
    uint16_t indice_sample;

    uint16_t longueur = 0;

    for (uint8_t i = 2; i < argc; i++) {

        char *chaine_help = "--help";
        if (strcmp(argv[i], chaine_help) == 0) {
            // Si on a au moins un --help -> prime sur les autres
            paras[0] = 'h';
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
            // "e" => erreur (ici un argument non connu)
            paras[0] = 'e';
            return paras;
        }
    }

    if (longueur >= 3) {
        // "e" => erreur
        paras[0] = 'e';
        return paras;
    }
    else if (longueur == 1) {
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
    else if (longueur == 2) {
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


int main(uint16_t argc, char **argv) {

    char *chemin = chemin_fichier(argv[1]);
    printf("%s\n", chemin);
    free(chemin);

    char *parametres = paras_optionnels(argc, argv);
    printf("%c\n", *parametres);
    free(parametres);
    return 0;
}
