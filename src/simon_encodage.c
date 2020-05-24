#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define M_PI 3.14159265358979323846

#include "../include/qtables.h"


/************************************************************/
/* Partie consacrée à la transformation en cosinus discrète */
/************************************************************/


/* Défini un Bloc_YCbCr */
struct Bloc_YCbCr {
    int16_t **pixels;      // un Bloc_YCbCr = une matrice de pixels
};


/* MCU après conversion en YCbCr */
struct MCU_YCbCr{
    uint8_t h1;
    uint8_t v1;
    uint8_t h2;
    uint8_t v2;
    uint8_t h3;
    uint8_t v3;
    struct Bloc_YCbCr **blocs_Y;
    struct Bloc_YCbCr **blocs_Cb;
    struct Bloc_YCbCr **blocs_Cr;
};


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


/* Calcul C(i) */
double C(uint8_t i)
{
    if (i == 0) {
        return 0.707106781186547;
    } else {
        return 1.0;
    }
}



/* Applique la transformée en cosinus discrète à une MCU */
struct Bloc_freq transf_cos_bloc(struct Bloc_YCbCr Bloc)
{    
    int16_t **pixels_freq = malloc(8 * sizeof(int16_t *));
    for (uint8_t i = 0; i < 8; i++){
        int16_t *ligne_pixels_freq = malloc(8 * sizeof(int16_t));
        
        for (uint8_t j = 0; j < 8; j++){

            // Calcul de la coordonnées (i, j) du domaine frequentiel
            double freq = 0.0;
            for (uint8_t x = 0; x < 8; x++){        
                for (uint8_t y = 0; y < 8; y++){
                    freq += (Bloc.pixels[x][y] - 128) * cos((2*x + 1)*i*M_PI/16) * cos((2*y + 1)*j*M_PI/16);
                }
            }
            freq = C(i) * C(j) * freq/4.0;

            ligne_pixels_freq[j] = (int16_t) freq;
        }
        pixels_freq[i] = ligne_pixels_freq;
    }
    
    struct Bloc_freq Bloc_freq;
    Bloc_freq.pixels = pixels_freq;

    return Bloc_freq;
}


/* Applique la transformée en cosinus discrète à une MCU YCbCr */
struct MCU_freq_Y *transf_cos_MCU_YCbCr(struct MCU_YCbCr *MCU)
{    
    struct MCU_freq_Y *MCU_freq = malloc(sizeof(struct MCU_freq_Y));
    MCU_freq->h1 = MCU->h1;
    MCU_freq->v1 = MCU->v1;
    MCU_freq->h2 = MCU->h2;
    MCU_freq->v2 = MCU->v2;
    MCU_freq->h3 = MCU->h3;
    MCU_freq->v3 = MCU->v3;
    
    // Encodage des blocs Y
    struct Bloc_freq **blocs_Y_freq = malloc(MCU_freq->h1 * sizeof(struct Bloc_freq *));
    for (uint8_t hauteur_Y = 0; hauteur_Y < MCU_freq->h1; hauteur_Y++) {
        struct Bloc_freq *ligne_blocs_Y_freq = malloc(MCU_freq->v1 * sizeof(struct Bloc_freq));
        for (uint8_t largeur_Y = 0; largeur_Y < MCU_freq->v1; largeur_Y++){
            ligne_blocs_Y_freq[largeur_Y] = transf_cos_bloc(MCU->blocs_Y[hauteur_Y][largeur_Y]);
        }
        blocs_Y_freq[hauteur_Y] = ligne_blocs_Y_freq;
    }
    MCU_freq->blocs_Y_freq = blocs_Y_freq;

    // Encodage des blocs Cb
    struct Bloc_freq **blocs_Cb_freq = malloc(MCU_freq->h2 * sizeof(struct Bloc_freq *));
    for (uint8_t hauteur_Cb = 0; hauteur_Cb < MCU_freq->h2; hauteur_Cb++) {
        struct Bloc_freq *ligne_blocs_Cb_freq = malloc(MCU_freq->v2 * sizeof(struct Bloc_freq));
        for (uint8_t largeur_Cb = 0; largeur_Cb < MCU_freq->v2; largeur_Cb++){
            ligne_blocs_Cb_freq[largeur_Cb] = transf_cos_bloc(MCU->blocs_Cb[hauteur_Cb][largeur_Cb]);
        }
        blocs_Cb_freq[hauteur_Cb] = ligne_blocs_Cb_freq;
    }
    MCU_freq->blocs_Cb_freq = blocs_Cb_freq;

