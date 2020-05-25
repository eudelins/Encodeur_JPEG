#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


/* Ouvre le fichier filename avec le mode d'accès mode. Retourne le FILE *
 * correspondant. */
FILE *ouvrir_fichier(const char *filename, const char *mode) {
    FILE *fichier = fopen(filename, mode);
}


/* Ferme le fichier passé en paramètre. */
void fermer_fichier(FILE *fichier) {
     int ret = fclose(fichier);
}


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


/* Texte à afficher si on a un --help en argument */
void affichage_help()
{
    printf("\nUtilisation : ./ppm2jpeg [IMAGE] [--OPTION] \n"
    "Encoder l'image PPM en une image JPEG \n \n \n"
    "--help                         afficher l'aide et quitter \n \n"
    "--outfile=sortie.jpg           redéfinir le nom du fichier de sortie \n \n"
    "--sample=h1xv1,h2xv2,h3xv3     définir les facteurs d'échantillonnage hxv des trois composantes de couleur \n \n \n"
    "Exemples : \n"
    "ppm2jpeg invaders.ppm          encode l'image 'invaders' en une image jpeg \n \n");
}


/* Texte à afficher si on a une erreur d'argument */
void affichage_erreur()
{
printf("\nUtilisez ./ppm2jpeg [--OPTION] [FICHIER]\n"
        "ou \n"
        "./ppm2jpeg --help pour ouvrir l'aide \n \n");


/* Récupère les paramètes optionnels
 * Renvoie "h" si help, "o" si outfile, "s" si sample, "e" si erreur, "b" si outfile puis sample, "q" si sample puis outfile */
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
    uint8_t indice_outfile;
    bool sample = false;
    uint16_t longueur = 0;

    for (uint8_t i = 1; i < argc; i++) {

        char *chaine_help = "--help";
        if (strcmp(argv[i], chaine_help) == 0) {
            // Si on a au moins un --help -> prime sur les autres
            paras[0] = 'h';
            return paras;
        }
        else if (strncmp(argv[i], "--samplexx", 6) == 0) {
            sample = true;
            longueur += 1;
        }
        else if (strncmp(argv[i], "--outfilexx", 7) == 0) {
            outfile = true;
            indice_outfile = i;
            longueur += 1;
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
            if (indice_outfile == 1) {
                // "b" => deux outfile => on a outfile et sample avec outfile en premier
                paras[0] = 'b';
            }
            else {
                // "q" => deux sample => on a outfile ET sample avec sample en premier
                paras[0] = 'q';
            }
            return paras;
        }
        else {
            // "e" => erreur (ici 2 fois le même argument)
            paras[0] = 'e';
            return paras;
        }
    }
}



/* Vérifie que la valeur de chaque facteur h ou v être comprise entre 1 et 4 : renvoie true si ok, false sinon
 * Fonction à utiliser avant somme_produits_valeurs et diviser_valeurs car on vérifie ici que les valeur h et v sont bien des uint8_t */
bool encadrement_valeurs(uint32_t h1,
                         uint32_t v1,
                         uint32_t h2,
                         uint32_t v2,
                         uint32_t h3,
                         uint32_t v3)
{
    bool encadrement = true;

    if (h1 < 1 || h1 > 4) {
        encadrement = false;
    }
    else if (v1 < 1 || v1 > 4) {
        encadrement = false;
    }
    else if (h2 < 1 || h2 > 4) {
        encadrement = false;
    }
    else if (v2 < 1 || v2 > 4) {
        encadrement = false;
    }
    else if (h3 < 1 || h3 > 4) {
        encadrement = false;
    }
    else if (v3 < 1 || v3 > 4) {
        encadrement = false;
    }

    return encadrement;
}


/* Vérifie que la somme des produits hi x vi est inférieure à 10 : renvoie true si ok, false sinon */
bool somme_produits_valeurs(uint8_t h1,
                            uint8_t v1,
                            uint8_t h2,
                            uint8_t v2,
                            uint8_t h3,
                            uint8_t v3)
{
    bool somme_produits = true;

    if (h1 * v1 + h2 * v2 + h3 * v3 > 10) {
        somme_produits = false;
    }

    return somme_produits;
}


/* Vérifie que les facteurs d'échantillonnage des chrominances divisent ceux de la luminance : renvoie true si ok, false sinon */
bool diviser_valeurs(uint8_t h1,
                     uint8_t v1,
                     uint8_t h2,
                     uint8_t v2,
                     uint8_t h3,
                     uint8_t v3)
{
    bool division = true;

    if (h1 % h2 != 0 || h1 % h3 != 0) {
        division = false;
    }
    else if (v1 % v2 != 0 || v1 % v3 != 0) {
        division = false;
    }

    return division;
}


/* Vérifie que toutes les conditions sur h et v sont vérifiées : renvoie true si ok, false sinon */
bool verif_conditions(uint32_t h1,
                      uint32_t v1,
                      uint32_t h2,
                      uint32_t v2,
                      uint32_t h3,
                      uint32_t v3)
{
    bool conditions = true;

    bool encadrement = encadrement_valeurs(h1, v1, h2, v2, h3, v3);
    if (encadrement == false) {
        printf("Pas le bon encadrement\n");
        conditions = false;
    }
    else {
        bool somme_produits = somme_produits_valeurs(h1, v1, h2, v2, h3, v3);
        if (somme_produits == false) {
            printf("La somme des produits est supérieure à 10\n");
            conditions = false;
        }
        else {
            bool division = diviser_valeurs(h1, v1, h2, v2, h3, v3);
            if (division == false) {
                printf("Les facteurs d'échantillonnage des chrominances ne divisent pas ceux de la luminance\n");
                conditions = false;
            }
            else {
                printf("Tout est ok\n");
            }
        }
    }

    return conditions;
}


