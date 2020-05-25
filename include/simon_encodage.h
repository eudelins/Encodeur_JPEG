#ifndef SIMON_ENCODAGE_H
#define SIMON_ENCODAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>


/************************************************************/
/* Partie consacrée à la transformation en cosinus discrète */
/************************************************************/


// /* Défini un Bloc_YCbCr */
// struct Bloc_YCbCr {
//     int16_t **pixels;      // un Bloc_YCbCr = une matrice de pixels
// };


// /* MCU après conversion en YCbCr */
// struct MCU_YCbCr{
//     uint8_t h1;
//     uint8_t v1;
//     uint8_t h2;
//     uint8_t v2;
//     uint8_t h3;
//     uint8_t v3;
//     struct Bloc_YCbCr **blocs_Y;
//     struct Bloc_YCbCr **blocs_Cb;
//     struct Bloc_YCbCr **blocs_Cr;
// };


/* Défini un bloc après DCT */
struct Bloc_freq {
    int16_t **pixels;      // un Bloc = une matrice de pixels
};


/* MCU après DCT */
struct MCU_freq_Y{
    uint8_t h1;
    uint8_t v1;
    uint8_t h2;
    uint8_t v2;
    uint8_t h3;
    uint8_t v3;
    struct Bloc_freq **blocs_Y_freq;
    struct Bloc_freq **blocs_Cb_freq;
    struct Bloc_freq **blocs_Cr_freq;
};


/* Applique la transformée en cosinus discrète à une MCU */
struct Bloc_freq transf_cos_bloc(struct Bloc_YCbCr Bloc);

/* Applique la transformée en cosinus discrète à une MCU YCbCr */
struct MCU_freq_Y *transf_cos_MCU_YCbCr(struct MCU_YCbCr *MCU);


/* Applique la transformée en cosinus discrète aux MCUs YCbCr */
struct MCU_freq_Y ***transf_cos_Y(struct MCU_YCbCr ***MCUs, uint32_t nb_MCUs_largeur, uint32_t nb_MCUs_hauteur);


/* Affiche un bloc transformée */
void print_bloc_freq(struct Bloc_freq Bloc_freq);


/* Affiche une MCU transformée */
void print_MCU_freq_Y(struct MCU_freq_Y *MCU_freq);



/* Affiche les MCUs transformées */
void print_MCUs_freq_Y(struct MCU_freq_Y ***MCUs_freq, uint32_t nb_MCUs_largeur, uint32_t nb_MCUs_hauteur);


/* Libère la mémoire alloué pour les pixels d'un bloc */
void free_pixel_Y(int16_t **pixels, uint32_t hauteur_pix);


/* Libère la mémoire alloué pour les pixels d'un bloc fréquentielle */
void free_pixel_freq_Y(int16_t **pixels, uint32_t hauteur_pix);


/* Libère la mémoire allouée à une MCU fréquentielle */
void free_MCU_freq_Y(struct MCU_freq_Y *MCU_freq);



/* Libère la mémoire allouée aux MCUs fréquentielles */
void free_MCUs_freq_Y(struct MCU_freq_Y ***MCUs_freq, uint32_t nb_MCUs_largeur, uint32_t nb_MCUs_hauteur);


/**********************************************************************/
/* Fin de la partie consacrée à la transformation en cosinus discrète */
/**********************************************************************/


/**************************************************/
/* Partie consacrée à la réorganisation en zigzag */
/**************************************************/


/* Défini un bloc après DCT */
struct Bloc_zigzag {
    int16_t *pixels;      // un Bloc = une matrice de pixels
};


/* MCU après DCT */
struct MCU_zigzag_Y{
    uint8_t h1;
    uint8_t v1;
    uint8_t h2;
    uint8_t v2;
    uint8_t h3;
    uint8_t v3;
    struct Bloc_zigzag **blocs_Y_zigzag;
    struct Bloc_zigzag **blocs_Cb_zigzag;
    struct Bloc_zigzag **blocs_Cr_zigzag;
};


/* Calcul le chemin en zigzag d'un bloc */
struct Bloc_zigzag zigzag_bloc(struct Bloc_freq Bloc_freq);


/* Applique la transformée en cosinus discrète à une MCU YCbCr */
struct MCU_zigzag_Y *zigzag_MCU_Y(struct MCU_freq_Y *MCU_freq);


/* Applique la réorganisation en zigzag aux MCUs */
struct MCU_zigzag_Y ***zigzag_Y(struct MCU_freq_Y ***MCUs_freq, uint32_t nb_MCUs_largeur, uint32_t nb_MCUs_hauteur);


/* Affiche un bloc transformée */
void print_bloc_zigzag(struct Bloc_zigzag Bloc_zigzag);


/* Affiche une MCU transformée */
void print_MCU_zigzag_Y(struct MCU_zigzag_Y *MCU_zigzag);


/* Affiche les MCUs transformées */
void print_MCUs_zigzag_Y(struct MCU_zigzag_Y ***MCUs_zigzag, uint32_t nb_MCUs_largeur, uint32_t nb_MCUs_hauteur);


/* Libère la mémoire allouée à une MCU fréquentielle */
void free_MCU_zigzag_Y(struct MCU_zigzag_Y *MCU_zigzag);


/* Libère la mémoire allouée aux MCUs fréquentielles */
void free_MCUs_zigzag_Y(struct MCU_zigzag_Y ***MCUs_zigzag, uint32_t nb_MCUs_largeur, uint32_t nb_MCUs_hauteur);


/************************************************************/
/* Fin de la partie consacrée à la réorganisation en zigzag */
/************************************************************/


/****************************************/
/* Partie consacrée à la quantification */
/****************************************/

/* Applique la quantification à un bloc */
void quantification_bloc_Y(struct Bloc_zigzag *Bloc_zigzag);


/* Applique la quantification à un bloc */
void quantification_bloc_CbCr(struct Bloc_zigzag *Bloc_zigzag);


/* Applique la quantification à une MCU */
void quantification_MCU_couleur(struct MCU_zigzag_Y *MCU_zigzag);


/* Applique la quantification aux MCUs */
void quantification_couleur(struct MCU_zigzag_Y ***MCUs_zigzag, uint32_t largeur_MCUs, uint32_t hauteur_MCUs);


/**************************************************/
/* Fin de la partie consacrée à la quantification */
/**************************************************/


#endif /* SIMON_ENCODAGE_H */