    // Encodage des blocs Cr
    struct Bloc_freq **blocs_Cr_freq = malloc(MCU_freq->h3 * sizeof(struct Bloc_freq *));
    for (uint8_t hauteur_Cr = 0; hauteur_Cr < MCU_freq->h3; hauteur_Cr++) {
        struct Bloc_freq *ligne_blocs_Cr_freq = malloc(MCU_freq->v3 * sizeof(struct Bloc_freq));
        for (uint8_t largeur_Cr = 0; largeur_Cr < MCU_freq->v3; largeur_Cr++){
            ligne_blocs_Cr_freq[largeur_Cr] = transf_cos_bloc(MCU->blocs_Cr[hauteur_Cr][largeur_Cr]);
        }
        blocs_Cr_freq[hauteur_Cr] = ligne_blocs_Cr_freq;
    }
    MCU_freq->blocs_Cr_freq = blocs_Cr_freq;
    return MCU_freq;
}


/* Applique la transformée en cosinus discrète aux MCUs YCbCr */
struct MCU_freq_Y ***transf_cos_Y(struct MCU_YCbCr ***MCUs, uint32_t nb_MCUs_largeur, uint32_t nb_MCUs_hauteur)
{ 
    struct MCU_freq_Y ***MCUs_freq = malloc(nb_MCUs_hauteur * sizeof(struct MCU_freq_Y**));
    for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur; hauteur++){
        struct MCU_freq_Y **ligne_MCUs_freq = malloc(nb_MCUs_largeur * sizeof(struct MCU_freq_Y*));
        for (uint32_t largeur = 0; largeur < nb_MCUs_largeur; largeur++){
            struct MCU_freq_Y *MCU_freq = transf_cos_MCU_YCbCr(MCUs[hauteur][largeur]);
            ligne_MCUs_freq[largeur] = MCU_freq;
        }
        MCUs_freq[hauteur] = ligne_MCUs_freq;
    }
    return MCUs_freq;
}


/* Affiche un bloc transformée */
void print_bloc_freq(struct Bloc_freq Bloc_freq)
{
    uint8_t largeur_bloc = 8, hauteur_bloc = 8;
    int16_t **pixels_freq = Bloc_freq.pixels;
    for (uint8_t hauteur_pix = 0; hauteur_pix < hauteur_bloc; hauteur_pix++){
        for (uint8_t largeur_pix = 0; largeur_pix < largeur_bloc; largeur_pix++){
            printf("%hx\t", pixels_freq[hauteur_pix][largeur_pix]);
        }
        printf("\n");        
    }
}


/* Affiche une MCU transformée */
void print_MCU_freq_Y(struct MCU_freq_Y *MCU_freq)
{
    // Print des blocs Y
    for (uint8_t hauteur_Y = 0; hauteur_Y < MCU_freq->h1; hauteur_Y++) {
        for (uint8_t largeur_Y = 0; largeur_Y < MCU_freq->v1; largeur_Y++){
            printf("Bloc Y numéro: %d\n", hauteur_Y * MCU_freq->v1 + largeur_Y);
            print_bloc_freq(MCU_freq->blocs_Y_freq[hauteur_Y][largeur_Y]);
            printf("\n");
        }
    }
    printf("\n");

    // Print des blocs Cb
    for (uint8_t hauteur_Cb = 0; hauteur_Cb < MCU_freq->h2; hauteur_Cb++) {
        for (uint8_t largeur_Cb = 0; largeur_Cb < MCU_freq->v2; largeur_Cb++){
            printf("Bloc Cb numéro: %d\n", hauteur_Cb * MCU_freq->v2 + largeur_Cb);
            print_bloc_freq(MCU_freq->blocs_Cb_freq[hauteur_Cb][largeur_Cb]);
            printf("\n");
        }
    }
    printf("\n");

    // Print des blocs Cr
    for (uint8_t hauteur_Cr = 0; hauteur_Cr < MCU_freq->h3; hauteur_Cr++) {
        for (uint8_t largeur_Cr = 0; largeur_Cr < MCU_freq->v3; largeur_Cr++){
            printf("Bloc Cr numéro: %d\n", hauteur_Cr * MCU_freq->v3 + largeur_Cr);
            print_bloc_freq(MCU_freq->blocs_Cr_freq[hauteur_Cr][largeur_Cr]);
            printf("\n");
        }
    }}



