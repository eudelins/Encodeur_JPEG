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
    uint8_t **pixels;      // un Bloc = une matrice de pixels
};


/* Défini une MCU */
struct MCU {
    uint8_t largeur;
    uint8_t hauteur;
    struct Pixel_RGB **pixels;      // une MCU = une matrice de pixels
    struct Bloc **blocs;            // une MCU = une matrice de blocs
};


/* MCU après conversion en YCbCr */
struct MCU_Y{
    uint8_t largeur;
    uint8_t hauteur;
    struct Bloc **blocs_Y;
    struct Bloc **blocs_Cb;
    struct Bloc **blocs_Cr;
};


/*******************************************/
/* Partie consacrée au sous-echantillonage */
/*******************************************/


struct MCU_Y *sous_echantillonage_horizontal(struct MCU_Y *MCU_Y,
                                  uint8_t h1,
                                  uint8_t v1,
                                  uint8_t h2,
                                  uint8_t v2,
                                  uint8_t h3,
                                  uint8_t v3)
{
  /* On sous-echantillone les Cb */
  struct Bloc ***sous_blocs_Cb = malloc(v2 * sizeof(struct Bloc**));
  for (uint8_t hauteur = 0; hauteur < v2; hauteur++) {
    struct Bloc **sous_ligne_Cb = malloc(h2 * sizeof(struct Bloc*));
    for (uint8_t largeur = 0; largeur < h2; largeur++) {



      /* On fait la moyenne des pixels */
      struct Bloc *sous_bloc_Cb = malloc(sizeof(struct Bloc));
      uint8_t **pixels_Cb = malloc(8 * sizeof(uint8_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        uint8_t *ligne_pixels_Cb = malloc(8 * sizeof(uint8_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          // uint8_t *pixel_cb = malloc(sizeof(uint8_t));
          // pixel_cb[0] = (MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix] + MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix + 1])/2;
          ligne_pixels_Cb[largeur_pix] = (MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * (largeur_pix%4)] + MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * (largeur_pix%4) + 1])/2;
        }
        pixels_Cb[hauteur_pix] = ligne_pixels_Cb;
      }
      sous_bloc_Cb->pixels = pixels_Cb;
      sous_ligne_Cb[largeur] = sous_bloc_Cb;
    }
    sous_blocs_Cb[hauteur] = sous_ligne_Cb;
  }


  /* On sous-echantillone les Cr */

  struct Bloc ***sous_blocs_Cr = malloc(v3 * sizeof(struct Bloc**));
  for (uint8_t hauteur = 0; hauteur < v3; hauteur++) {
    struct Bloc **sous_ligne_Cr = malloc(h3 * sizeof(struct Bloc*));
    for (uint8_t largeur = 0; largeur < h3; largeur++) {



      /* On fait la moyenne des pixels */
      struct Bloc *sous_bloc_Cr = malloc(sizeof(struct Bloc));
      uint8_t **pixels_Cr = malloc(8 * sizeof(uint8_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        uint8_t *ligne_pixels_Cr = malloc(8 * sizeof(uint8_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          // uint8_t *pixel_cb = malloc(sizeof(uint8_t));
          // pixel_cb[0] = (MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix] + MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix + 1])/2;
          ligne_pixels_Cr[largeur_pix] = (MCU_Y->blocs_Cr[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * (largeur_pix%4)] + MCU_Y->blocs_Cr[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * (largeur_pix%4) + 1])/2;
        }
        pixels_Cr[hauteur_pix] = ligne_pixels_Cr;
      }
      sous_bloc_Cr->pixels = pixels_Cr;
      sous_ligne_Cr[largeur] = sous_bloc_Cr;
    }
    sous_blocs_Cr[hauteur] = sous_ligne_Cr;
  }

  struct MCU_Y *sous_MCU = malloc(sizeof(struct MCU_Y));
  sous_MCU->hauteur = v1;
  sous_MCU->largeur = h1;
  sous_MCU->blocs_Y = MCU_Y->blocs_Y;
  sous_MCU->blocs_Cb = *sous_blocs_Cb;
  sous_MCU->blocs_Cr = *sous_blocs_Cr;

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
  /* On sous-echantillone les Cb */
  struct Bloc ***sous_blocs_Cb = malloc(v2 * sizeof(struct Bloc**));
  for (uint8_t hauteur = 0; hauteur < v2; hauteur++) {
    struct Bloc **sous_ligne_Cb = malloc(h2 * sizeof(struct Bloc*));
    for (uint8_t largeur = 0; largeur < h2; largeur++) {



      /* On fait la moyenne des pixels */
      struct Bloc *sous_bloc_Cb = malloc(sizeof(struct Bloc));
      uint8_t **pixels_Cb = malloc(8 * sizeof(uint8_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        uint8_t *ligne_pixels_Cb = malloc(8 * sizeof(uint8_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          // uint8_t *pixel_cb = malloc(sizeof(uint8_t));
          // pixel_cb[0] = (MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix] + MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix + 1])/2;
          ligne_pixels_Cb[largeur_pix] = (MCU_Y->blocs_Cb[hauteur_pix/4][largeur].pixels[2 * (hauteur_pix%4)][largeur_pix] + MCU_Y->blocs_Cb[hauteur_pix/4][largeur].pixels[2 * (hauteur_pix%4) + 1][largeur_pix])/2;
        }
        pixels_Cb[hauteur_pix] = ligne_pixels_Cb;
      }
      sous_bloc_Cb->pixels = pixels_Cb;
      sous_ligne_Cb[largeur] = sous_bloc_Cb;
    }
    sous_blocs_Cb[hauteur] = sous_ligne_Cb;
  }


  /* On sous-echantillone les Cr */

  struct Bloc ***sous_blocs_Cr = malloc(v3 * sizeof(struct Bloc**));
  for (uint8_t hauteur = 0; hauteur < v3; hauteur++) {
    struct Bloc **sous_ligne_Cr = malloc(h3 * sizeof(struct Bloc*));
    for (uint8_t largeur = 0; largeur < h3; largeur++) {



      /* On fait la moyenne des pixels */
      struct Bloc *sous_bloc_Cr = malloc(sizeof(struct Bloc));
      uint8_t **pixels_Cr = malloc(8 * sizeof(uint8_t*));
      for (uint8_t hauteur_pix = 0; hauteur_pix < 8; hauteur_pix++) {
        uint8_t *ligne_pixels_Cr = malloc(8 * sizeof(uint8_t));
        for (uint8_t largeur_pix = 0; largeur_pix < 8; largeur_pix++) {
          // uint8_t *pixel_cb = malloc(sizeof(uint8_t));
          // pixel_cb[0] = (MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix] + MCU_Y->blocs_Cb[hauteur][largeur_pix/4].pixels[hauteur_pix][2 * largeur_pix + 1])/2;
          ligne_pixels_Cr[largeur_pix] = (MCU_Y->blocs_Cr[hauteur_pix/4][largeur].pixels[2 * (hauteur_pix%4)][largeur_pix] + MCU_Y->blocs_Cr[hauteur_pix/4][largeur].pixels[2 * (hauteur_pix%4) + 1][largeur_pix])/2;
        }
        pixels_Cr[hauteur_pix] = ligne_pixels_Cr;
      }
      sous_bloc_Cr->pixels = pixels_Cr;
      sous_ligne_Cr[largeur] = sous_bloc_Cr;
    }
    sous_blocs_Cr[hauteur] = sous_ligne_Cr;
  }

  struct MCU_Y *sous_MCU = malloc(sizeof(struct MCU_Y));
  sous_MCU->hauteur = MCU_Y->hauteur;
  sous_MCU->largeur = MCU_Y->largeur;
  sous_MCU->blocs_Y = MCU_Y->blocs_Y;
  sous_MCU->blocs_Cb = *sous_blocs_Cb;
  sous_MCU->blocs_Cr = *sous_blocs_Cr;

  return sous_MCU;
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
  print_bloc(MCU_Y->blocs_Y[0][0]);
  printf("\n");
  print_bloc(MCU_Y->blocs_Y[0][1]);
  printf("\n");
  print_bloc(MCU_Y->blocs_Y[1][0]);
  printf("\n");
  print_bloc(MCU_Y->blocs_Y[1][1]);
  printf("Blocs Cb \n");
  print_bloc(MCU_Y->blocs_Cb[0][0]);
  printf("\n");
  print_bloc(MCU_Y->blocs_Cb[1][0]);
  printf("Blocs Cr \n");
  print_bloc(MCU_Y->blocs_Cr[0][0]);
  printf("\n");
  print_bloc(MCU_Y->blocs_Cr[1][0]);
}


void main()
{
  struct MCU_Y *MCU_Y = malloc(sizeof(struct MCU_Y));
  MCU_Y->hauteur = 1, MCU_Y->largeur = 2;

  uint8_t mat_Y_g [8][8] = {  {0xa6, 0xa0, 0x9a, 0x98, 0x9a, 0x9a, 0x96, 0x91},
                              {0xa0, 0xa3, 0x9d, 0x8e, 0x88, 0x8f, 0x95, 0x94},
                              {0xa5, 0x97, 0x96, 0xa1, 0x9f, 0x90, 0x90, 0x9e},
                              {0xa6, 0x9a, 0x91, 0x91, 0x92, 0x90, 0x90, 0x93},
                              {0xc9, 0xd9, 0xc8, 0x98, 0x85, 0x98, 0xa2, 0x95},
                              {0xf0, 0xf5, 0xf9, 0xea, 0xbf, 0x98, 0x90, 0x9d},
                              {0xe9, 0xe1, 0xf3, 0xfd, 0xf2, 0xaf, 0x8a, 0x90},
                              {0xe6, 0xf2, 0xf1, 0xed, 0xf8, 0xfb, 0xd0, 0x95}};

  uint8_t mat_Y_d [8][8] = { {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
                                {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
                                {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa6, 0xa7},
                                {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
                                {0x9d, 0xa0, 0xa3, 0xa4, 0xa3, 0xa3, 0xa4, 0xa6},
                                {0x9b, 0x9e, 0xa1, 0xa2, 0xa1, 0xa1, 0xa3, 0xa4},
                                {0x99, 0x9c, 0x9f, 0xa0, 0x9f, 0x9f, 0xa1, 0xa2},
                                {0x98, 0x9b, 0x9e, 0x9f, 0x9e, 0x9e, 0xa0, 0xa1}};

  uint8_t mat_Cb_g [8][8] = { {0x75, 0x75, 0x75, 0x75, 0x76, 0x76, 0x77, 0x78},
                                  {0x75, 0x75, 0x75, 0x75, 0x76, 0x76, 0x77, 0x77},
                                  {0x75, 0x75, 0x76, 0x76, 0x76, 0x76, 0x77, 0x77},
                                  {0x76, 0x76, 0x76, 0x76, 0x77, 0x77, 0x77, 0x77},
                                  {0x76, 0x76, 0x77, 0x77, 0x77, 0x77, 0x78, 0x78},
                                  {0x77, 0x78, 0x78, 0x77, 0x77, 0x77, 0x78, 0x78},
                                  {0x77, 0x77, 0x77, 0x78, 0x78, 0x78, 0x78, 0x78},
                                  {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78}};

  uint8_t mat_Cb_d [8][8] = { {0x78, 0x79, 0x7a, 0x7a, 0x7b, 0x7b, 0x7b, 0x7b},
                                  {0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7b, 0x7b},
                                  {0x78, 0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7a},
                                  {0x78, 0x78, 0x79, 0x79, 0x7a, 0x7a, 0x7a, 0x7a},
                                  {0x78, 0x78, 0x79, 0x79, 0x79, 0x79, 0x79, 0x79},
                                  {0x78, 0x78, 0x78, 0x78, 0x79, 0x79, 0x79, 0x79},
                                  {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78},
                                  {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78}};

  uint8_t mat_Cr_g [8][8] = { {0x8d, 0x8c, 0x89, 0x87, 0x85, 0x85, 0x86, 0x89},
                                  {0x8d, 0x8c, 0x89, 0x87, 0x85, 0x85, 0x86, 0x88},
                                  {0x8c, 0x8b, 0x89, 0x87, 0x85, 0x85, 0x85, 0x88},
                                  {0x8c, 0x8b, 0x88, 0x86, 0x85, 0x85, 0x85, 0x88},
                                  {0x8c, 0x8b, 0x88, 0x86, 0x84, 0x84, 0x85, 0x87},
                                  {0x8b, 0x87, 0x84, 0x86, 0x84, 0x84, 0x84, 0x87},
                                  {0x8b, 0x8a, 0x87, 0x81, 0x84, 0x84, 0x84, 0x87},
                                  {0x8b, 0x89, 0x87, 0x85, 0x84, 0x83, 0x84, 0x87}};

  uint8_t mat_Cr_d [8][8] = { {0x8b, 0x8e, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x88},
                                  {0x8b, 0x8d, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x87},
                                  {0x8a, 0x8d, 0x8e, 0x8e, 0x8c, 0x8a, 0x87, 0x87},
                                  {0x8a, 0x8d, 0x8d, 0x8d, 0x8c, 0x8a, 0x87, 0x86},
                                  {0x8a, 0x8c, 0x8d, 0x8d, 0x8b, 0x89, 0x86, 0x85},
                                  {0x89, 0x8c, 0x8d, 0x8d, 0x8b, 0x89, 0x86, 0x86},
                                  {0x89, 0x8c, 0x8c, 0x8c, 0x8b, 0x89, 0x86, 0x86},
                                  {0x89, 0x8c, 0x8c, 0x8c, 0x8b, 0x89, 0x86, 0x86}};

  uint8_t **pixels_Y_g = malloc(8 * sizeof(uint8_t *));
  uint8_t **pixels_Y_d = malloc(8 * sizeof(uint8_t *));
  uint8_t **pixels_Cb_g = malloc(8 * sizeof(uint8_t *));
  uint8_t **pixels_Cb_d = malloc(8 * sizeof(uint8_t *));
  uint8_t **pixels_Cr_g = malloc(8 * sizeof(uint8_t *));
  uint8_t **pixels_Cr_d = malloc(8 * sizeof(uint8_t *));

  for (uint8_t i = 0; i < 8; i++){
      uint8_t *ligne_pixels_Y_g = malloc(8 * sizeof(uint8_t));
      uint8_t *ligne_pixels_Y_d = malloc(8 * sizeof(uint8_t));
      uint8_t *ligne_pixels_Cb_g = malloc(8 * sizeof(uint8_t));
      uint8_t *ligne_pixels_Cb_d = malloc(8 * sizeof(uint8_t));
      uint8_t *ligne_pixels_Cr_g = malloc(8 * sizeof(uint8_t));
      uint8_t *ligne_pixels_Cr_d = malloc(8 * sizeof(uint8_t));
      for (uint8_t j = 0; j <8; j++){
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
  ligne_bloc_Y_2[0] = matrice_Y_g;
  ligne_bloc_Cb_1[0] = matrice_Cb_g;
  ligne_bloc_Cb_2[0] = matrice_Cb_g;
  ligne_bloc_Cr_1[0] = matrice_Cr_g;
  ligne_bloc_Cr_2[0] = matrice_Cr_g;

  ligne_bloc_Y_1[1] = matrice_Y_d;
  ligne_bloc_Y_2[1] = matrice_Y_d;
  ligne_bloc_Cb_1[1] = matrice_Cb_d;
  ligne_bloc_Cb_2[1] = matrice_Cb_d;
  ligne_bloc_Cr_1[1] = matrice_Cr_d;
  ligne_bloc_Cr_2[1] = matrice_Cr_d;

  bloc_Y[0] = ligne_bloc_Y_1;
  bloc_Y[1] = ligne_bloc_Y_2;
  bloc_Cb[0] = ligne_bloc_Cb_1;
  bloc_Cb[1] = ligne_bloc_Cb_2;
  bloc_Cr[0] = ligne_bloc_Cr_1;
  bloc_Cr[1] = ligne_bloc_Cr_2;

  MCU_Y->blocs_Y = bloc_Y;
  MCU_Y->blocs_Cb = bloc_Cb;
  MCU_Y->blocs_Cr = bloc_Cr;

  // print_MCU_Y(MCU_Y);

  struct MCU_Y *test = sous_echantillonage_horizontal(MCU_Y, 2, 2, 1, 2, 1, 2);

  print_MCU_Y(test);

}
