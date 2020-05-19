#ifndef RECUP_PARAS_H
#define RECUP_PARAS_H


/* Prend un fichier ppm en argument et renvoie l'en-tête de ce fichier */
uint32_t *paras(FILE *fichier);


/* Récupère le nom de l'image à encoder en ligne de commande et renvoie le chemin d'accès en chaine de caractères */
char *chemin_fichier(char *nom_fichier);


/* Récupère les paramètes optionnels
 * Renvoie "h" si help, "o" si outfile, "s" si sample, "e" si erreur*/
char *paras_optionnels(uint8_t argc, char **argv);


#endif /* RECUP_PARAS_H */