/* Affiche les MCUs transformées */
void print_MCUs_freq_Y(struct MCU_freq_Y ***MCUs_freq, uint32_t nb_MCUs_largeur, uint32_t nb_MCUs_hauteur)
{
    for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur; hauteur++){
        for (uint32_t largeur =  0; largeur < nb_MCUs_largeur; largeur++){
            printf("MCU %d:\n", hauteur * nb_MCUs_largeur + largeur);
            print_MCU_freq_Y(MCUs_freq[hauteur][largeur]);
            printf("\n");
        }
    }
}


/* Libère la mémoire alloué pour les pixels d'un bloc */
void free_pixel(int16_t **pixels, uint32_t hauteur_pix)
{
    for (int32_t hauteur = hauteur_pix - 1; hauteur >= 0; hauteur--){
        free(pixels[hauteur]);
    }
    free(pixels);
}


/* Libère la mémoire alloué pour les pixels d'un bloc fréquentielle */
void free_pixel_freq(int16_t **pixels, uint32_t hauteur_pix)
{
    for (int32_t hauteur = hauteur_pix - 1; hauteur >= 0; hauteur--){
        free(pixels[hauteur]);
    }
    free(pixels);
}


/* Libère la mémoire allouée à une MCU fréquentielle */
void free_MCU_freq_Y(struct MCU_freq_Y *MCU_freq)
{
    // free des blocs Y
    for (uint8_t hauteur_Y = 0; hauteur_Y < MCU_freq->h1; hauteur_Y++) {
        for (uint8_t largeur_Y = 0; largeur_Y < MCU_freq->v1; largeur_Y++){
            free_pixel_freq(MCU_freq->blocs_Y_freq[hauteur_Y][largeur_Y].pixels, 8);
        }
        free(MCU_freq->blocs_Y_freq[hauteur_Y]);
    }
    free(MCU_freq->blocs_Y_freq);

    // free des blocs Cb
    for (uint8_t hauteur_Cb = 0; hauteur_Cb < MCU_freq->h2; hauteur_Cb++) {
        for (uint8_t largeur_Cb = 0; largeur_Cb < MCU_freq->v2; largeur_Cb++){
            free_pixel_freq(MCU_freq->blocs_Cb_freq[hauteur_Cb][largeur_Cb].pixels, 8);
        }
        free(MCU_freq->blocs_Cb_freq[hauteur_Cb]);
    }
    free(MCU_freq->blocs_Cb_freq);

    // free des blocs Cr
    for (uint8_t hauteur_Cr = 0; hauteur_Cr < MCU_freq->h3; hauteur_Cr++) {
        for (uint8_t largeur_Cr = 0; largeur_Cr < MCU_freq->v3; largeur_Cr++){
            free_pixel_freq(MCU_freq->blocs_Cr_freq[hauteur_Cr][largeur_Cr].pixels, 8);
        }
        free(MCU_freq->blocs_Cr_freq[hauteur_Cr]);
    }
    free(MCU_freq->blocs_Cr_freq);

    free(MCU_freq);
}



/* Libère la mémoire allouée aux MCUs fréquentielles */
void free_MCUs_freq_Y(struct MCU_freq_Y ***MCUs_freq, uint32_t nb_MCUs_largeur, uint32_t nb_MCUs_hauteur)
{
    for (int32_t hauteur = nb_MCUs_hauteur - 1; hauteur >= 0; hauteur--){
        for (int32_t largeur = nb_MCUs_largeur - 1; largeur >= 0; largeur--){
            free_MCU_freq_Y(MCUs_freq[hauteur][largeur]);
        }
        free(MCUs_freq[hauteur]);
    }
    free(MCUs_freq);
}



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
struct Bloc_zigzag zigzag_bloc(struct Bloc_freq Bloc_freq)
{
    struct Bloc_zigzag Bloc_zigzag;
    
    int16_t *zigzag = malloc(64 * sizeof(int16_t));
    uint8_t compteur_elem = 0;
    
    // Première partie du zigzag
    for (uint8_t i = 0; i < 8; i++){
        for (uint8_t j = 0; j <= i; j++){
            if (i % 2 == 0){
                zigzag[compteur_elem] = Bloc_freq.pixels[i - j][j];
                compteur_elem += 1;
            } else {
                zigzag[compteur_elem] = Bloc_freq.pixels[j][i - j];
                compteur_elem += 1;
            }
        }
    }

