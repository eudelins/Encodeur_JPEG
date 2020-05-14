#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


/* Récupère le nom de l'image à encoder en ligne de commande et renvoie le chemin d'accès en chaine de caractères */
char *chemin_fichier(char *nom_fichier) {
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


/* Récupère les paramètes optionnels */
char **paras_optionnels(uint8_t argc, char **argv) {
    if (argc <= 2) {
        char **paras = malloc(12*sizeof(char));
        paras[0] = "pas de paras";
        return paras;
    }

    uint16_t longueur = 0;
    for (uint8_t i = 2; i < argc; i++) {
        longueur += strlen(argv[i]);
    }
    char **paras = malloc(longueur * sizeof(char));

    for(uint8_t i = 2; i < argc; i++) {
        paras[i-2] = argv[i];
    }

    printf("%s, %s\n", paras[0], paras[1]);

    return paras;
}


int main(uint16_t argc, char **argv) {
    char *chemin = chemin_fichier(argv[1]);
    printf("%s\n", chemin);
    free(chemin);

    char **parametres = paras_optionnels(argc, argv);
    free_pointeur(parametres);
    return 0;
}
