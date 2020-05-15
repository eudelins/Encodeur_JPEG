#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

/* Défini un pixel RGB */
typedef struct Pixel_RGB Pixel_RGB;
struct Pixel_RGB {
  uint8_t R;
  uint8_t G;
  uint8_t B;
};



/* Défini un pixel YCbCr */
typedef struct Pixel_YCbCr Pixel_YCbCr;
struct Pixel_YCbCr {
  uint8_t Y;
  uint8_t Cb;
  uint8_t Cr;
};

/* Converti un pixel RGB en pixel YCbCr, en renvoie un pointeur*/
Pixel_YCbCr *conversion(Pixel_RGB *pixelRGB)
{
  Pixel_YCbCr *pixelYCbCr = malloc(sizeof(Pixel_YCbCr));
  pixelYCbCr->Y = 0.299 * pixelRGB->R + 0.587 * pixelRGB->G + 0.114 * pixelRGB->B;
  pixelYCbCr->Cb = 128 - 0.1687 * pixelRGB->R - 0.3313 * pixelRGB->G + 0.5 * pixelRGB->B;
  pixelYCbCr->Cr = 128 + 0.5 * pixelRGB->R - 0.4187 * pixelRGB->G - 0.0813 * pixelRGB->B;
  return pixelYCbCr;
};

int main(void)
{
  Pixel_RGB pixelRGB = {255, 255, 255};
  Pixel_RGB *pixel = &pixelRGB;
  printf("La valeur du pixel RGB est : %u, %u, %u \n", pixel->R, pixel->G, pixel->B);
  Pixel_YCbCr *converti = conversion(pixel);
  printf("La valeur du pixel YCbCr est : %u, %u, %u \n", converti->Y, converti->Cb, converti->Cr);
  free(converti);
  return 0;
}
