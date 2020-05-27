#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "../include/decoupe_couleur.h"
#include "../include/conversion_YCbCr.h"


/* Libère la mémoire allouée à un bloc de pixels */
void free_bloc(struct Bloc_YCbCr bloc)
{
  for (uint8_t hauteur = 0; hauteur < 8; hauteur ++) {
    free(bloc.pixels[hauteur]);
  }
  free(bloc.pixels);
}

/* Libère la mémoire allouée à une matrice de blocs en connaissant ses dimensions */
void free_blocs(struct Bloc_YCbCr **blocs, uint8_t hi, uint8_t vi)
{
  for (uint8_t hauteur = 0; hauteur < vi; hauteur++) {
    for (uint8_t largeur = 0; largeur < hi; largeur++) {
      free_bloc(blocs[hauteur][largeur]);
    }
    free(blocs[hauteur]);
  }
  free(blocs);
}

/* Libère la mémoire allouée à une MCU_YCbCr */
void free_MCU(struct MCU_YCbCr *MCU_YCbCr)
{
  free_blocs(MCU_YCbCr->blocs_Y, MCU_YCbCr->h1, MCU_YCbCr->v1);
  free_blocs(MCU_YCbCr->blocs_Cb, MCU_YCbCr->h2, MCU_YCbCr->v2);
  free_blocs(MCU_YCbCr->blocs_Cr, MCU_YCbCr->h3, MCU_YCbCr->v3);
  free(MCU_YCbCr);
}


/* Libère la mémoire allouée à une matrice de MCUs YCbCr en connaissant ses dimensions */
void free_MCUs_YCbCr_val(struct MCU_YCbCr ***matrice_MCUs_sous_ech,
                     uint32_t *dimensions_MCUs)
{
    uint32_t nb_MCUs_largeur = dimensions_MCUs[0];
    uint32_t nb_MCUs_hauteur = dimensions_MCUs[1];

    for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur; hauteur++) {
        for (uint32_t largeur = 0; largeur < nb_MCUs_largeur; largeur++) {
            free_MCU(matrice_MCUs_sous_ech[hauteur][largeur]);
        }
        free(matrice_MCUs_sous_ech[hauteur]);
    }
    free(matrice_MCUs_sous_ech);
}

/*******************************************/
/* Partie consacrée au sous-échantillonnage */
/*******************************************/


/* Fait la moyenne sur un certain nombre de pixels Cb alignés horizontalement */
int16_t moyenne_horizontale_Cb(struct MCU_YCbCr *MCU_YCbCr,
                                      uint8_t hauteur_pix,
                                      uint8_t largeur_pix,
                                      uint8_t hauteur,
                                      uint8_t largeur)
{
/* En fonction du rapport h1/h2, fait une moyenne horizontale sur 1, 2, 3 ou 4 pixels Cb */
  uint8_t div = MCU_YCbCr->h1/MCU_YCbCr->h2;
  int16_t somme = 0;

  /* Cas ou le rapport vaut 3 : cas spécial car les pixels dont on fait
  la moyenne sont sur des blocs différents*/
  if (div == 3) {
    if (largeur_pix < 2) {
      for (uint8_t pixel = 0; pixel < 3; pixel++) {
        somme += MCU_YCbCr->blocs_Cb[hauteur][largeur_pix/3].pixels[hauteur_pix][largeur_pix * 3 + pixel];
      }
    }
    else if (largeur_pix == 2) {
      somme += MCU_YCbCr->blocs_Cb[hauteur][largeur_pix/3].pixels[hauteur_pix][6];
      somme += MCU_YCbCr->blocs_Cb[hauteur][largeur_pix/3].pixels[hauteur_pix][7];
      somme += MCU_YCbCr->blocs_Cb[hauteur][1].pixels[hauteur_pix][0];
    }
    else if (largeur_pix > 2 && largeur_pix < 5) {
      for (uint8_t pixel = 0; pixel < 3; pixel++) {
        somme += MCU_YCbCr->blocs_Cb[hauteur][largeur_pix/3].pixels[hauteur_pix][1 + (largeur_pix%3) * 3 + pixel];
      }
    }
    else if (largeur_pix == 5) {
      somme += MCU_YCbCr->blocs_Cb[hauteur][1].pixels[hauteur_pix][7];
      somme += MCU_YCbCr->blocs_Cb[hauteur][2].pixels[hauteur_pix][0];
      somme += MCU_YCbCr->blocs_Cb[hauteur][2].pixels[hauteur_pix][1];
    }
    else if (largeur_pix > 5) {
      for (uint8_t pixel = 0; pixel < 3; pixel++) {
        somme += MCU_YCbCr->blocs_Cb[hauteur][largeur_pix/3].pixels[hauteur_pix][2 + (largeur_pix%3) * 3 + pixel];
      }
    }
  }

  /* Cas ou le rapport vaut 2 ou 4 */
  else if (div > 1) {
    for (uint8_t pixel = 0; pixel < div; pixel++) {
      int16_t pixel_a_ajouter = MCU_YCbCr->blocs_Cb[hauteur][largeur_pix * div / 8 + (largeur * div)].pixels[hauteur_pix][div * (largeur_pix % (8/div)) + pixel];
      somme += pixel_a_ajouter;
      }
    }

  /* Cas ou le rapport vaut 1, on recopie simplement le pixel */
  else {
    somme = MCU_YCbCr->blocs_Cb[hauteur][largeur].pixels[hauteur_pix][largeur_pix];
  }
  somme /= div;
  return somme;
}


