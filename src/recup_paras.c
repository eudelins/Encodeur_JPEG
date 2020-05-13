#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Ouvre le fichier filename avec le mode d'accès mode. Retourne le FILE *
 * correspondant. */
static FILE *ouvrir_fichier(const char *filename, const char *mode) {
     FILE *fichier = fopen(filename, mode);
     return fichier;
}

/* Ferme le fichier passé en paramètre. */
static void fermer_fichier(FILE *fichier) {
     int ret = fclose(fichier);
}

/* Prend un fichier ppm en argument sur ligne de commande et renvoie l'en-tête de ce fichier */
void main(int argc, char **argv) {
    FILE *fichier = ouvrir_fichier("../jpeg_boulhole_rousseau_eudelins/images/zig-zag.ppm", "r");

    char dimensions[10];
    char ligne[10];
    char largeur[10];
    char hauteur[10];
    char nb_vals[10];
    char *chaine_trouvee;

    /* On récupère les dimensions du fichier */
    fgets(dimensions, 10, fichier);
    printf("magic number : %s \n", dimensions);

    /* On récupère la largeur et la hauteur du fichier */
    fgets(ligne, 10, fichier);
    sscanf(ligne, "%s %s", &largeur, &hauteur);
    printf("largeur : %s, hauteur : %s\n\n", largeur, hauteur);

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
    printf("nombre de valeurs d'une composante : %s \n", nb_vals);

    fermer_fichier(fichier);
}