/* Récupère les éléments de chaine à partir de indice_recup */
char *recup_nom(char *chaine, uint8_t indice_recup)
{
    char *sortie;
    for (uint8_t i = indice_recup, j = 0; i < strlen(chaine) + 1; i++, j++)
    {
        sortie[j] = chaine[i];
    }
    return sortie;
}


//int main(uint16_t argc, char **argv) {
//
//    char *parametres = paras_optionnels(argc, argv);
//    if (parametres[0] == 'r') {
//        char *chemin = chemin_fichier(argv[1]);
//        FILE *fichier = ouvrir_fichier(chemin, "r");
//        if (fichier == NULL) {
//            printf("Pas de fichier/Fichier non connu\n");
//        }
//        else {
//            uint32_t *param;
//            param = paras(fichier);
//            free(param);
//            fermer_fichier(fichier);
//        }
//    }
//    else if (parametres[0] == 's') {
//        char *chemin = chemin_fichier(argv[2]);
//        FILE *fichier = ouvrir_fichier(chemin, "r");
//        if (fichier == NULL) {
//            printf("Fichier non connu\n");
//        }
//        else {
//            uint32_t *param;
//            param = paras(fichier);
//
//            uint32_t h1;
//            uint32_t v1;
//            uint32_t h2;
//            uint32_t v2;
//            uint32_t h3;
//            uint32_t v3;
//
//            sscanf(argv[1], "--sample=%ux%u,%ux%u,%ux%u", &h1, &v1, &h2, &v2, &h3, &v3);
//            bool conditions = verif_conditions(h1, v1, h2, v2, h3, v3);
//            if (conditions == false) {
//                printf("\nVérifiez les valeurs entrées pour h1, v1, h2, v2, h3 et v3 : elles ne respectent pas les conditions requises\n\n");
//            }
//
//            free(param);
//            fermer_fichier(fichier);
//        }
//    }
//    else if (parametres[0] == 'o') {
//        FILE *fichier = ouvrir_fichier(argv[2], "r");
//        if (fichier == NULL) {
//            printf("Fichier non connu/Chemin non connu\n");
//        }
//        else {
//            uint32_t *param;
//            param = paras(fichier);
//
//            char *sortie = recup_nom(argv[1], 10);
//            printf("%s\n", sortie);
//            free(param);
//            fermer_fichier(fichier);
//        }
//
//    }
//    else if (parametres[0] == 'b') {
//        // cas où outfile puis sample
//        FILE *fichier = ouvrir_fichier(argv[3], "r");
//        if (fichier == NULL) {
//            printf("Fichier non connu/Chemin non connu\n");
//        }
//        else {
//            uint32_t *param;
//            param = paras(fichier);
//
//            char *sortie = recup_nom(argv[1], 10);
//            printf("%s\n", sortie);
//
//            uint32_t h1;
//            uint32_t v1;
//            uint32_t h2;
//            uint32_t v2;
//            uint32_t h3;
//            uint32_t v3;
//
//            sscanf(argv[2], "--sample=%ux%u,%ux%u,%ux%u", &h1, &v1, &h2, &v2, &h3, &v3);
//            bool conditions = verif_conditions(h1, v1, h2, v2, h3, v3);
//            if (conditions == false) {
//                printf("\nVérifiez les valeurs entrées pour h1, v1, h2, v2, h3 et v3 : elles ne respectent pas les conditions requises\n\n");
//            }
//           free(param);
//           fermer_fichier(fichier);
//        }
//    }
//    else if (parametres[0] == 'q') {
//        // cas où sample puis outfile
//        FILE *fichier = ouvrir_fichier(argv[3], "r");
//        if (fichier == NULL) {
//            printf("Fichier non connu/Chemin non connu\n");
//        }
//        else {
//            uint32_t *param;
//            param = paras(fichier);
//
//            char *sortie = recup_nom(argv[2], 10);
//            printf("%s\n", sortie);
//
//            uint32_t h1;
//            uint32_t v1;
//            uint32_t h2;
//            uint32_t v2;
//            uint32_t h3;
//            uint32_t v3;
//
//            sscanf(argv[1], "--sample=%ux%u,%ux%u,%ux%u", &h1, &v1, &h2, &v2, &h3, &v3);
//            bool conditions = verif_conditions(h1, v1, h2, v2, h3, v3);
//            if (conditions == false) {
//                printf("\nVérifiez les valeurs entrées pour h1, v1, h2, v2, h3 et v3 : elles ne respectent pas les conditions requises\n\n");
//            }
//            else {
//                printf("%ux%u,%ux%u,%ux%u",h1, v1, h2, v2, h3, v3);
//            }
//           free(param);
//           fermer_fichier(fichier);
//        }
//    }
//    else if (parametres[0] == 'e') {
//    affichage_erreur()
//    }
//    free(parametres);
//
//    return 0;
//}