    // Deuxième partie
    for (uint8_t i = 0; i < 8; i++){
        for (uint8_t j = i + 1; j <= 7; j++){
            if (i % 2 == 0){
                zigzag[compteur_elem] = Bloc_freq.pixels[8 + i - j][j];
                compteur_elem += 1;
            } else {
                zigzag[compteur_elem] = Bloc_freq.pixels[j][8 + i - j];
                compteur_elem += 1;
            }
        }
    }
    Bloc_zigzag.pixels = zigzag;
    
    return Bloc_zigzag;
}


/* Applique la transformée en cosinus discrète à une MCU YCbCr */
struct MCU_zigzag_Y *zigzag_MCU_Y(struct MCU_freq_Y *MCU_freq)
{    
    struct MCU_zigzag_Y *MCU_zigzag = malloc(sizeof(struct MCU_zigzag_Y));
    MCU_zigzag->h1 = MCU_freq->h1;
    MCU_zigzag->v1 = MCU_freq->v1;
    MCU_zigzag->h2 = MCU_freq->h2;
    MCU_zigzag->v2 = MCU_freq->v2;
    MCU_zigzag->h3 = MCU_freq->h3;
    MCU_zigzag->v3 = MCU_freq->v3;
    
    // zigzag des blocs Y
    struct Bloc_zigzag **blocs_Y_zigzag = malloc(MCU_zigzag->h1 * sizeof(struct Bloc_zigzag *));
    for (uint8_t hauteur_Y = 0; hauteur_Y < MCU_zigzag->h1; hauteur_Y++) {
        struct Bloc_zigzag *ligne_blocs_Y_zigzag = malloc(MCU_zigzag->v1 * sizeof(struct Bloc_zigzag));
        for (uint8_t largeur_Y = 0; largeur_Y < MCU_zigzag->v1; largeur_Y++){
            ligne_blocs_Y_zigzag[largeur_Y] = zigzag_bloc(MCU_freq->blocs_Y_freq[hauteur_Y][largeur_Y]);
        }
        blocs_Y_zigzag[hauteur_Y] = ligne_blocs_Y_zigzag;
    }
    MCU_zigzag->blocs_Y_zigzag = blocs_Y_zigzag;

    // zigzag des blocs Cb
    struct Bloc_zigzag **blocs_Cb_zigzag = malloc(MCU_zigzag->h2 * sizeof(struct Bloc_zigzag *));
    for (uint8_t hauteur_Cb = 0; hauteur_Cb < MCU_zigzag->h2; hauteur_Cb++) {
        struct Bloc_zigzag *ligne_blocs_Cb_zigzag = malloc(MCU_zigzag->v2 * sizeof(struct Bloc_zigzag));
        for (uint8_t largeur_Cb = 0; largeur_Cb < MCU_zigzag->v2; largeur_Cb++){
            ligne_blocs_Cb_zigzag[largeur_Cb] = zigzag_bloc(MCU_freq->blocs_Cb_freq[hauteur_Cb][largeur_Cb]);
        }
        blocs_Cb_zigzag[hauteur_Cb] = ligne_blocs_Cb_zigzag;
    }
    MCU_zigzag->blocs_Cb_zigzag = blocs_Cb_zigzag;

    // zigzag des blocs Cr
    struct Bloc_zigzag **blocs_Cr_zigzag = malloc(MCU_zigzag->h3 * sizeof(struct Bloc_zigzag *));
    for (uint8_t hauteur_Cr = 0; hauteur_Cr < MCU_zigzag->h3; hauteur_Cr++) {
        struct Bloc_zigzag *ligne_blocs_Cr_zigzag = malloc(MCU_zigzag->v3 * sizeof(struct Bloc_zigzag));
        for (uint8_t largeur_Cr = 0; largeur_Cr < MCU_zigzag->v3; largeur_Cr++){
            ligne_blocs_Cr_zigzag[largeur_Cr] = zigzag_bloc(MCU_freq->blocs_Cr_freq[hauteur_Cr][largeur_Cr]);
        }
        blocs_Cr_zigzag[hauteur_Cr] = ligne_blocs_Cr_zigzag;
    }
    MCU_zigzag->blocs_Cr_zigzag = blocs_Cr_zigzag;
    return MCU_zigzag;
}


