/******************************************************************************
 *
 * Copyright(c) 2022 Lian Ferrand
 * This file is part of GCTlib
 *
 * File description:
 *  Convert raw 32-bit image data to a gct using GCTlib
 *  NOTE: No error checking is done on CRT functions for brevity,
 *        this program may crash!
 *
 ******************************************************************************/

#include "gct/gctlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256

/* Function prototypes */
static void MakeImageData(gct_color_t *pixels);
static const char *GetOutputFilename(int argc, char **argv);

int main(int argc, char **argv) {
  const char *out;
  FILE *f;
  gct_color_t *imageData;
  void *compressedData;
  gct_error_t err;
  gct_header_t hdr;
  gct_iptr compressedSize;

  out = GetOutputFilename(argc, argv);
  f = fopen(out, "wb");

  /* Allocate & initialize raw image data */
  imageData = (gct_color_t*)malloc(4 * IMAGE_WIDTH*IMAGE_HEIGHT);
  MakeImageData(imageData);

  /* Initialize header from image size and desired
   * output format */
  err = gct_InitHeader(&hdr, IMAGE_WIDTH, IMAGE_HEIGHT, gct_HDR_TRANSP_FLAGS);
  if (err != gct_SUCCESS) {
    printf("ERROR: Cannot initialize image header! (%s)\n", gct_StrError(err));
    fclose(f);
    free(imageData);
    return EXIT_FAILURE;
  }

  /* Allocate compressed image data */
  compressedSize = gct_EncodedSize(&hdr);
  if (compressedSize < 0) {
    printf("ERROR: Cannot get encoded image data size! (%s)\n", gct_StrError(compressedSize));
    fclose(f);
    free(imageData);
    return EXIT_FAILURE;
  }

  compressedData = malloc(compressedSize);

  /* Encode raw 32-bit image data into GCT image data */
  err = gct_Encode(&hdr, imageData, compressedData);
  if (err != gct_SUCCESS) {
    printf("ERROR: Unable to encode image data! (%s)\n", gct_StrError(err));
    free(imageData);
    free(compressedData);
    fclose(f);
    return EXIT_FAILURE;
  }

  /* Write header, then compressed data, to file */
  fwrite(&hdr, 1, sizeof(hdr), f);
  fwrite(compressedData, 1, compressedSize, f);

  /* Free resources and exit */
  fclose(f);
  free(imageData);
  free(compressedData);

  return EXIT_SUCCESS;
}

static void MakeImageData(gct_color_t *pixels) {
  /* Make a xor pattern out of the pixels */
  int x, y;

  for (y = 0; y < IMAGE_HEIGHT; ++y) {
    for (x = 0; x < IMAGE_WIDTH; ++x) {
      gct_color_t *p = &pixels[y*IMAGE_WIDTH + x];

      p->r = p->g = p->b = x^y;
      p->a = x+y;
    }
  }
}

static const char *GetOutputFilename(int argc, char **argv) {
  if (argc < 2) return "sampleImage.gct";
  return argv[1];
}
