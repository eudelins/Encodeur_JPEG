#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


/* Récupère le nom de l'image à encoder en ligne de commande et renvoie le chemin d'accès en chaine de caractères */
char *chemin_fichier(char *nom_fichier) {
    if (nom_fichier == NULL) {
        char *chemin = malloc(14 * sizeof(char));
        chemin = "pas de fichier";
        return chemin;
    }

    char *chemin =  malloc((11 + strlen(nom_fichier)) * sizeof(char));
    strcpy(chemin, "../images/");
    strcat(chemin, nom_fichier);
    return chemin;
}


/* Free un pointeur de pointeurs */
void free_pointeur(char **pointeur) {
    for (uint8_t i = 0; i < strlen(*pointeur); i++) {
        free(*pointeur);
    }
}


/* Récupère une chaine de caractères de type --chaine et renvoie le premier caractère de chaine*/
char premier_caractr(char *chaine) {
    char prem;
    sscanf(chaine, "--%c", prem);
    return prem;
}



///* Récupère les paramètes optionnels */
//char *paras_optionnels(uint8_t argc, char **argv) {
//    if (argc <= 2) {
//        char *paras = malloc(sizeof(char));
//        paras = "r";
//        return paras;
//    }
//
//    bool help = false;
//    bool outfile = false;
//    bool sample = false;
//    uint16_t longueur = 0;
//    for (uint8_t i = 2; i < argc; i++) {
//        if (argv[i] == "--help") {
//            help = true;
//        }
//        elif (argv[i] == "--"
//        longueur += strlen(argv[i]);
//    }
//
//    /* Si on a au moins un --help -> prime sur les autres */
//    if (help == true) {
//        char *paras = malloc(sizeof(char));
//        paras = "h";
//    }
//    else {
//        char **paras = malloc(longueur * sizeof(char));
//    }
//
//    for(uint8_t i = 2; i < argc; i++) {
//        paras[i-2] = argv[i];
//    }
//
//
//    return paras;
//}
//

int main(uint16_t argc, char **argv) {

    char prem = premier_caractr(argv[1]);
    printf("%c\n", prem);

    //char *chemin = chemin_fichier(argv[1]);
    //printf("%s\n", chemin);
    //free(chemin);

    //char **parametres = paras_optionnels(argc, argv);
    //free_pointeur(parametres);
    return 0;
}
