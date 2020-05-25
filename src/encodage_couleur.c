#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// #include "../include/decoupe_tout.h"
// #include "../include/qtables.h"
//
// #define M_PI 3.14159265358979323846
//


/******************************/
/* Module d'encodage des MCUs */
/******************************/


/* Défini un bloc */
struct Bloc {
    uint16_t **pixels;      // un Bloc = une matrice de pixels
};


/* MCU après conversion en YCbCr */
struct MCU_Y{
    uint8_t largeur_MCU;
    uint8_t hauteur_MCU;
    struct Bloc **blocs_Y;
    uint8_t largeur_bloc_Cb;
    uint8_t hauteur_bloc_Cb;
    struct Bloc **blocs_Cb;
    uint8_t largeur_bloc_Cr;
    uint8_t hauteur_bloc_Cr;
    struct Bloc **blocs_Cr;
};

/**********************************/
/* Partie consacrée a l'affichage */
/**********************************/


void free_bloc(struct Bloc bloc)
{
  for (uint8_t hauteur = 0; hauteur < 8; hauteur ++) {
    free(bloc.pixels[hauteur]);
  }
  free(bloc.pixels);
}


void free_blocs(struct Bloc **blocs, uint8_t vi, uint8_t hi)
{
  for (uint8_t hauteur = 0; hauteur < vi; hauteur++) {
    for (uint8_t largeur = 0; largeur < hi; largeur++) {
      free_bloc(blocs[hauteur][largeur]);
    }
    free(blocs[hauteur]);
  }
  free(blocs);
}


void free_MCU(struct MCU_Y *MCU_Y)
{
  free_blocs(MCU_Y->blocs_Y, MCU_Y->hauteur_MCU, MCU_Y->largeur_MCU);
  free_blocs(MCU_Y->blocs_Cb, MCU_Y->hauteur_bloc_Cb, MCU_Y->largeur_bloc_Cb);
  free_blocs(MCU_Y->blocs_Cr, MCU_Y->hauteur_bloc_Cr, MCU_Y->largeur_bloc_Cr);
  free(MCU_Y);
}

/*******************************************/
/* Partie consacrée au sous-echantillonage */
/*******************************************/



uint16_t moyenne_horizontale_Cb(struct MCU_Y *MCU_Y,
                                    uint8_t largeur_MCU,
                                    uint8_t largeur_bloc_Cb,
                                    uint8_t hauteur_pix,
                                    uint8_t largeur_pix,
                                    uint8_t hauteur)
{
  uint8_t div = largeur_MCU/largeur_bloc_Cb;
  uint16_t somme = 0;
  for (uint8_t pixel = 0; pixel < div; pixel++) {
    uint16_t pixel_a_ajouter = MCU_Y->blocs_Cb[hauteur][largeur_pix * div / 8].pixels[hauteur_pix][div * (largeur_pix % 4) + pixel];
    somme += pixel_a_ajouter;
  }
  somme /= div;
  return somme;
}

uint16_t moyenne_horizontale_Cr(struct MCU_Y *MCU_Y,
                                    uint8_t largeur_MCU,
                                    uint8_t largeur_bloc_Cr,
                                    uint8_t hauteur_pix,
                                    uint8_t largeur_pix,
                                    uint8_t hauteur)
{
  uint8_t div = largeur_MCU/largeur_bloc_Cr;
  uint16_t somme = 0;
  for (uint8_t pixel = 0; pixel < div; pixel++) {
    uint16_t pixel_a_ajouter = MCU_Y->blocs_Cr[hauteur][largeur_pix * div / 8].pixels[hauteur_pix][div * (largeur_pix % 4) + pixel];
    somme += pixel_a_ajouter;
  }
  somme /= div;
  return somme;
}

uint16_t moyenne_verticale_Cb(struct MCU_Y *MCU_Y,
                                    uint8_t hauteur_MCU,
                                    uint8_t hauteur_bloc_Cb,
                                    uint8_t hauteur_pix,
                                    uint8_t largeur_pix,
                                    uint8_t largeur)
{
  uint8_t div = hauteur_MCU/hauteur_bloc_Cb;
  uint16_t somme = 0;
  for (uint8_t pixel = 0; pixel < div; pixel++) {
    uint16_t pixel_a_ajouter = MCU_Y->blocs_Cb[hauteur_pix * div / 8][largeur].pixels[div * (hauteur_pix % 4) + pixel][largeur_pix];
    somme += pixel_a_ajouter;
  }
  somme /= div;
  return somme;
}