/* Applique la réorganisation en zigzag aux MCUs */
struct MCU_zigzag_Y ***zigzag_Y(struct MCU_freq_Y ***MCUs_freq, uint32_t nb_MCUs_largeur, uint32_t nb_MCUs_hauteur)
{ 
    struct MCU_zigzag_Y ***MCUs_zigzag = malloc(nb_MCUs_hauteur * sizeof(struct MCU_zigzag_Y**));
    for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur; hauteur++){
        struct MCU_zigzag_Y **ligne_MCUs_zigzag = malloc(nb_MCUs_largeur * sizeof(struct MCU_zigzag_Y*));
        for (uint32_t largeur = 0; largeur < nb_MCUs_largeur; largeur++){
            struct MCU_zigzag_Y *MCU_zigzag = zigzag_MCU_Y(MCUs_freq[hauteur][largeur]);
            ligne_MCUs_zigzag[largeur] = MCU_zigzag;
        }
        MCUs_zigzag[hauteur] = ligne_MCUs_zigzag;
    }
    return MCUs_zigzag;
}


/* Affiche un bloc transformée */
void print_bloc_zigzag(struct Bloc_zigzag Bloc_zigzag)
{
    uint8_t largeur_bloc = 8, hauteur_bloc = 8;
    int16_t *pixels_zigzag = Bloc_zigzag.pixels;
    for (uint8_t hauteur_pix = 0; hauteur_pix < hauteur_bloc; hauteur_pix++){
        for (uint8_t largeur_pix = 0; largeur_pix < largeur_bloc; largeur_pix++){
            printf("%hx\t", pixels_zigzag[hauteur_pix * 8 + largeur_pix]);
        }
        printf("\n");        
    }
}


/* Affiche une MCU transformée */
void print_MCU_zigzag_Y(struct MCU_zigzag_Y *MCU_zigzag)
{
    // Print des blocs Y
    for (uint8_t hauteur_Y = 0; hauteur_Y < MCU_zigzag->h1; hauteur_Y++) {
        for (uint8_t largeur_Y = 0; largeur_Y < MCU_zigzag->v1; largeur_Y++){
            printf("Bloc Y zigzag numéro: %d\n", hauteur_Y * MCU_zigzag->v1 + largeur_Y);
            print_bloc_zigzag(MCU_zigzag->blocs_Y_zigzag[hauteur_Y][largeur_Y]);
            printf("\n");
        }
    }
    printf("\n");

    // Print des blocs Cb
    for (uint8_t hauteur_Cb = 0; hauteur_Cb < MCU_zigzag->h2; hauteur_Cb++) {
        for (uint8_t largeur_Cb = 0; largeur_Cb < MCU_zigzag->v2; largeur_Cb++){
            printf("Bloc Cb zigzag numéro: %d\n", hauteur_Cb * MCU_zigzag->v2 + largeur_Cb);
            print_bloc_zigzag(MCU_zigzag->blocs_Cb_zigzag[hauteur_Cb][largeur_Cb]);
            printf("\n");
        }
    }
    printf("\n");

    // Print des blocs Cr
    for (uint8_t hauteur_Cr = 0; hauteur_Cr < MCU_zigzag->h3; hauteur_Cr++) {
        for (uint8_t largeur_Cr = 0; largeur_Cr < MCU_zigzag->v3; largeur_Cr++){
            printf("Bloc Cr zigzag numéro: %d\n", hauteur_Cr * MCU_zigzag->v3 + largeur_Cr);
            print_bloc_zigzag(MCU_zigzag->blocs_Cr_zigzag[hauteur_Cr][largeur_Cr]);
            printf("\n");
        }
    }}



/* Affiche les MCUs transformées */
void print_MCUs_zigzag_Y(struct MCU_zigzag_Y ***MCUs_zigzag, uint32_t nb_MCUs_largeur, uint32_t nb_MCUs_hauteur)
{
    for (uint32_t hauteur = 0; hauteur < nb_MCUs_hauteur; hauteur++){
        for (uint32_t largeur =  0; largeur < nb_MCUs_largeur; largeur++){
            printf("MCU zigzag %d:\n", hauteur * nb_MCUs_largeur + largeur);
            print_MCU_zigzag_Y(MCUs_zigzag[hauteur][largeur]);
            printf("\n");
        }
    }
}


