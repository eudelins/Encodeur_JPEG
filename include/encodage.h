#ifndef ENCODAGE_H
#define ENCODAGE_H

#include <stdint.h>


/******************************/
/* Module d'encodage des MCUs */
/******************************/


/************************************************************/
/* Partie consacrée à la transformation en cosinus discrète */
/************************************************************/


/* MCU après transformée en cosinus discrète */
struct MCU_freq{
    uint8_t largeur;
    uint8_t hauteur;
    int16_t **pixels_freq;
};


/* Calcul C(i) */
double C(uint8_t i);


/* Applique la transformée en cosinus discrète à une MCU */
struct MCU_freq *transf_cos_MCU(struct MCU *MCU);


/* Applique la transformée en cosinus discrète aux MCUs */
struct MCU_freq ***transf_cos(struct MCU ***MCUs, uint32_t largeur_MCUs, uint32_t hauteur_MCUs);


/* Affiche une MCU transformée */
void print_MCU_freq(struct MCU_freq *MCU_freq);


/* Affiche les MCUs transformées */
void print_MCUs_freq(struct MCU_freq ***MCUs_freq, uint32_t largeur_MCUs, uint32_t hauteur_MCUs);


/* Libère la mémoire alloué pour les pixels d'une MCU fréquentielle */
void free_pixel_freq(int16_t **pixels, uint32_t hauteur_pix);


/* Libère la mémoire allouée aux MCUs fréquentielles */
void free_MCUs_freq(struct MCU_freq ***MCUs_freq, uint32_t largeur_MCUs, uint32_t hauteur_MCUs);


/**********************************************************************/
/* Fin de la partie consacrée à la transformation en cosinus discrète */
/**********************************************************************/


/**************************************************/
/* Partie consacrée à la réorganisation en zigzag */
/**************************************************/


/* MCU après réorganisation en zigzag */
struct MCU_zigzag{
    uint8_t largeur;
    uint8_t hauteur;
    int16_t *pixels_zigzag;
};


/* Calcul le chemin en zigzag d'une MCU */
struct MCU_zigzag *zigzag_MCU(struct MCU_freq *MCU_freq);


/* Applique la réorganisation en zigzag aux MCUs */
struct MCU_zigzag ***zigzag(struct MCU_freq ***MCUs_freq, uint32_t largeur_MCUs, uint32_t hauteur_MCUs);


/* Affiche une MCU réorgansée en zigzag */
void print_MCU_zigzag(struct MCU_zigzag *MCU_zigzag);


/* Affiche les MCUs réorgansées en zigzag */
void print_MCUs_zigzag(struct MCU_zigzag ***MCUs_zigzag, uint32_t largeur_MCUs, uint32_t hauteur_MCUs);


/* Libère la mémoire allouée aux MCUs en zigzag */
void free_MCUs_zigzag(struct MCU_zigzag ***MCUs_zigzag, uint32_t largeur_MCUs, uint32_t hauteur_MCUs);


/************************************************************/
/* Fin de la partie consacrée à la réorganisation en zigzag */
/************************************************************/


/****************************************/
/* Partie consacrée à la quantification */
/****************************************/


/* Applique la quantification à une MCU */
void quantification_MCU(struct MCU_zigzag *MCU_zigzag);


/* Applique la quantification aux MCUs */
void quantification(struct MCU_zigzag ***MCUs_zigzag, uint32_t largeur_MCUs, uint32_t hauteur_MCUs);


/**************************************************/
/* Fin de la partie consacrée à la quantification */
/**************************************************/


#endif /* ENCODAGE_H */