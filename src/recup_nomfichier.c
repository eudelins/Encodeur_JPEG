#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/* Récupère le nom de l'image à encoder en ligne de commande et renvoie le chemin d'accès en chaine de caractères */
char **chemin_fichier(char *nom_fichier) {
    char **chemin =  malloc(22*sizeof(char));
    *chemin = strcat("../images/", nom_fichier);
    return chemin;
}


int main() {
    char *nom_fichier = "invaders.pgm";

    char **chemin = chemin_fichier(nom_fichier);
    printf("%s\n", *chemin);

    free(chemin);

    return 0;
}