/* Libère la mémoire allouée à une MCU fréquentielle */
void free_MCU_zigzag_Y(struct MCU_zigzag_Y *MCU_zigzag)
{
    // free des blocs Y
    for (uint8_t hauteur_Y = 0; hauteur_Y < MCU_zigzag->h1; hauteur_Y++) {
        for (uint8_t largeur_Y = 0; largeur_Y < MCU_zigzag->v1; largeur_Y++){
            free(MCU_zigzag->blocs_Y_zigzag[hauteur_Y][largeur_Y].pixels);
        }
        free(MCU_zigzag->blocs_Y_zigzag[hauteur_Y]);
    }
    free(MCU_zigzag->blocs_Y_zigzag);

    // free des blocs Cb
    for (uint8_t hauteur_Cb = 0; hauteur_Cb < MCU_zigzag->h2; hauteur_Cb++) {
        for (uint8_t largeur_Cb = 0; largeur_Cb < MCU_zigzag->v2; largeur_Cb++){
            free(MCU_zigzag->blocs_Cb_zigzag[hauteur_Cb][largeur_Cb].pixels);
        }
        free(MCU_zigzag->blocs_Cb_zigzag[hauteur_Cb]);
    }
    free(MCU_zigzag->blocs_Cb_zigzag);

    // free des blocs Cr
    for (uint8_t hauteur_Cr = 0; hauteur_Cr < MCU_zigzag->h3; hauteur_Cr++) {
        for (uint8_t largeur_Cr = 0; largeur_Cr < MCU_zigzag->v3; largeur_Cr++){
            free(MCU_zigzag->blocs_Cr_zigzag[hauteur_Cr][largeur_Cr].pixels);
        }
        free(MCU_zigzag->blocs_Cr_zigzag[hauteur_Cr]);
    }
    free(MCU_zigzag->blocs_Cr_zigzag);

    free(MCU_zigzag);
}



/* Libère la mémoire allouée aux MCUs fréquentielles */
void free_MCUs_zigzag_Y(struct MCU_zigzag_Y ***MCUs_zigzag, uint32_t nb_MCUs_largeur, uint32_t nb_MCUs_hauteur)
{
    for (int32_t hauteur = nb_MCUs_hauteur - 1; hauteur >= 0; hauteur--){
        for (int32_t largeur = nb_MCUs_largeur - 1; largeur >= 0; largeur--){
            free_MCU_zigzag_Y(MCUs_zigzag[hauteur][largeur]);
        }
        free(MCUs_zigzag[hauteur]);
    }
    free(MCUs_zigzag);
}


/************************************************************/
/* Fin de la partie consacrée à la réorganisation en zigzag */
/************************************************************/


/****************************************/
/* Partie consacrée à la quantification */
/****************************************/

/* Applique la quantification à une MCU */
void quantification_bloc(struct Bloc_zigzag *Bloc_zigzag)
{
    for (uint8_t indice = 0; indice < 64; indice++){
        Bloc_zigzag->pixels[indice] = Bloc_zigzag->pixels[indice]/quantification_table_Y[indice];
    }
}


void quantification_MCU_couleur(struct MCU_zigzag_Y *MCU_zigzag)
{
    // Quantification des blocs Y
    for (uint8_t hauteur_Y = 0; hauteur_Y < MCU_zigzag->h1; hauteur_Y++) {
        for (uint8_t largeur_Y = 0; largeur_Y < MCU_zigzag->v1; largeur_Y++){
            quantification_bloc(&(MCU_zigzag->blocs_Y_zigzag[hauteur_Y][largeur_Y]));
        }
    }

    // Quantification des blocs Cb
    for (uint8_t hauteur_Cb = 0; hauteur_Cb < MCU_zigzag->h2; hauteur_Cb++) {
        for (uint8_t largeur_Cb = 0; largeur_Cb < MCU_zigzag->v2; largeur_Cb++){
            quantification_bloc(&(MCU_zigzag->blocs_Cb_zigzag[hauteur_Cb][largeur_Cb]));
        }
    }

    // Quantification des blocs Cr
    for (uint8_t hauteur_Cr = 0; hauteur_Cr < MCU_zigzag->h3; hauteur_Cr++) {
        for (uint8_t largeur_Cr = 0; largeur_Cr < MCU_zigzag->v3; largeur_Cr++){
            quantification_bloc(&(MCU_zigzag->blocs_Cr_zigzag[hauteur_Cr][largeur_Cr]));
        }
    }
}