/* Fait la moyenne sur un certain nombre de pixels Cr alignés horizontalement */
int16_t moyenne_horizontale_Cr(struct MCU_YCbCr *MCU_YCbCr,
                                      uint8_t hauteur_pix,
                                      uint8_t largeur_pix,
                                      uint8_t hauteur,
                                      uint8_t largeur)
{
  /* Fonction identique à moyenne_horizontale_Cb en remplaçant Cb par Cr et h2 par h3 */
  uint8_t div = MCU_YCbCr->h1/MCU_YCbCr->h3;
  int16_t somme = 0;
  if (div == 3) {
    if (largeur_pix < 2) {
      for (uint8_t pixel = 0; pixel < 3; pixel++) {
        somme += MCU_YCbCr->blocs_Cr[hauteur][largeur_pix/3].pixels[hauteur_pix][largeur_pix * 3 + pixel];
      }
    }
    else if (largeur_pix == 2) {
      somme += MCU_YCbCr->blocs_Cr[hauteur][largeur_pix/3].pixels[hauteur_pix][6];
      somme += MCU_YCbCr->blocs_Cr[hauteur][largeur_pix/3].pixels[hauteur_pix][7];
      somme += MCU_YCbCr->blocs_Cr[hauteur][1].pixels[hauteur_pix][0];
    }
    else if (largeur_pix > 2 && largeur_pix < 5) {
      for (uint8_t pixel = 0; pixel < 3; pixel++) {
        somme += MCU_YCbCr->blocs_Cr[hauteur][largeur_pix/3].pixels[hauteur_pix][1 + (largeur_pix%3) * 3 + pixel];
      }
    }
    else if (largeur_pix == 5) {
      somme += MCU_YCbCr->blocs_Cr[hauteur][1].pixels[hauteur_pix][7];
      somme += MCU_YCbCr->blocs_Cr[hauteur][2].pixels[hauteur_pix][0];
      somme += MCU_YCbCr->blocs_Cr[hauteur][2].pixels[hauteur_pix][1];
    }
    else if (largeur_pix > 5) {
      for (uint8_t pixel = 0; pixel < 3; pixel++) {
        somme += MCU_YCbCr->blocs_Cr[hauteur][largeur_pix/3].pixels[hauteur_pix][2 + (largeur_pix%3) * 3 + pixel];
      }
    }
  }
  else if (div > 1) {
    for (uint8_t pixel = 0; pixel < div; pixel++) {
      int16_t pixel_a_ajouter = MCU_YCbCr->blocs_Cr[hauteur][largeur_pix * div / 8 + (largeur * div)].pixels[hauteur_pix][div * (largeur_pix % (8/div)) + pixel];
      somme += pixel_a_ajouter;
      }
    }
  else {
    somme = MCU_YCbCr->blocs_Cr[hauteur][largeur].pixels[hauteur_pix][largeur_pix];
  }
  somme /= div;
  return somme;
}


/* Fait la moyenne sur un certain nombre de pixels Cb alignés verticalement */
int16_t moyenne_verticale_Cb(struct MCU_YCbCr *MCU_YCbCr,
                                    uint8_t hauteur_pix,
                                    uint8_t largeur_pix,
                                    uint8_t hauteur,
                                    uint8_t largeur)
{
  /* Fonction identique à moyenne_horizontale_Cb, la différence venant du fait
  qu'on travail ici sur la hauteur des blocs et de pixels plutôt que sur la largeur */
  uint8_t div = MCU_YCbCr->v1/MCU_YCbCr->v2;
  int16_t somme = 0;
  if (div == 3) {
    if (hauteur_pix < 2) {
      for (uint8_t pixel = 0; pixel < 3; pixel++) {
        somme += MCU_YCbCr->blocs_Cb[hauteur_pix/3][largeur].pixels[hauteur_pix * 3 + pixel][largeur_pix];
      }
    }
    else if (hauteur_pix == 2) {
      somme += MCU_YCbCr->blocs_Cb[hauteur_pix/3][largeur].pixels[6][largeur_pix];
      somme += MCU_YCbCr->blocs_Cb[hauteur_pix/3][largeur].pixels[7][largeur_pix];
      somme += MCU_YCbCr->blocs_Cb[1][largeur].pixels[0][largeur_pix];
    }
    else if (hauteur_pix > 2 && hauteur_pix < 5) {
      for (uint8_t pixel = 0; pixel < 3; pixel++) {
        somme += MCU_YCbCr->blocs_Cb[hauteur_pix/3][largeur].pixels[1 + (hauteur_pix%3) * 3 + pixel][largeur_pix];
      }
    }
    else if (hauteur_pix == 5) {
      somme += MCU_YCbCr->blocs_Cb[1][largeur].pixels[7][largeur_pix];
      somme += MCU_YCbCr->blocs_Cb[2][largeur].pixels[0][largeur_pix];
      somme += MCU_YCbCr->blocs_Cb[2][largeur].pixels[1][largeur_pix];
    }
    else if (hauteur_pix > 5) {
      for (uint8_t pixel = 0; pixel < 3; pixel++) {
        somme += MCU_YCbCr->blocs_Cb[hauteur_pix/3][largeur].pixels[2 + (hauteur_pix%3) * 3 + pixel][largeur_pix];
      }
    }
  }
  else if (div > 1) {
    for (uint8_t pixel = 0; pixel < div; pixel++) {
      int16_t pixel_a_ajouter = MCU_YCbCr->blocs_Cb[hauteur_pix * div / 8 + (hauteur * div)][largeur].pixels[div * (hauteur_pix % (8/div)) + pixel][largeur_pix];
      somme += pixel_a_ajouter;
    }
  }
  else {
    somme = MCU_YCbCr->blocs_Cb[hauteur][largeur].pixels[hauteur_pix][largeur_pix];
  }
  somme /= div;
  return somme;
}


/* Fait la moyenne sur un certain nombre de pixels Cr alignés verticalement */
int16_t moyenne_verticale_Cr(struct MCU_YCbCr *MCU_YCbCr,
                                    uint8_t hauteur_pix,
                                    uint8_t largeur_pix,
                                    uint8_t hauteur,
                                    uint8_t largeur)
{
  /* Fonction identique à moyenne_verticale_Cb en remplaçant Cb par Cr et v2 par v3 */
  uint8_t div = MCU_YCbCr->v1/MCU_YCbCr->v3;
  int16_t somme = 0;
  if (div == 3) {
    if (hauteur_pix < 2) {
      for (uint8_t pixel = 0; pixel < 3; pixel++) {
        somme += MCU_YCbCr->blocs_Cr[hauteur_pix/3][largeur].pixels[hauteur_pix * 3 + pixel][largeur_pix];
      }
    }
    else if (hauteur_pix == 2) {
      somme += MCU_YCbCr->blocs_Cr[hauteur_pix/3][largeur].pixels[6][largeur_pix];
      somme += MCU_YCbCr->blocs_Cr[hauteur_pix/3][largeur].pixels[7][largeur_pix];
      somme += MCU_YCbCr->blocs_Cr[1][largeur].pixels[0][largeur_pix];
    }
    else if (hauteur_pix > 2 && hauteur_pix < 5) {
      for (uint8_t pixel = 0; pixel < 3; pixel++) {
        somme += MCU_YCbCr->blocs_Cr[hauteur_pix/3][largeur].pixels[1 + (hauteur_pix%3) * 3 + pixel][largeur_pix];
      }
    }
    else if (hauteur_pix == 5) {
      somme += MCU_YCbCr->blocs_Cr[1][largeur].pixels[7][largeur_pix];
      somme += MCU_YCbCr->blocs_Cr[2][largeur].pixels[0][largeur_pix];
      somme += MCU_YCbCr->blocs_Cr[2][largeur].pixels[1][largeur_pix];
    }
    else if (hauteur_pix > 5) {
      for (uint8_t pixel = 0; pixel < 3; pixel++) {
        somme += MCU_YCbCr->blocs_Cr[hauteur_pix/3][largeur].pixels[2 + (hauteur_pix%3) * 3 + pixel][largeur_pix];
      }
    }
  }
  else if (div > 1) {
    for (uint8_t pixel = 0; pixel < div; pixel++) {
      int16_t pixel_a_ajouter = MCU_YCbCr->blocs_Cr[hauteur_pix * div / 8 + (hauteur * div)][largeur].pixels[div * (hauteur_pix % (8/div)) + pixel][largeur_pix];
      somme += pixel_a_ajouter;
    }
  }
  else {
    somme = MCU_YCbCr->blocs_Cr[hauteur][largeur].pixels[hauteur_pix][largeur_pix];
  }
  somme /= div;
  return somme;
}