uint16_t moyenne_verticale_Cr(struct MCU_Y *MCU_Y,
                                    uint8_t hauteur_MCU,
                                    uint8_t hauteur_bloc_Cr,
                                    uint8_t hauteur_pix,
                                    uint8_t largeur_pix,
                                    uint8_t largeur)
{
  uint8_t div = hauteur_MCU/hauteur_bloc_Cr;
  uint16_t somme = 0;
  for (uint8_t pixel = 0; pixel < div; pixel++) {
    uint16_t pixel_a_ajouter = MCU_Y->blocs_Cr[hauteur_pix * div / 8][largeur].pixels[div * (hauteur_pix % 4) + pixel][largeur_pix];
    somme += pixel_a_ajouter;
  }
  somme /= div;
  return somme;
}

struct MCU_Y *sous_echantillonage_horizontal(struct MCU_Y *MCU_Y,
                                  uint8_t h1,
                                  uint8_t v1,
                                  uint8_t h2,
                                  uint8_t v2,
                                  uint8_t h3,
                                  uint8_t v3)
{

  /* On recopie les Y*/
  struct Bloc **blocs_Y = malloc(v1 * sizeof(struct Bloc*));
  for (uint8_t hauteur = 0; hauteur < v1; hauteur++) {
    struct Bloc *ligne_Y = malloc(h1 * sizeof(struct Bloc));
    for (uint8_t largeur = 0; largeur < h1; largeur++) {

      /* On fait la moyenne des pixels */
      uint16_t **pixels_Y = malloc(8 * sizeof(uint16_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        uint16_t *ligne_pixels_Y = malloc(8 * sizeof(uint16_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          // uint16_t *pixel_cb = malloc(sizeof(uint16_t));
          // pixel_cb[0] = (MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix] + MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix + 1])/2;
          ligne_pixels_Y[largeur_pix] = MCU_Y->blocs_Y[hauteur][largeur].pixels[hauteur_pix][largeur_pix];
        }
        pixels_Y[hauteur_pix] = ligne_pixels_Y;
      }
      ligne_Y[largeur].pixels = pixels_Y;
    }
    blocs_Y[hauteur] = ligne_Y;
  }

  /* On sous-echantillone les Cb */
  struct Bloc **sous_blocs_Cb = malloc(v2 * sizeof(struct Bloc*));
  for (uint8_t hauteur = 0; hauteur < v2; hauteur++) {
    struct Bloc *sous_ligne_Cb = malloc(h2 * sizeof(struct Bloc));
    for (uint8_t largeur = 0; largeur < h2; largeur++) {

      /* On fait la moyenne des pixels */
      uint16_t **pixels_Cb = malloc(8 * sizeof(uint16_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        uint16_t *ligne_pixels_Cb = malloc(8 * sizeof(uint16_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          // uint16_t *pixel_cb = malloc(sizeof(uint16_t));
          // pixel_cb[0] = (MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix] + MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix + 1])/2;
          // ligne_pixels_Cb[largeur_pix] = (MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * (largeur_pix%4)] + MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * (largeur_pix%4) + 1])/2;
          ligne_pixels_Cb[largeur_pix] = moyenne_horizontale_Cb(MCU_Y, h1, h2, hauteur_pix, largeur_pix, hauteur);
        }
        pixels_Cb[hauteur_pix] = ligne_pixels_Cb;
      }
      sous_ligne_Cb[largeur].pixels = pixels_Cb;
    }
    sous_blocs_Cb[hauteur] = sous_ligne_Cb;
  }


  /* On sous-echantillone les Cr */

  struct Bloc **sous_blocs_Cr = malloc(v3 * sizeof(struct Bloc*));
  for (uint8_t hauteur = 0; hauteur < v3; hauteur++) {
    struct Bloc *sous_ligne_Cr = malloc(h3 * sizeof(struct Bloc));
    for (uint8_t largeur = 0; largeur < h3; largeur++) {


      /* On fait la moyenne des pixels */
      uint16_t **pixels_Cr = malloc(8 * sizeof(uint16_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        uint16_t *ligne_pixels_Cr = malloc(8 * sizeof(uint16_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          // uint16_t *pixel_cb = malloc(sizeof(uint16_t));
          // pixel_cb[0] = (MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix] + MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix + 1])/2;
          // ligne_pixels_Cr[largeur_pix] = (MCU_Y->blocs_Cr[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * (largeur_pix%4)] + MCU_Y->blocs_Cr[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * (largeur_pix%4) + 1])/2;
          ligne_pixels_Cr[largeur_pix] = moyenne_horizontale_Cr(MCU_Y, h1, h3, hauteur_pix, largeur_pix, hauteur);
        }
        pixels_Cr[hauteur_pix] = ligne_pixels_Cr;
      }
      sous_ligne_Cr[largeur].pixels = pixels_Cr;
    }
    sous_blocs_Cr[hauteur] = sous_ligne_Cr;
  }

  struct MCU_Y *sous_MCU = malloc(sizeof(struct MCU_Y));
  sous_MCU->hauteur_MCU = v1;
  sous_MCU->largeur_MCU = h1;
  sous_MCU->blocs_Y = blocs_Y;
  sous_MCU->blocs_Cb = sous_blocs_Cb;
  sous_MCU->blocs_Cr = sous_blocs_Cr;
  sous_MCU->hauteur_bloc_Cb = v2;
  sous_MCU->largeur_bloc_Cb = h2;
  sous_MCU->hauteur_bloc_Cr = v3;
  sous_MCU->largeur_bloc_Cr = h3;

  return sous_MCU;
}


struct MCU_Y *sous_echantillonage_vertical(struct MCU_Y *MCU_Y,
                                  uint8_t h1,
                                  uint8_t v1,
                                  uint8_t h2,
                                  uint8_t v2,
                                  uint8_t h3,
                                  uint8_t v3)
{
  /* On recopie les Y*/
  struct Bloc **blocs_Y = malloc(v1 * sizeof(struct Bloc*));
  for (uint8_t hauteur = 0; hauteur < v1; hauteur++) {
    struct Bloc *ligne_Y = malloc(h1 * sizeof(struct Bloc));
    for (uint8_t largeur = 0; largeur < h1; largeur++) {

      /* On fait la moyenne des pixels */
      uint16_t **pixels_Y = malloc(8 * sizeof(uint16_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        uint16_t *ligne_pixels_Y = malloc(8 * sizeof(uint16_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          // uint16_t *pixel_cb = malloc(sizeof(uint16_t));
          // pixel_cb[0] = (MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix] + MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix + 1])/2;
          ligne_pixels_Y[largeur_pix] = MCU_Y->blocs_Y[hauteur][largeur].pixels[hauteur_pix][largeur_pix];
        }
        pixels_Y[hauteur_pix] = ligne_pixels_Y;
      }
      ligne_Y[largeur].pixels = pixels_Y;
    }
    blocs_Y[hauteur] = ligne_Y;
  }

  /* On sous-echantillone les Cb */
  struct Bloc **sous_blocs_Cb = malloc(v2 * sizeof(struct Bloc*));
  for (uint8_t hauteur = 0; hauteur < v2; hauteur++) {
    struct Bloc *sous_ligne_Cb = malloc(h2 * sizeof(struct Bloc));
    for (uint8_t largeur = 0; largeur < h2; largeur++) {



      /* On fait la moyenne des pixels */
      uint16_t **pixels_Cb = malloc(8 * sizeof(uint16_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        uint16_t *ligne_pixels_Cb = malloc(8 * sizeof(uint16_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          // uint16_t *pixel_cb = malloc(sizeof(uint16_t));
          // pixel_cb[0] = (MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix] + MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix + 1])/2;
          // ligne_pixels_Cb[largeur_pix] = (MCU_Y->blocs_Cb[hauteur_pix/4][largeur].pixels[2 * (hauteur_pix%4)][largeur_pix] + MCU_Y->blocs_Cb[hauteur_pix/4][largeur].pixels[2 * (hauteur_pix%4) + 1][largeur_pix])/2;
          ligne_pixels_Cb[largeur_pix] = moyenne_verticale_Cb(MCU_Y, v1, v2, hauteur_pix, largeur_pix, largeur);
        }
        pixels_Cb[hauteur_pix] = ligne_pixels_Cb;
      }
      sous_ligne_Cb[largeur].pixels = pixels_Cb;
    }
    sous_blocs_Cb[hauteur] = sous_ligne_Cb;
  }


  /* On sous-echantillone les Cr */

  struct Bloc **sous_blocs_Cr = malloc(v3 * sizeof(struct Bloc*));
  for (uint8_t hauteur = 0; hauteur < v3; hauteur++) {
    struct Bloc *sous_ligne_Cr = malloc(h3 * sizeof(struct Bloc));
    for (uint8_t largeur = 0; largeur < h3; largeur++) {



      /* On fait la moyenne des pixels */
      uint16_t **pixels_Cr = malloc(8 * sizeof(uint16_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        uint16_t *ligne_pixels_Cr = malloc(8 * sizeof(uint16_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          // uint16_t *pixel_cb = malloc(sizeof(uint16_t));
          // pixel_cb[0] = (MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix] + MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix + 1])/2;
          // ligne_pixels_Cr[largeur_pix] = (MCU_Y->blocs_Cr[hauteur_pix/4][largeur].pixels[2 * (hauteur_pix%4)][largeur_pix] + MCU_Y->blocs_Cr[hauteur_pix/4][largeur].pixels[2 * (hauteur_pix%4) + 1][largeur_pix])/2;
          ligne_pixels_Cr[largeur_pix] = moyenne_verticale_Cr(MCU_Y, v1, v3, hauteur_pix, largeur_pix, largeur);
        }
        pixels_Cr[hauteur_pix] = ligne_pixels_Cr;
      }
      sous_ligne_Cr[largeur].pixels = pixels_Cr;
    }
    sous_blocs_Cr[hauteur] = sous_ligne_Cr;
  }

  struct MCU_Y *sous_MCU = malloc(sizeof(struct MCU_Y));
  sous_MCU->hauteur_MCU = v1;
  sous_MCU->largeur_MCU = h1;
  sous_MCU->blocs_Y = blocs_Y;
  sous_MCU->blocs_Cb = sous_blocs_Cb;
  sous_MCU->blocs_Cr = sous_blocs_Cr;
  sous_MCU->hauteur_bloc_Cb = v2;
  sous_MCU->largeur_bloc_Cb = h2;
  sous_MCU->hauteur_bloc_Cr = v3;
  sous_MCU->largeur_bloc_Cr = h3;


  return sous_MCU;
}


struct MCU_Y *sous_echantillonage(struct MCU_Y *MCU_Y,
                                  uint8_t h1,
                                  uint8_t v1,
                                  uint8_t h2,
                                  uint8_t v2,
                                  uint8_t h3,
                                  uint8_t v3)
{
  struct MCU_Y *horizontal;
  struct MCU_Y *vertical;
  if (h2 < h1) {
    horizontal = sous_echantillonage_horizontal(MCU_Y, h1, v1, h2, v1, h3, v1);
    if (v2 < v1) {
      vertical = sous_echantillonage_vertical(horizontal, h1, v1, horizontal->largeur_bloc_Cb, v2, horizontal->largeur_bloc_Cr, v3);
      free_MCU(horizontal);
      return vertical;
    }
    return horizontal;
  }
  if (v2 < v1) {
    vertical = sous_echantillonage_vertical(MCU_Y, h1, v1, h2, v2, h3, v3);
    return vertical;
  }
}



void print_bloc(struct Bloc bloc)
{
  for (uint8_t hauteur = 0; hauteur < 8; hauteur++) {
    for (uint8_t largeur = 0; largeur < 8; largeur++) {
      printf("%x \t", bloc.pixels[hauteur][largeur]);
    }
    printf("\n");
  }
}


void print_MCU_Y(struct MCU_Y *MCU_Y)
{
  printf("Blocs Y \n");
  for (uint8_t hauteur = 0; hauteur < MCU_Y->hauteur_MCU; hauteur++) {
    for (uint8_t largeur = 0; largeur < MCU_Y->largeur_MCU; largeur++) {
      print_bloc(MCU_Y->blocs_Y[hauteur][largeur]);
      printf("\n");
    }
  }
  printf("Blocs Cb \n");
  for (uint8_t hauteur = 0; hauteur < MCU_Y->hauteur_bloc_Cb; hauteur++) {
    for (uint8_t largeur = 0; largeur < MCU_Y->largeur_bloc_Cb; largeur++) {
      print_bloc(MCU_Y->blocs_Cb[hauteur][largeur]);
      printf("\n");
    }
  }
  printf("Blocs Cr \n");
  for (uint8_t hauteur = 0; hauteur < MCU_Y->hauteur_bloc_Cr; hauteur++) {
    for (uint8_t largeur = 0; largeur < MCU_Y->largeur_bloc_Cr; largeur++) {
      print_bloc(MCU_Y->blocs_Cr[hauteur][largeur]);
      printf("\n");
    }
  }
}


void main()
{
  struct MCU_Y *MCU_Y = malloc(sizeof(struct MCU_Y));
  MCU_Y->hauteur_MCU = 1, MCU_Y->largeur_MCU = 2;

  uint16_t mat_Y_g [8][8] = {  {0xa6, 0xa0, 0x9a, 0x98, 0x9a, 0x9a, 0x96, 0x91},
                              {0xa0, 0xa3, 0x9d, 0x8e, 0x88, 0x8f, 0x95, 0x94},
                              {0xa5, 0x97, 0x96, 0xa1, 0x9f, 0x90, 0x90, 0x9e},
                              {0xa6, 0x9a, 0x91, 0x91, 0x92, 0x90, 0x90, 0x93},
                              {0xc9, 0xd9, 0xc8, 0x98, 0x85, 0x98, 0xa2, 0x95},
                              {0xf0, 0xf5, 0xf9, 0xea, 0xbf, 0x98, 0x90, 0x9d},
                              {0xe9, 0xe1, 0xf3, 0xfd, 0xf2, 0xaf, 0x8a, 0x90},
                              {0xe6, 0xf2, 0xf1, 0xed, 0xf8, 0xfb, 0xd0, 0x95}};

  uint16_t mat_Y_d [8][8] = { {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
                                {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
                                {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa6, 0xa7},
                                {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
                                {0x9d, 0xa0, 0xa3, 0xa4, 0xa3, 0xa3, 0xa4, 0xa6},
                                {0x9b, 0x9e, 0xa1, 0xa2, 0xa1, 0xa1, 0xa3, 0xa4},
                                {0x99, 0x9c, 0x9f, 0xa0, 0x9f, 0x9f, 0xa1, 0xa2},
                                {0x98, 0x9b, 0x9e, 0x9f, 0x9e, 0x9e, 0xa0, 0xa1}};

  uint16_t mat_Cb_g [8][8] = { {0x75, 0x75, 0x75, 0x75, 0x76, 0x76, 0x77, 0x78},
                                  {0x75, 0x75, 0x75, 0x75, 0x76, 0x76, 0x77, 0x77},
                                  {0x75, 0x75, 0x76, 0x76, 0x76, 0x76, 0x77, 0x77},
                                  {0x76, 0x76, 0x76, 0x76, 0x77, 0x77, 0x77, 0x77},
                                  {0x76, 0x76, 0x77, 0x77, 0x77, 0x77, 0x78, 0x78},
                                  {0x77, 0x78, 0x78, 0x77, 0x77, 0x77, 0x78, 0x78},
                                  {0x77, 0x77, 0x77, 0x78, 0x78, 0x78, 0x78, 0x78},
                                  {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78}};

  uint16_t mat_Cb_d [8][8] = { {0x78, 0x79, 0x7a, 0x7a, 0x7b, 0x7b, 0x7b, 0x7b},
                                  {0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7b, 0x7b},
                                  {0x78, 0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7a},
                                  {0x78, 0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7a},
                                  {0x78, 0x78, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79},
                                  {0x78, 0x78, 0x78, 0x78, 0x79, 0x79, 0x79, 0x79},
                                  {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78},
                                  {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78}};

  uint16_t mat_Cr_g [8][8] = { {0x8d, 0x8c, 0x89, 0x87, 0x85, 0x85, 0x86, 0x89},
                                  {0x8d, 0x8c, 0x89, 0x87, 0x85, 0x85, 0x86, 0x88},
                                  {0x8c, 0x8b, 0x89, 0x87, 0x85, 0x85, 0x85, 0x88},
                                  {0x8c, 0x8b, 0x88, 0x86, 0x85, 0x85, 0x85, 0x88},
                                  {0x8c, 0x8b, 0x88, 0x86, 0x84, 0x84, 0x85, 0x87},
                                  {0x8b, 0x87, 0x84, 0x86, 0x84, 0x84, 0x84, 0x87},
                                  {0x8b, 0x8a, 0x87, 0x81, 0x84, 0x84, 0x84, 0x87},
                                  {0x8b, 0x89, 0x87, 0x85, 0x84, 0x83, 0x84, 0x87}};

  uint16_t mat_Cr_d [8][8] = { {0x8b, 0x8e, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x88},
                                  {0x8b, 0x8d, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x87},
                                  {0x8a, 0x8d, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x87},
                                  {0x8a, 0x8d, 0x8d, 0x8d, 0x8c, 0x8a, 0x87, 0x86},
                                  {0x8a, 0x8c, 0x8d, 0x8d, 0x8b, 0x89, 0x86, 0x85},
                                  {0x89, 0x8c, 0x8d, 0x8d, 0x8b, 0x89, 0x86, 0x86},
                                  {0x89, 0x8c, 0x8c, 0x8c, 0x8b, 0x89, 0x86, 0x86},
                                  {0x89, 0x8c, 0x8c, 0x8c, 0x8b, 0x89, 0x86, 0x86}};

  uint16_t mat_Y_g1 [8][8] = {  {0xa6, 0xa0, 0x9a, 0x98, 0x9a, 0x9a, 0x96, 0x91},
                              {0xa0, 0xa3, 0x9d, 0x8e, 0x88, 0x8f, 0x95, 0x94},
                              {0xa5, 0x97, 0x96, 0xa1, 0x9f, 0x90, 0x90, 0x9e},
                              {0xa6, 0x9a, 0x91, 0x91, 0x92, 0x90, 0x90, 0x93},
                              {0xc9, 0xd9, 0xc8, 0x98, 0x85, 0x98, 0xa2, 0x95},
                              {0xf0, 0xf5, 0xf9, 0xea, 0xbf, 0x98, 0x90, 0x9d},
                              {0xe9, 0xe1, 0xf3, 0xfd, 0xf2, 0xaf, 0x8a, 0x90},
                              {0xe6, 0xf2, 0xf1, 0xed, 0xf8, 0xfb, 0xd0, 0x95}};

  uint16_t mat_Y_d1 [8][8] = { {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
                                {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
                                {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa6, 0xa7},
                                {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
                                {0x9d, 0xa0, 0xa3, 0xa4, 0xa3, 0xa3, 0xa4, 0xa6},
                                {0x9b, 0x9e, 0xa1, 0xa2, 0xa1, 0xa1, 0xa3, 0xa4},
                                {0x99, 0x9c, 0x9f, 0xa0, 0x9f, 0x9f, 0xa1, 0xa2},
                                {0x98, 0x9b, 0x9e, 0x9f, 0x9e, 0x9e, 0xa0, 0xa1}};

  uint16_t mat_Cb_g1 [8][8] = { {0x75, 0x75, 0x75, 0x75, 0x76, 0x76, 0x77, 0x78},
                                  {0x75, 0x75, 0x75, 0x75, 0x76, 0x76, 0x77, 0x77},
                                  {0x75, 0x75, 0x76, 0x76, 0x76, 0x76, 0x77, 0x77},
                                  {0x76, 0x76, 0x76, 0x76, 0x77, 0x77, 0x77, 0x77},
                                  {0x76, 0x76, 0x77, 0x77, 0x77, 0x77, 0x78, 0x78},
                                  {0x77, 0x78, 0x78, 0x77, 0x77, 0x77, 0x78, 0x78},
                                  {0x77, 0x77, 0x77, 0x78, 0x78, 0x78, 0x78, 0x78},
                                  {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78}};

  uint16_t mat_Cb_d1 [8][8] = { {0x78, 0x79, 0x7a, 0x7a, 0x7b, 0x7b, 0x7b, 0x7b},
                                  {0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7b, 0x7b},
                                  {0x78, 0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7a},
                                  {0x78, 0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7a},
                                  {0x78, 0x78, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79},
                                  {0x78, 0x78, 0x78, 0x78, 0x79, 0x79, 0x79, 0x79},
                                  {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78},
                                  {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78}};

  uint16_t mat_Cr_g1 [8][8] = { {0x8d, 0x8c, 0x89, 0x87, 0x85, 0x85, 0x86, 0x89},
                                  {0x8d, 0x8c, 0x89, 0x87, 0x85, 0x85, 0x86, 0x88},
                                  {0x8c, 0x8b, 0x89, 0x87, 0x85, 0x85, 0x85, 0x88},
                                  {0x8c, 0x8b, 0x88, 0x86, 0x85, 0x85, 0x85, 0x88},
                                  {0x8c, 0x8b, 0x88, 0x86, 0x84, 0x84, 0x85, 0x87},
                                  {0x8b, 0x87, 0x84, 0x86, 0x84, 0x84, 0x84, 0x87},
                                  {0x8b, 0x8a, 0x87, 0x81, 0x84, 0x84, 0x84, 0x87},
                                  {0x8b, 0x89, 0x87, 0x85, 0x84, 0x83, 0x84, 0x87}};

  uint16_t mat_Cr_d1 [8][8] = { {0x8b, 0x8e, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x88},
                                  {0x8b, 0x8d, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x87},
                                  {0x8a, 0x8d, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x87},
                                  {0x8a, 0x8d, 0x8d, 0x8d, 0x8c, 0x8a, 0x87, 0x86},
                                  {0x8a, 0x8c, 0x8d, 0x8d, 0x8b, 0x89, 0x86, 0x85},
                                  {0x89, 0x8c, 0x8d, 0x8d, 0x8b, 0x89, 0x86, 0x86},
                                  {0x89, 0x8c, 0x8c, 0x8c, 0x8b, 0x89, 0x86, 0x86},
                                  {0x89, 0x8c, 0x8c, 0x8c, 0x8b, 0x89, 0x86, 0x86}};

  uint16_t **pixels_Y_g = malloc(8 * sizeof(uint16_t *));
  uint16_t **pixels_Y_d = malloc(8 * sizeof(uint16_t *));
  uint16_t **pixels_Cb_g = malloc(8 * sizeof(uint16_t *));
  uint16_t **pixels_Cb_d = malloc(8 * sizeof(uint16_t *));
  uint16_t **pixels_Cr_g = malloc(8 * sizeof(uint16_t *));
  uint16_t **pixels_Cr_d = malloc(8 * sizeof(uint16_t *));

  for (uint16_t i = 0; i < 8; i++){
      uint16_t *ligne_pixels_Y_g = malloc(8 * sizeof(uint16_t));
      uint16_t *ligne_pixels_Y_d = malloc(8 * sizeof(uint16_t));
      uint16_t *ligne_pixels_Cb_g = malloc(8 * sizeof(uint16_t));
      uint16_t *ligne_pixels_Cb_d = malloc(8 * sizeof(uint16_t));
      uint16_t *ligne_pixels_Cr_g = malloc(8 * sizeof(uint16_t));
      uint16_t *ligne_pixels_Cr_d = malloc(8 * sizeof(uint16_t));
      for (uint16_t j = 0; j <8; j++){
          ligne_pixels_Y_g[j] = mat_Y_g[i][j];
          ligne_pixels_Y_d[j] = mat_Y_d[i][j];
          ligne_pixels_Cb_g[j] = mat_Cb_g[i][j];
          ligne_pixels_Cb_d[j] = mat_Cb_d[i][j];
          ligne_pixels_Cr_g[j] = mat_Cr_g[i][j];
          ligne_pixels_Cr_d[j] = mat_Cr_d[i][j];
      }
      pixels_Y_g[i] = ligne_pixels_Y_g;
      pixels_Y_d[i] = ligne_pixels_Y_d;
      pixels_Cb_g[i] = ligne_pixels_Cb_g;
      pixels_Cb_d[i] = ligne_pixels_Cb_d;
      pixels_Cr_g[i] = ligne_pixels_Cr_g;
      pixels_Cr_d[i] = ligne_pixels_Cr_d;
  }

  struct Bloc matrice_Y_g;
  struct Bloc matrice_Y_d;
  struct Bloc matrice_Cb_g;
  struct Bloc matrice_Cb_d;
  struct Bloc matrice_Cr_g;
  struct Bloc matrice_Cr_d;

  matrice_Y_g.pixels = pixels_Y_g;
  matrice_Y_d.pixels = pixels_Y_d;
  matrice_Cb_g.pixels = pixels_Cb_g;
  matrice_Cb_d.pixels = pixels_Cb_d;
  matrice_Cr_g.pixels = pixels_Cr_g;
  matrice_Cr_d.pixels = pixels_Cr_d;

  uint16_t **pixels_Y_g1 = malloc(8 * sizeof(uint16_t *));
  uint16_t **pixels_Y_d1 = malloc(8 * sizeof(uint16_t *));
  uint16_t **pixels_Cb_g1 = malloc(8 * sizeof(uint16_t *));
  uint16_t **pixels_Cb_d1 = malloc(8 * sizeof(uint16_t *));
  uint16_t **pixels_Cr_g1 = malloc(8 * sizeof(uint16_t *));
  uint16_t **pixels_Cr_d1 = malloc(8 * sizeof(uint16_t *));

  for (uint16_t i = 0; i < 8; i++){
      uint16_t *ligne_pixels_Y_g1 = malloc(8 * sizeof(uint16_t));
      uint16_t *ligne_pixels_Y_d1 = malloc(8 * sizeof(uint16_t));
      uint16_t *ligne_pixels_Cb_g1 = malloc(8 * sizeof(uint16_t));
      uint16_t *ligne_pixels_Cb_d1 = malloc(8 * sizeof(uint16_t));
      uint16_t *ligne_pixels_Cr_g1 = malloc(8 * sizeof(uint16_t));
      uint16_t *ligne_pixels_Cr_d1 = malloc(8 * sizeof(uint16_t));
      for (uint16_t j = 0; j <8; j++){
          ligne_pixels_Y_g1[j] = mat_Y_g1[i][j];
          ligne_pixels_Y_d1[j] = mat_Y_d1[i][j];
          ligne_pixels_Cb_g1[j] = mat_Cb_g1[i][j];
          ligne_pixels_Cb_d1[j] = mat_Cb_d1[i][j];
          ligne_pixels_Cr_g1[j] = mat_Cr_g1[i][j];
          ligne_pixels_Cr_d1[j] = mat_Cr_d1[i][j];
      }
      pixels_Y_g1[i] = ligne_pixels_Y_g1;
      pixels_Y_d1[i] = ligne_pixels_Y_d1;
      pixels_Cb_g1[i] = ligne_pixels_Cb_g1;
      pixels_Cb_d1[i] = ligne_pixels_Cb_d1;
      pixels_Cr_g1[i] = ligne_pixels_Cr_g1;
      pixels_Cr_d1[i] = ligne_pixels_Cr_d1;
  }

  struct Bloc matrice_Y_g1;
  struct Bloc matrice_Y_d1;
  struct Bloc matrice_Cb_g1;
  struct Bloc matrice_Cb_d1;
  struct Bloc matrice_Cr_g1;
  struct Bloc matrice_Cr_d1;

  matrice_Y_g1.pixels = pixels_Y_g1;
  matrice_Y_d1.pixels = pixels_Y_d1;
  matrice_Cb_g1.pixels = pixels_Cb_g1;
  matrice_Cb_d1.pixels = pixels_Cb_d1;
  matrice_Cr_g1.pixels = pixels_Cr_g1;
  matrice_Cr_d1.pixels = pixels_Cr_d1;

  // struct Bloc **bloc_Y = malloc(sizeof(struct Bloc*));
  // struct Bloc **bloc_Cb = malloc(sizeof(struct Bloc*));
  // struct Bloc **bloc_Cr = malloc(sizeof(struct Bloc*));
  //
  // struct Bloc *ligne_bloc_Y = malloc(2 * sizeof(struct Bloc));
  // struct Bloc *ligne_bloc_Cb = malloc(2 * sizeof(struct Bloc));
  // struct Bloc *ligne_bloc_Cr = malloc(2 * sizeof(struct Bloc));
  //
  // ligne_bloc_Y[0] = matrice_Y_g;
  // ligne_bloc_Y[1] = matrice_Y_d;
  // ligne_bloc_Cb[0] = matrice_Cb_g;
  // ligne_bloc_Cb[1] = matrice_Cb_d;
  // ligne_bloc_Cr[0] = matrice_Cr_g;
  // ligne_bloc_Cr[1] = matrice_Cr_d;
  //
  // bloc_Y[0] = ligne_bloc_Y;
  // bloc_Cb[0] = ligne_bloc_Cb;
  // bloc_Cr[0] = ligne_bloc_Cr;

  struct Bloc **bloc_Y = malloc(2 * sizeof(struct Bloc*));
  struct Bloc **bloc_Cb = malloc(2 * sizeof(struct Bloc*));
  struct Bloc **bloc_Cr = malloc(2 * sizeof(struct Bloc*));

  struct Bloc *ligne_bloc_Y_1 = malloc(2 * sizeof(struct Bloc));
  struct Bloc *ligne_bloc_Y_2 = malloc(2 * sizeof(struct Bloc));
  struct Bloc *ligne_bloc_Cb_1 = malloc(2 * sizeof(struct Bloc));
  struct Bloc *ligne_bloc_Cb_2 = malloc(2 * sizeof(struct Bloc));
  struct Bloc *ligne_bloc_Cr_1 = malloc(2 * sizeof(struct Bloc));
  struct Bloc *ligne_bloc_Cr_2 = malloc(2 * sizeof(struct Bloc));

  ligne_bloc_Y_1[0] = matrice_Y_g;
  ligne_bloc_Y_2[0] = matrice_Y_g1;
  ligne_bloc_Cb_1[0] = matrice_Cb_g;
  ligne_bloc_Cb_2[0] = matrice_Cb_g1;
  ligne_bloc_Cr_1[0] = matrice_Cr_g;
  ligne_bloc_Cr_2[0] = matrice_Cr_g1;

  ligne_bloc_Y_1[1] = matrice_Y_d;
  ligne_bloc_Y_2[1] = matrice_Y_d1;
  ligne_bloc_Cb_1[1] = matrice_Cb_d;
  ligne_bloc_Cb_2[1] = matrice_Cb_d1;
  ligne_bloc_Cr_1[1] = matrice_Cr_d;
  ligne_bloc_Cr_2[1] = matrice_Cr_d1;

  bloc_Y[0] = ligne_bloc_Y_1;
  bloc_Y[1] = ligne_bloc_Y_2;
  bloc_Cb[0] = ligne_bloc_Cb_1;
  bloc_Cb[1] = ligne_bloc_Cb_2;
  bloc_Cr[0] = ligne_bloc_Cr_1;
  bloc_Cr[1] = ligne_bloc_Cr_2;

  MCU_Y->blocs_Y = bloc_Y;
  MCU_Y->blocs_Cb = bloc_Cb;
  MCU_Y->blocs_Cr = bloc_Cr;
  MCU_Y->hauteur_MCU = 2;
  MCU_Y->largeur_MCU = 2;
  MCU_Y->hauteur_bloc_Cb = 2;
  MCU_Y->largeur_bloc_Cb = 2;
  MCU_Y->hauteur_bloc_Cr = 2;
  MCU_Y->largeur_bloc_Cr = 2;

  // print_MCU_Y(MCU_Y);

  struct MCU_Y *test1 = sous_echantillonage(MCU_Y, 2, 2, 1, 1, 1, 1);
  // struct MCU_Y *test2 = sous_echantillonage_horizontal(test1, 2, 2, 1, 1, 1, 1);


  print_MCU_Y(test1);

  free_MCU(test1);
  free_MCU(MCU_Y);

}