/* Applique la quantification aux MCUs */
void quantification_couleur(struct MCU_zigzag_Y ***MCUs_zigzag, uint32_t largeur_MCUs, uint32_t hauteur_MCUs)
{
    for (uint32_t hauteur = 0; hauteur < hauteur_MCUs; hauteur++){
        for (uint32_t largeur = 0; largeur < largeur_MCUs; largeur++){
            quantification_MCU_couleur(MCUs_zigzag[hauteur][largeur]);
        }
    }
}


/**************************************************/
/* Fin de la partie consacrée à la quantification */
/**************************************************/




int main()
{
    struct MCU_YCbCr *MCU_YCbCr = malloc(sizeof(struct MCU_YCbCr));
    MCU_YCbCr->h1 = 1, MCU_YCbCr->h2 = 1, MCU_YCbCr->h3 = 1;
    MCU_YCbCr->v1 = 2, MCU_YCbCr->v2 = 1, MCU_YCbCr->v3 = 1;

    int16_t mat_Y_g[8][8] = {   {0xa6, 0xa0, 0x9a, 0x98, 0x9a, 0x9a, 0x96, 0x91},
                                {0xa0, 0xa3, 0x9d, 0x8e, 0x88, 0x8f, 0x95, 0x94},
                                {0xa5, 0x97, 0x96, 0xa1, 0x9f, 0x90, 0x90, 0x9e},
                                {0xa6, 0x9a, 0x91, 0x91, 0x92, 0x90, 0x90, 0x93},
                                {0xc9, 0xd9, 0xc8, 0x98, 0x85, 0x98, 0xa2, 0x95},
                                {0xf0, 0xf5, 0xf9, 0xea, 0xbf, 0x98, 0x90, 0x9d},
                                {0xe9, 0xe1, 0xf3, 0xfd, 0xf2, 0xaf, 0x8a, 0x90},
                                {0xe6, 0xf2, 0xf1, 0xed, 0xf8, 0xfb, 0xd0, 0x95}};

    int16_t mat_Y_d [8][8] = {  {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
                                {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
                                {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa6, 0xa7},
                                {0x9e, 0xa1, 0xa4, 0xa5, 0xa4, 0xa4, 0xa5, 0xa7},
                                {0x9d, 0xa0, 0xa3, 0xa4, 0xa3, 0xa3, 0xa4, 0xa6},
                                {0x9b, 0x9e, 0xa1, 0xa2, 0xa1, 0xa1, 0xa3, 0xa4},
                                {0x99, 0x9c, 0x9f, 0xa0, 0x9f, 0x9f, 0xa1, 0xa2},
                                {0x98, 0x9b, 0x9e, 0x9f, 0x9e, 0x9e, 0xa0, 0xa1}};

    int16_t mat_Cb[8][8] = {    {0x75, 0x75, 0x76, 0x77, 0x78, 0x7a, 0x7b, 0x7b},
                                {0x75, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b},
                                {0x75, 0x76, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7a},
                                {0x76, 0x76, 0x77, 0x77, 0x78, 0x79, 0x7a, 0x7a},
                                {0x76, 0x77, 0x77, 0x78, 0x78, 0x79, 0x79, 0x79},
                                {0x77, 0x77, 0x77, 0x78, 0x78, 0x78, 0x79, 0x79},
                                {0x77, 0x77, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78},
                                {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78}};


    int16_t mat_Cr [8][8] = {   {0x8c, 0x88, 0x85, 0x87, 0x8c, 0x8e, 0x8b, 0x87},
                                {0x8c, 0x88, 0x85, 0x87, 0x8c, 0x8e, 0x8b, 0x87},
                                {0x8b, 0x88, 0x85, 0x86, 0x8b, 0x8e, 0x8b, 0x87},
                                {0x8b, 0x87, 0x85, 0x86, 0x8b, 0x8d, 0x8b, 0x86},
                                {0x8b, 0x87, 0x84, 0x86, 0x8b, 0x8d, 0x8a, 0x85},
                                {0x89, 0x85, 0x84, 0x85, 0x8a, 0x8d, 0x8a, 0x85},
                                {0x8a, 0x84, 0x84, 0x85, 0x8a, 0x8c, 0x8a, 0x86},
                                {0x8a, 0x86, 0x83, 0x85, 0x8a, 0x8c, 0x8a, 0x86}};

    int16_t **pixels_Y_g = malloc(8 * sizeof(int16_t *));
    int16_t **pixels_Y_d = malloc(8 * sizeof(int16_t *));
    int16_t **pixels_Cb = malloc(8 * sizeof(int16_t *));
    int16_t **pixels_Cr = malloc(8 * sizeof(int16_t *));

    for (uint8_t i = 0; i < 8; i++){
        int16_t *ligne_pixels_Y_g = malloc(8 * sizeof(int16_t));
        int16_t *ligne_pixels_Y_d = malloc(8 * sizeof(int16_t));
        int16_t *ligne_pixels_Cb = malloc(8 * sizeof(int16_t));
        int16_t *ligne_pixels_Cr = malloc(8 * sizeof(int16_t));
        for (uint8_t j = 0; j < 8; j++){
            ligne_pixels_Y_g[j] = mat_Y_g[i][j];
            ligne_pixels_Y_d[j] = mat_Y_d[i][j];
            ligne_pixels_Cb[j] = mat_Cb[i][j];
            ligne_pixels_Cr[j] = mat_Cr[i][j];
        }
        pixels_Y_g[i] = ligne_pixels_Y_g;
        pixels_Y_d[i] = ligne_pixels_Y_d;
        pixels_Cb[i] = ligne_pixels_Cb;
        pixels_Cr[i] = ligne_pixels_Cr;
    }

    struct Bloc_YCbCr matrice_Y_g;
    struct Bloc_YCbCr matrice_Y_d;
    struct Bloc_YCbCr matrice_Cb;
    struct Bloc_YCbCr matrice_Cr;

    matrice_Y_g.pixels = pixels_Y_g;
    matrice_Y_d.pixels = pixels_Y_d;
    matrice_Cb.pixels = pixels_Cb;
    matrice_Cr.pixels = pixels_Cr;

    struct Bloc_YCbCr **Blocs_Y = malloc(MCU_YCbCr->h1 * sizeof(struct Bloc_YCbCr *));
    struct Bloc_YCbCr **Blocs_Cb = malloc(MCU_YCbCr->h2 * sizeof(struct Bloc_YCbCr *));
    struct Bloc_YCbCr **Blocs_Cr = malloc(MCU_YCbCr->h3 * sizeof(struct Bloc_YCbCr *));

    struct Bloc_YCbCr *ligne_blocs_Y = malloc(MCU_YCbCr->v1 * sizeof(struct Bloc_YCbCr));
    struct Bloc_YCbCr *ligne_blocs_Cb = malloc(MCU_YCbCr->v2 * sizeof(struct Bloc_YCbCr));
    struct Bloc_YCbCr *ligne_blocs_Cr = malloc(MCU_YCbCr->v3 * sizeof(struct Bloc_YCbCr));

    ligne_blocs_Y[0] = matrice_Y_g;
    ligne_blocs_Y[1] = matrice_Y_d;
    ligne_blocs_Cb[0] = matrice_Cb;
    ligne_blocs_Cr[0] = matrice_Cr;

    Blocs_Y[0] = ligne_blocs_Y;
    Blocs_Cb[0] = ligne_blocs_Cb;
    Blocs_Cr[0] = ligne_blocs_Cr;

    MCU_YCbCr->blocs_Y = Blocs_Y;
    MCU_YCbCr->blocs_Cb = Blocs_Cb;
    MCU_YCbCr->blocs_Cr = Blocs_Cr;

    struct MCU_freq_Y *MCU_freq = transf_cos_MCU_YCbCr(MCU_YCbCr);
    print_MCU_freq_Y(MCU_freq);
    struct MCU_zigzag_Y *MCU_zigzag = zigzag_MCU_Y(MCU_freq);
    print_MCU_zigzag_Y(MCU_zigzag);
    quantification_MCU_couleur(MCU_zigzag);
    print_MCU_zigzag_Y(MCU_zigzag);

    free(ligne_blocs_Y);
    free(ligne_blocs_Cb);
    free(ligne_blocs_Cr);

    free(Blocs_Y);
    free(Blocs_Cb);
    free(Blocs_Cr);

    free_pixel(pixels_Cb, 8);
    free_pixel(pixels_Y_d, 8);
    free_pixel(pixels_Y_g, 8);
    free_pixel(pixels_Cr, 8);

    free(MCU_YCbCr);
    free_MCU_freq_Y(MCU_freq);
    free_MCU_zigzag_Y(MCU_zigzag);

    return 0;
}