/* Sous-échantillonne horizontalement une MCU_YCbCr */
struct MCU_YCbCr *sous_echantillonnage_horizontal(struct MCU_YCbCr *MCU_YCbCr)
{

  /* Recopie les pixels Y */
  struct Bloc_YCbCr **blocs_Y = malloc(MCU_YCbCr->v1 * sizeof(struct Bloc_YCbCr*));
  for (uint8_t hauteur = 0; hauteur < MCU_YCbCr->v1; hauteur++) {
    struct Bloc_YCbCr *ligne_Y = malloc(MCU_YCbCr->h1 * sizeof(struct Bloc_YCbCr));
    for (uint8_t largeur = 0; largeur < MCU_YCbCr->h1; largeur++) {
      int16_t **pixels_Y = malloc(8 * sizeof(int16_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        int16_t *ligne_pixels_Y = malloc(8 * sizeof(int16_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          ligne_pixels_Y[largeur_pix] = MCU_YCbCr->blocs_Y[hauteur][largeur].pixels[hauteur_pix][largeur_pix];
        }
        pixels_Y[hauteur_pix] = ligne_pixels_Y;
      }
      ligne_Y[largeur].pixels = pixels_Y;
    }
    blocs_Y[hauteur] = ligne_Y;
  }

  /* Sous-échantillonne horizontalement les pixels Cb */
  struct Bloc_YCbCr **sous_blocs_Cb = malloc(MCU_YCbCr->v1 * sizeof(struct Bloc_YCbCr*));
  for (uint8_t hauteur = 0; hauteur < MCU_YCbCr->v1; hauteur++) {
    struct Bloc_YCbCr *sous_ligne_Cb = malloc(MCU_YCbCr->h2 * sizeof(struct Bloc_YCbCr));
    for (uint8_t largeur = 0; largeur < MCU_YCbCr->h2; largeur++) {
      int16_t **pixels_Cb = malloc(8 * sizeof(int16_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        int16_t *ligne_pixels_Cb = malloc(8 * sizeof(int16_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          /* Fait la moyenne de x pixels alignés horizontalement et
          affecte cette valeur au nouveau bloc_Cb sous échantillonné */
          ligne_pixels_Cb[largeur_pix] = moyenne_horizontale_Cb(MCU_YCbCr, hauteur_pix, largeur_pix, hauteur, largeur);
        }
        pixels_Cb[hauteur_pix] = ligne_pixels_Cb;
      }
      sous_ligne_Cb[largeur].pixels = pixels_Cb;
    }
    sous_blocs_Cb[hauteur] = sous_ligne_Cb;
  }

  /* Sous-échantillonne horizontalement les Cr */
  struct Bloc_YCbCr **sous_blocs_Cr = malloc(MCU_YCbCr->v1 * sizeof(struct Bloc_YCbCr*));
  for (uint8_t hauteur = 0; hauteur < MCU_YCbCr->v1; hauteur++) {
    struct Bloc_YCbCr *sous_ligne_Cr = malloc(MCU_YCbCr->h3 * sizeof(struct Bloc_YCbCr));
    for (uint8_t largeur = 0; largeur < MCU_YCbCr->h3; largeur++) {
      int16_t **pixels_Cr = malloc(8 * sizeof(int16_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        int16_t *ligne_pixels_Cr = malloc(8 * sizeof(int16_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          /* Fait la moyenne de x pixels alignés horizontalement et
          affecte cette valeur au nouveau bloc_Cr sous échantillonné */
          ligne_pixels_Cr[largeur_pix] = moyenne_horizontale_Cr(MCU_YCbCr, hauteur_pix, largeur_pix, hauteur, largeur);
        }
        pixels_Cr[hauteur_pix] = ligne_pixels_Cr;
      }
      sous_ligne_Cr[largeur].pixels = pixels_Cr;
    }
    sous_blocs_Cr[hauteur] = sous_ligne_Cr;
  }

  /* Crée la nouvelle MCU sous-échantillonnée à laquelle
  on affecte les matrices de blocs calculées précédemment */
  struct MCU_YCbCr *sous_MCU = malloc(sizeof(struct MCU_YCbCr));
  sous_MCU->v1 = MCU_YCbCr->v1;
  sous_MCU->h1 = MCU_YCbCr->h1;
  sous_MCU->blocs_Y = blocs_Y;
  sous_MCU->blocs_Cb = sous_blocs_Cb;
  sous_MCU->blocs_Cr = sous_blocs_Cr;
  sous_MCU->v2 = MCU_YCbCr->v2;
  sous_MCU->h2 = MCU_YCbCr->h2;
  sous_MCU->v3 = MCU_YCbCr->v3;
  sous_MCU->h3 = MCU_YCbCr->h3;

  /* Retourne le pointeur de notre nouvelle MCU_YCbCr */
  return sous_MCU;
}


/* Sous-échantillonne verticalement une MCU_YCbCr */
struct MCU_YCbCr *sous_echantillonnage_vertical(struct MCU_YCbCr *MCU_YCbCr)
{
  /* Recopie les Y*/
  struct Bloc_YCbCr **blocs_Y = malloc(MCU_YCbCr->v1 * sizeof(struct Bloc_YCbCr*));
  for (uint8_t hauteur = 0; hauteur < MCU_YCbCr->v1; hauteur++) {
    struct Bloc_YCbCr *ligne_Y = malloc(MCU_YCbCr->h1 * sizeof(struct Bloc_YCbCr));
    for (uint8_t largeur = 0; largeur < MCU_YCbCr->h1; largeur++) {
      int16_t **pixels_Y = malloc(8 * sizeof(int16_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        int16_t *ligne_pixels_Y = malloc(8 * sizeof(int16_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          ligne_pixels_Y[largeur_pix] = MCU_YCbCr->blocs_Y[hauteur][largeur].pixels[hauteur_pix][largeur_pix];
        }
        pixels_Y[hauteur_pix] = ligne_pixels_Y;
      }
      ligne_Y[largeur].pixels = pixels_Y;
    }
    blocs_Y[hauteur] = ligne_Y;
  }

  /* Sous-échantillonne  verticalement les Cb */
  struct Bloc_YCbCr **sous_blocs_Cb = malloc(MCU_YCbCr->v2 * sizeof(struct Bloc_YCbCr*));
  for (uint8_t hauteur = 0; hauteur < MCU_YCbCr->v2; hauteur++) {
    struct Bloc_YCbCr *sous_ligne_Cb = malloc(MCU_YCbCr->h2 * sizeof(struct Bloc_YCbCr));
    for (uint8_t largeur = 0; largeur < MCU_YCbCr->h2; largeur++) {
      int16_t **pixels_Cb = malloc(8 * sizeof(int16_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        int16_t *ligne_pixels_Cb = malloc(8 * sizeof(int16_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          /* Fait la moyenne de x pixels alignés verticalement et
          affecte cette valeur au nouveau bloc_Cb sous échantillonné */
          ligne_pixels_Cb[largeur_pix] = moyenne_verticale_Cb(MCU_YCbCr, hauteur_pix, largeur_pix, hauteur, largeur);
        }
        pixels_Cb[hauteur_pix] = ligne_pixels_Cb;
      }
      sous_ligne_Cb[largeur].pixels = pixels_Cb;
    }
    sous_blocs_Cb[hauteur] = sous_ligne_Cb;
  }

  /* Sous-échantillonne les Cr */
  struct Bloc_YCbCr **sous_blocs_Cr = malloc(MCU_YCbCr->v3 * sizeof(struct Bloc_YCbCr*));
  for (uint8_t hauteur = 0; hauteur < MCU_YCbCr->v3; hauteur++) {
    struct Bloc_YCbCr *sous_ligne_Cr = malloc(MCU_YCbCr->h3 * sizeof(struct Bloc_YCbCr));
    for (uint8_t largeur = 0; largeur < MCU_YCbCr->h3; largeur++) {
      int16_t **pixels_Cr = malloc(8 * sizeof(int16_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        int16_t *ligne_pixels_Cr = malloc(8 * sizeof(int16_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          /* Fait la moyenne de x pixels alignés verticalement et
          affecte cette valeur au nouveau bloc_Cr sous échantillonné */
          ligne_pixels_Cr[largeur_pix] = moyenne_verticale_Cr(MCU_YCbCr, hauteur_pix, largeur_pix, hauteur, largeur);
        }
        pixels_Cr[hauteur_pix] = ligne_pixels_Cr;
      }
      sous_ligne_Cr[largeur].pixels = pixels_Cr;
    }
    sous_blocs_Cr[hauteur] = sous_ligne_Cr;
  }

  /* Crée la nouvelle MCU sous-échantillonnée à laquelle
  on affecte les matrices de blocs calculées précédemment */
  struct MCU_YCbCr *sous_MCU = malloc(sizeof(struct MCU_YCbCr));
  sous_MCU->v1 = MCU_YCbCr->v1;
  sous_MCU->h1 = MCU_YCbCr->h1;
  sous_MCU->blocs_Y = blocs_Y;
  sous_MCU->blocs_Cb = sous_blocs_Cb;
  sous_MCU->blocs_Cr = sous_blocs_Cr;
  sous_MCU->v2 = MCU_YCbCr->v2;
  sous_MCU->h2 = MCU_YCbCr->h2;
  sous_MCU->v3 = MCU_YCbCr->v3;
  sous_MCU->h3 = MCU_YCbCr->h3;

  /* Retourne le pointeur de notre nouvelle MCU_YCbCr */
  return sous_MCU;
}


/* Sous-échantillonne une MCU en supposant que tous les
paramètres respectent les conditions sur les hi et vi */
struct MCU_YCbCr *sous_echantillonnage(struct MCU_YCbCr *MCU_YCbCr)
{
  struct MCU_YCbCr *horizontal;
  /* Initialise vertical au cas ou on appelle sous_echantillonnage sans sous-échantillonner*/
  struct MCU_YCbCr *vertical = MCU_YCbCr;

  /* Commence par sous-échantillonner horizontalement si nécessaire */
  if (MCU_YCbCr->h2 < MCU_YCbCr->h1 || MCU_YCbCr->h3 < MCU_YCbCr->h1) {
    horizontal = sous_echantillonnage_horizontal(MCU_YCbCr);
    /* Sous-échantillonne ensuite verticalement */
    if (MCU_YCbCr->v2 < MCU_YCbCr->v1 || MCU_YCbCr->v3 < MCU_YCbCr->v1) {
      vertical = sous_echantillonnage_vertical(horizontal);
      horizontal->v2 = horizontal->v1;
      horizontal->v3 = horizontal->v1;
      /* Libère la mémoire allouée à horizontal qui sera sinon perdue */
      free_MCU(horizontal);
      /* Retourne une MCU sous échantillonnée horizontalement et verticalement */
      return vertical;
    }
    /* Retourne une MCU sous échantillonnée horizontalement uniquement */
    return horizontal;
  }
  /* Sous-échantillonne uniquement de manière verticale */
  if (MCU_YCbCr->v2 < MCU_YCbCr->v1 || MCU_YCbCr->v3 < MCU_YCbCr->v1) {
    vertical = sous_echantillonnage_vertical(MCU_YCbCr);
    /* Retourne une MCU sous échantillonnée verticalement uniquement */
    return vertical;
  }
  /* Retourne une MCU non sous-échantillonnée */
  return vertical;
}


/* Sous-échantillonne une matrice de MCU_YCbCr, on suppose ici aussi
 que tous les paramètres vérifient les conditions sur les hi et vi */
struct MCU_YCbCr ***sous_echantillonne(struct MCU_YCbCr ***MCU_a_sous_ech,
                                            uint32_t nb_MCUs_largeur,
                                            uint32_t nb_MCUs_hauteur)
{
  struct MCU_YCbCr ***matrice_MCUs_sous_ech = malloc(nb_MCUs_hauteur * sizeof(struct MCU_YCbCr**));
  /* Parcourt la matrice en sous-échantillonnant chaque MCU_YCbCr une à une */
  for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur; hauteur++) {
      struct MCU_YCbCr **ligne_MCUs_sous_ech = malloc(nb_MCUs_largeur * sizeof(struct MCU_YCbCr*));
      for (uint32_t largeur = 0; largeur < nb_MCUs_largeur; largeur++) {
        ligne_MCUs_sous_ech[largeur] = sous_echantillonnage(MCU_a_sous_ech[hauteur][largeur]);
      }
      matrice_MCUs_sous_ech[hauteur] = ligne_MCUs_sous_ech;
  }
  /* Retourne le pointeur de la matrice sous-échantillonnée */
  return matrice_MCUs_sous_ech;
}


/* Affiche un bloc */
void print_bloc(struct Bloc_YCbCr bloc)
{
  for (uint8_t hauteur = 0; hauteur < 8; hauteur++) {
    for (uint8_t largeur = 0; largeur < 8; largeur++) {
      printf("%x \t", bloc.pixels[hauteur][largeur]);
    }
    printf("\n");
  }
}

/* Affiche une MCU_YCbCr */
void print_MCU_YCbCr_val(struct MCU_YCbCr *MCU_YCbCr)
{
  printf("Bloc_YCbCrs Y \n");
  for (uint8_t hauteur = 0; hauteur < MCU_YCbCr->v1; hauteur++) {
    for (uint8_t largeur = 0; largeur < MCU_YCbCr->h1; largeur++) {
      print_bloc(MCU_YCbCr->blocs_Y[hauteur][largeur]);
      printf("\n");
    }
  }
  printf("Bloc_YCbCrs Cb \n");
  for (uint8_t hauteur = 0; hauteur < MCU_YCbCr->v2; hauteur++) {
    for (uint8_t largeur = 0; largeur < MCU_YCbCr->h2; largeur++) {
      print_bloc(MCU_YCbCr->blocs_Cb[hauteur][largeur]);
      printf("\n");
    }
  }
  printf("Bloc_YCbCrs Cr \n");
  for (uint8_t hauteur = 0; hauteur < MCU_YCbCr->v3; hauteur++) {
    for (uint8_t largeur = 0; largeur < MCU_YCbCr->h3; largeur++) {
      print_bloc(MCU_YCbCr->blocs_Cr[hauteur][largeur]);
      printf("\n");
    }
  }
}


/* Affiche la matrice de MCU_YCbCr */
void print_matrice_MCU_YCbCr_val(struct MCU_YCbCr ***MCUs_YCbCr_a_afficher,
                             uint32_t nb_MCUs_largeur,
                             uint32_t nb_MCUs_hauteur)
{
    for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur; hauteur++) {
        for (uint32_t largeur = 0; largeur < nb_MCUs_largeur; largeur++) {
            printf("MCU n°%u\n\n", largeur + hauteur * nb_MCUs_largeur);
            print_MCU_YCbCr_val(MCUs_YCbCr_a_afficher[hauteur][largeur]);
            printf("\n");
        }
    }
}


// int main()
// {
//    FILE *fichier = fopen("images/invaderedx16.ppm", "r");
//
//    // On récupère l'en-tête (P5 ou P6)
//    char en_tete[10];
//    fgets(en_tete, 10, fichier);
//
//    // On récupère les dimensions de l'image
//    char dimensions[30];
//    uint32_t largeur_image, hauteur_image;
//    fgets(dimensions, 30, fichier);
//    sscanf(dimensions, "%u %u", &largeur_image, &hauteur_image);
//
//    // On saute une ligne
//    char couleurs_max[10];
//    fgets(couleurs_max, 10, fichier);
//
//    // On initialise les dimensions des MCUs
//    uint8_t h1 = 1;
//    uint8_t v1 = 3;
//    uint8_t h2 = 1;
//    uint8_t v2 = 1;
//    uint8_t h3 = 1;
//    uint8_t v3 = 1;
//
//    uint32_t *dimensions_MCUs = calcul_dimensions_MCUs_RGB(largeur_image, hauteur_image, h1, v1);
//    uint32_t nb_MCUs_hauteur, nb_MCUs_largeur;
//    nb_MCUs_largeur = dimensions_MCUs[0];
//    nb_MCUs_hauteur = dimensions_MCUs[1];
//
//    /* On découpe notre matrice en MCU puis en blocs */
//    struct MCU_RGB ***MCUs = decoupage_MCUs(fichier, largeur_image, hauteur_image, nb_MCUs_largeur, nb_MCUs_hauteur, h1, v1, h2, v2, h3, v3);
//    MCUs = decoupage_MCUs_en_blocs(MCUs, nb_MCUs_largeur, nb_MCUs_hauteur);
//
//    /* On converti la matrice en YCbCr */
//    struct MCU_YCbCr ***matrice_MCUs_converti = conversion_matrice_MCUs(MCUs, nb_MCUs_largeur, nb_MCUs_hauteur);
//
//    /* On sous-échantillonne la matrice */
//    struct MCU_YCbCr ***matrice_MCUs_sous_ech = sous_echantillonne(matrice_MCUs_converti, nb_MCUs_largeur, nb_MCUs_hauteur);
//
//    /* On affiche les différentes matrices à l'écran */
//    print_MCUs_RGB(MCUs, dimensions_MCUs);
//    print_matrice_MCU_YCbCr(matrice_MCUs_converti, nb_MCUs_largeur, nb_MCUs_hauteur);
//    print_matrice_MCU_YCbCr_val(matrice_MCUs_sous_ech, nb_MCUs_largeur, nb_MCUs_hauteur);
//
//
//    /* On libère la mémoire allouée aux différentes matrices */
//    free_MCUs_YCbCr_val(matrice_MCUs_sous_ech, dimensions_MCUs);
//    free_MCUs_YCbCr(matrice_MCUs_converti, dimensions_MCUs);
//    free_MCUs_dims_RGB(MCUs, dimensions_MCUs);
//
//    fclose(fichier);
//    return 0;
// }


/* main permettant de tester le sous-échantillonnage
quand la conversion RGB vers YCbCr n'était pas terminée */

// void main()
// {
//   struct MCU_YCbCr *MCU_YCbCr = malloc(sizeof(struct MCU_YCbCr));
//   MCU_YCbCr->v1 = 1, MCU_YCbCr->h1 = 2;
//
//   /* On crée à la main toutes les matrices nécessaires */
//
//   int16_t mat_Y_g [8][8] = {  {0xa6, 0xa0, 0x9a, 0x98, 0x9a, 0x9a, 0x96, 0x91},
//                               {0xa0, 0xa3, 0x9d, 0x8e, 0x88, 0x8f, 0x95, 0x94},
//                               {0xa5, 0x97, 0x96, 0xa1, 0x9f, 0x90, 0x90, 0x9e},
//                               {0xa6, 0x9a, 0x91, 0x91, 0x92, 0x90, 0x90, 0x93},
//                               {0xc9, 0xd9, 0xc8, 0x98, 0x85, 0x98, 0xa2, 0x95},
//                               {0xf0, 0xf5, 0xf9, 0xea, 0xbf, 0x98, 0x90, 0x9d},
//                               {0xe9, 0xe1, 0xf3, 0xfd, 0xf2, 0xaf, 0x8a, 0x90},
//                               {0xe6, 0xf2, 0xf1, 0xed, 0xf8, 0xfb, 0xd0, 0x95}};
//
//   int16_t mat_Y_d [8][8] = { {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
//                                 {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
//                                 {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa6, 0xa7},
//                                 {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
//                                 {0x9d, 0xa0, 0xa3, 0xa4, 0xa3, 0xa3, 0xa4, 0xa6},
//                                 {0x9b, 0x9e, 0xa1, 0xa2, 0xa1, 0xa1, 0xa3, 0xa4},
//                                 {0x99, 0x9c, 0x9f, 0xa0, 0x9f, 0x9f, 0xa1, 0xa2},
//                                 {0x98, 0x9b, 0x9e, 0x9f, 0x9e, 0x9e, 0xa0, 0xa1}};
//
//   int16_t mat_Cb_g [8][8] = { {0x75, 0x75, 0x75, 0x75, 0x76, 0x76, 0x77, 0x78},
//                                   {0x75, 0x75, 0x75, 0x75, 0x76, 0x76, 0x77, 0x77},
//                                   {0x75, 0x75, 0x76, 0x76, 0x76, 0x76, 0x77, 0x77},
//                                   {0x76, 0x76, 0x76, 0x76, 0x77, 0x77, 0x77, 0x77},
//                                   {0x76, 0x76, 0x77, 0x77, 0x77, 0x77, 0x78, 0x78},
//                                   {0x77, 0x78, 0x78, 0x77, 0x77, 0x77, 0x78, 0x78},
//                                   {0x77, 0x77, 0x77, 0x78, 0x78, 0x78, 0x78, 0x78},
//                                   {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78}};
//
//   int16_t mat_Cb_d [8][8] = { {0x78, 0x79, 0x7a, 0x7a, 0x7b, 0x7b, 0x7b, 0x7b},
//                                   {0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7b, 0x7b},
//                                   {0x78, 0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7a},
//                                   {0x78, 0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7a},
//                                   {0x78, 0x78, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79},
//                                   {0x78, 0x78, 0x78, 0x78, 0x79, 0x79, 0x79, 0x79},
//                                   {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78},
//                                   {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78}};
//
//   int16_t mat_Cr_g [8][8] = { {0x8d, 0x8c, 0x89, 0x87, 0x85, 0x85, 0x86, 0x89},
//                                   {0x8d, 0x8c, 0x89, 0x87, 0x85, 0x85, 0x86, 0x88},
//                                   {0x8c, 0x8b, 0x89, 0x87, 0x85, 0x85, 0x85, 0x88},
//                                   {0x8c, 0x8b, 0x88, 0x86, 0x85, 0x85, 0x85, 0x88},
//                                   {0x8c, 0x8b, 0x88, 0x86, 0x84, 0x84, 0x85, 0x87},
//                                   {0x8b, 0x87, 0x84, 0x86, 0x84, 0x84, 0x84, 0x87},
//                                   {0x8b, 0x8a, 0x87, 0x81, 0x84, 0x84, 0x84, 0x87},
//                                   {0x8b, 0x89, 0x87, 0x85, 0x84, 0x83, 0x84, 0x87}};
//
//   int16_t mat_Cr_d [8][8] = { {0x8b, 0x8e, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x88},
//                                   {0x8b, 0x8d, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x87},
//                                   {0x8a, 0x8d, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x87},
//                                   {0x8a, 0x8d, 0x8d, 0x8d, 0x8c, 0x8a, 0x87, 0x86},
//                                   {0x8a, 0x8c, 0x8d, 0x8d, 0x8b, 0x89, 0x86, 0x85},
//                                   {0x89, 0x8c, 0x8d, 0x8d, 0x8b, 0x89, 0x86, 0x86},
//                                   {0x89, 0x8c, 0x8c, 0x8c, 0x8b, 0x89, 0x86, 0x86},
//                                   {0x89, 0x8c, 0x8c, 0x8c, 0x8b, 0x89, 0x86, 0x86}};
//
//   int16_t mat_Y_g1 [8][8] = {  {0xa6, 0xa0, 0x9a, 0x98, 0x9a, 0x9a, 0x96, 0x91},
//                               {0xa0, 0xa3, 0x9d, 0x8e, 0x88, 0x8f, 0x95, 0x94},
//                               {0xa5, 0x97, 0x96, 0xa1, 0x9f, 0x90, 0x90, 0x9e},
//                               {0xa6, 0x9a, 0x91, 0x91, 0x92, 0x90, 0x90, 0x93},
//                               {0xc9, 0xd9, 0xc8, 0x98, 0x85, 0x98, 0xa2, 0x95},
//                               {0xf0, 0xf5, 0xf9, 0xea, 0xbf, 0x98, 0x90, 0x9d},
//                               {0xe9, 0xe1, 0xf3, 0xfd, 0xf2, 0xaf, 0x8a, 0x90},
//                               {0xe6, 0xf2, 0xf1, 0xed, 0xf8, 0xfb, 0xd0, 0x95}};
//
//   int16_t mat_Y_d1 [8][8] = { {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
//                                 {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
//                                 {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa6, 0xa7},
//                                 {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
//                                 {0x9d, 0xa0, 0xa3, 0xa4, 0xa3, 0xa3, 0xa4, 0xa6},
//                                 {0x9b, 0x9e, 0xa1, 0xa2, 0xa1, 0xa1, 0xa3, 0xa4},
//                                 {0x99, 0x9c, 0x9f, 0xa0, 0x9f, 0x9f, 0xa1, 0xa2},
//                                 {0x98, 0x9b, 0x9e, 0x9f, 0x9e, 0x9e, 0xa0, 0xa1}};
//
//   int16_t mat_Cb_g1 [8][8] = { {0x75, 0x75, 0x75, 0x75, 0x76, 0x76, 0x77, 0x78},
//                                   {0x75, 0x75, 0x75, 0x75, 0x76, 0x76, 0x77, 0x77},
//                                   {0x75, 0x75, 0x76, 0x76, 0x76, 0x76, 0x77, 0x77},
//                                   {0x76, 0x76, 0x76, 0x76, 0x77, 0x77, 0x77, 0x77},
//                                   {0x76, 0x76, 0x77, 0x77, 0x77, 0x77, 0x78, 0x78},
//                                   {0x77, 0x78, 0x78, 0x77, 0x77, 0x77, 0x78, 0x78},
//                                   {0x77, 0x77, 0x77, 0x78, 0x78, 0x78, 0x78, 0x78},
//                                   {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78}};
//
//   int16_t mat_Cb_d1 [8][8] = { {0x78, 0x79, 0x7a, 0x7a, 0x7b, 0x7b, 0x7b, 0x7b},
//                                   {0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7b, 0x7b},
//                                   {0x78, 0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7a},
//                                   {0x78, 0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7a},
//                                   {0x78, 0x78, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79},
//                                   {0x78, 0x78, 0x78, 0x78, 0x79, 0x79, 0x79, 0x79},
//                                   {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78},
//                                   {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78}};
//
//   int16_t mat_Cr_g1 [8][8] = { {0x8d, 0x8c, 0x89, 0x87, 0x85, 0x85, 0x86, 0x89},
//                                   {0x8d, 0x8c, 0x89, 0x87, 0x85, 0x85, 0x86, 0x88},
//                                   {0x8c, 0x8b, 0x89, 0x87, 0x85, 0x85, 0x85, 0x88},
//                                   {0x8c, 0x8b, 0x88, 0x86, 0x85, 0x85, 0x85, 0x88},
//                                   {0x8c, 0x8b, 0x88, 0x86, 0x84, 0x84, 0x85, 0x87},
//                                   {0x8b, 0x87, 0x84, 0x86, 0x84, 0x84, 0x84, 0x87},
//                                   {0x8b, 0x8a, 0x87, 0x81, 0x84, 0x84, 0x84, 0x87},
//                                   {0x8b, 0x89, 0x87, 0x85, 0x84, 0x83, 0x84, 0x87}};
//
//   int16_t mat_Cr_d1 [8][8] = { {0x8b, 0x8e, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x88},
//                                   {0x8b, 0x8d, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x87},
//                                   {0x8a, 0x8d, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x87},
//                                   {0x8a, 0x8d, 0x8d, 0x8d, 0x8c, 0x8a, 0x87, 0x86},
//                                   {0x8a, 0x8c, 0x8d, 0x8d, 0x8b, 0x89, 0x86, 0x85},
//                                   {0x89, 0x8c, 0x8d, 0x8d, 0x8b, 0x89, 0x86, 0x86},
//                                   {0x89, 0x8c, 0x8c, 0x8c, 0x8b, 0x89, 0x86, 0x86},
//                                   {0x89, 0x8c, 0x8c, 0x8c, 0x8b, 0x89, 0x86, 0x86}};
//
//   int16_t **pixels_Y_g = malloc(8 * sizeof(int16_t *));
//   int16_t **pixels_Y_d = malloc(8 * sizeof(int16_t *));
//   int16_t **pixels_Cb_g = malloc(8 * sizeof(int16_t *));
//   int16_t **pixels_Cb_d = malloc(8 * sizeof(int16_t *));
//   int16_t **pixels_Cr_g = malloc(8 * sizeof(int16_t *));
//   int16_t **pixels_Cr_d = malloc(8 * sizeof(int16_t *));
//
//   for (int16_t i = 0; i < 8; i++){
//       int16_t *ligne_pixels_Y_g = malloc(8 * sizeof(int16_t));
//       int16_t *ligne_pixels_Y_d = malloc(8 * sizeof(int16_t));
//       int16_t *ligne_pixels_Cb_g = malloc(8 * sizeof(int16_t));
//       int16_t *ligne_pixels_Cb_d = malloc(8 * sizeof(int16_t));
//       int16_t *ligne_pixels_Cr_g = malloc(8 * sizeof(int16_t));
//       int16_t *ligne_pixels_Cr_d = malloc(8 * sizeof(int16_t));
//       for (int16_t j = 0; j <8; j++){
//           ligne_pixels_Y_g[j] = mat_Y_g[i][j];
//           ligne_pixels_Y_d[j] = mat_Y_d[i][j];
//           ligne_pixels_Cb_g[j] = mat_Cb_g[i][j];
//           ligne_pixels_Cb_d[j] = mat_Cb_d[i][j];
//           ligne_pixels_Cr_g[j] = mat_Cr_g[i][j];
//           ligne_pixels_Cr_d[j] = mat_Cr_d[i][j];
//       }
//       pixels_Y_g[i] = ligne_pixels_Y_g;
//       pixels_Y_d[i] = ligne_pixels_Y_d;
//       pixels_Cb_g[i] = ligne_pixels_Cb_g;
//       pixels_Cb_d[i] = ligne_pixels_Cb_d;
//       pixels_Cr_g[i] = ligne_pixels_Cr_g;
//       pixels_Cr_d[i] = ligne_pixels_Cr_d;
//   }
//
//   struct Bloc_YCbCr matrice_Y_g;
//   struct Bloc_YCbCr matrice_Y_d;
//   struct Bloc_YCbCr matrice_Cb_g;
//   struct Bloc_YCbCr matrice_Cb_d;
//   struct Bloc_YCbCr matrice_Cr_g;
//   struct Bloc_YCbCr matrice_Cr_d;
//
//   matrice_Y_g.pixels = pixels_Y_g;
//   matrice_Y_d.pixels = pixels_Y_d;
//   matrice_Cb_g.pixels = pixels_Cb_g;
//   matrice_Cb_d.pixels = pixels_Cb_d;
//   matrice_Cr_g.pixels = pixels_Cr_g;
//   matrice_Cr_d.pixels = pixels_Cr_d;
//
//   int16_t **pixels_Y_g1 = malloc(8 * sizeof(int16_t *));
//   int16_t **pixels_Y_d1 = malloc(8 * sizeof(int16_t *));
//   int16_t **pixels_Cb_g1 = malloc(8 * sizeof(int16_t *));
//   int16_t **pixels_Cb_d1 = malloc(8 * sizeof(int16_t *));
//   int16_t **pixels_Cr_g1 = malloc(8 * sizeof(int16_t *));
//   int16_t **pixels_Cr_d1 = malloc(8 * sizeof(int16_t *));
//
//   for (int16_t i = 0; i < 8; i++){
//       int16_t *ligne_pixels_Y_g1 = malloc(8 * sizeof(int16_t));
//       int16_t *ligne_pixels_Y_d1 = malloc(8 * sizeof(int16_t));
//       int16_t *ligne_pixels_Cb_g1 = malloc(8 * sizeof(int16_t));
//       int16_t *ligne_pixels_Cb_d1 = malloc(8 * sizeof(int16_t));
//       int16_t *ligne_pixels_Cr_g1 = malloc(8 * sizeof(int16_t));
//       int16_t *ligne_pixels_Cr_d1 = malloc(8 * sizeof(int16_t));
//       for (int16_t j = 0; j <8; j++){
//           ligne_pixels_Y_g1[j] = mat_Y_g1[i][j];
//           ligne_pixels_Y_d1[j] = mat_Y_d1[i][j];
//           ligne_pixels_Cb_g1[j] = mat_Cb_g1[i][j];
//           ligne_pixels_Cb_d1[j] = mat_Cb_d1[i][j];
//           ligne_pixels_Cr_g1[j] = mat_Cr_g1[i][j];
//           ligne_pixels_Cr_d1[j] = mat_Cr_d1[i][j];
//       }
//       pixels_Y_g1[i] = ligne_pixels_Y_g1;
//       pixels_Y_d1[i] = ligne_pixels_Y_d1;
//       pixels_Cb_g1[i] = ligne_pixels_Cb_g1;
//       pixels_Cb_d1[i] = ligne_pixels_Cb_d1;
//       pixels_Cr_g1[i] = ligne_pixels_Cr_g1;
//       pixels_Cr_d1[i] = ligne_pixels_Cr_d1;
//   }
//
//   struct Bloc_YCbCr matrice_Y_g1;
//   struct Bloc_YCbCr matrice_Y_d1;
//   struct Bloc_YCbCr matrice_Cb_g1;
//   struct Bloc_YCbCr matrice_Cb_d1;
//   struct Bloc_YCbCr matrice_Cr_g1;
//   struct Bloc_YCbCr matrice_Cr_d1;
//
//   matrice_Y_g1.pixels = pixels_Y_g1;
//   matrice_Y_d1.pixels = pixels_Y_d1;
//   matrice_Cb_g1.pixels = pixels_Cb_g1;
//   matrice_Cb_d1.pixels = pixels_Cb_d1;
//   matrice_Cr_g1.pixels = pixels_Cr_g1;
//   matrice_Cr_d1.pixels = pixels_Cr_d1;
//
//   // struct Bloc_YCbCr **bloc_Y = malloc(sizeof(struct Bloc_YCbCr*));
//   // struct Bloc_YCbCr **bloc_Cb = malloc(sizeof(struct Bloc_YCbCr*));
//   // struct Bloc_YCbCr **bloc_Cr = malloc(sizeof(struct Bloc_YCbCr*));
//   //
//   // struct Bloc_YCbCr *ligne_bloc_Y = malloc(2 * sizeof(struct Bloc_YCbCr));
//   // struct Bloc_YCbCr *ligne_bloc_Cb = malloc(2 * sizeof(struct Bloc_YCbCr));
//   // struct Bloc_YCbCr *ligne_bloc_Cr = malloc(2 * sizeof(struct Bloc_YCbCr));
//   //
//   // ligne_bloc_Y[0] = matrice_Y_g;
//   // ligne_bloc_Y[1] = matrice_Y_d;
//   // ligne_bloc_Cb[0] = matrice_Cb_g;
//   // ligne_bloc_Cb[1] = matrice_Cb_d;
//   // ligne_bloc_Cr[0] = matrice_Cr_g;
//   // ligne_bloc_Cr[1] = matrice_Cr_d;
//   //
//   // bloc_Y[0] = ligne_bloc_Y;
//   // bloc_Cb[0] = ligne_bloc_Cb;
//   // bloc_Cr[0] = ligne_bloc_Cr;
//
//   struct Bloc_YCbCr **bloc_Y = malloc(2 * sizeof(struct Bloc_YCbCr*));
//   struct Bloc_YCbCr **bloc_Cb = malloc(2 * sizeof(struct Bloc_YCbCr*));
//   struct Bloc_YCbCr **bloc_Cr = malloc(2 * sizeof(struct Bloc_YCbCr*));
//
//   struct Bloc_YCbCr *ligne_bloc_Y_1 = malloc(2 * sizeof(struct Bloc_YCbCr));
//   struct Bloc_YCbCr *ligne_bloc_Y_2 = malloc(2 * sizeof(struct Bloc_YCbCr));
//   struct Bloc_YCbCr *ligne_bloc_Cb_1 = malloc(2 * sizeof(struct Bloc_YCbCr));
//   struct Bloc_YCbCr *ligne_bloc_Cb_2 = malloc(2 * sizeof(struct Bloc_YCbCr));
//   struct Bloc_YCbCr *ligne_bloc_Cr_1 = malloc(2 * sizeof(struct Bloc_YCbCr));
//   struct Bloc_YCbCr *ligne_bloc_Cr_2 = malloc(2 * sizeof(struct Bloc_YCbCr));
//
//   ligne_bloc_Y_1[0] = matrice_Y_g;
//   ligne_bloc_Y_2[0] = matrice_Y_g1;
//   ligne_bloc_Cb_1[0] = matrice_Cb_g;
//   ligne_bloc_Cb_2[0] = matrice_Cb_g1;
//   ligne_bloc_Cr_1[0] = matrice_Cr_g;
//   ligne_bloc_Cr_2[0] = matrice_Cr_g1;
//
//   ligne_bloc_Y_1[1] = matrice_Y_d;
//   ligne_bloc_Y_2[1] = matrice_Y_d1;
//   ligne_bloc_Cb_1[1] = matrice_Cb_d;
//   ligne_bloc_Cb_2[1] = matrice_Cb_d1;
//   ligne_bloc_Cr_1[1] = matrice_Cr_d;
//   ligne_bloc_Cr_2[1] = matrice_Cr_d1;
//
//   bloc_Y[0] = ligne_bloc_Y_1;
//   bloc_Y[1] = ligne_bloc_Y_2;
//   bloc_Cb[0] = ligne_bloc_Cb_1;
//   bloc_Cb[1] = ligne_bloc_Cb_2;
//   bloc_Cr[0] = ligne_bloc_Cr_1;
//   bloc_Cr[1] = ligne_bloc_Cr_2;
//
//   MCU_YCbCr->blocs_Y = bloc_Y;
//   MCU_YCbCr->blocs_Cb = bloc_Cb;
//   MCU_YCbCr->blocs_Cr = bloc_Cr;
//   MCU_YCbCr->v1 = 2;
//   MCU_YCbCr->h1 = 2;
//   MCU_YCbCr->v2 = 2;
//   MCU_YCbCr->h2 = 2;
//   MCU_YCbCr->v3 = 2;
//   MCU_YCbCr->h3 = 2;
//
//   // print_MCU_YCbCr(MCU_YCbCr);
//
//   /* On sous-échantillonne puis on affiche à l'écran pour vérifier */
//   struct MCU_YCbCr *test1 = sous_echantillonnage(MCU_YCbCr, 2, 2, 1, 2, 2, 1);
//   // struct MCU_YCbCr *test2 = sous_echantillonnage_horizontal(test1, 2, 2, 1, 1, 1, 1);
//
//
//   print_MCU_YCbCr(test1);
//
//   free_MCU(test1);
//   free_MCU(MCU_YCbCr);
//
// }
