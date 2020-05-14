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

/* Prend un fichier ppm en argument et renvoie l'en-tête de ce fichier */
uint32_t *paras(FILE *fichier) {

    char ligne[10];
    uint32_t number;
    char dimensions[10];
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
    fgets(dimensions, 10, fichier);
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

int main() {
    char *ad_fichier = "../images/complexite.pgm";
    FILE *fichier = ouvrir_fichier(ad_fichier, "r");

    uint32_t *parametres;
    parametres = paras(fichier);
    printf("%u, %u, %u, %u\n", parametres[0], parametres[1], parametres[2], parametres[3]);
    free(parametres);
    return 0;

    fermer_fichier(fichier);

}
