/******************************************************************************
 *
 * Copyright(c) 2022 Lian Ferrand
 * This file is part of GCTlib
 *
 * File description:
 *  Convert a raw image data file to a gct file using GCTlib
 *  NOTE: No error checking is done on CRT functions for brevity,
 *        this program may crash!
 *
 ******************************************************************************/

#include "gct/gctlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

/* Function prototypes */
static gct_color_t *GetImageFile(int argc, char **argv, int *width, int *height);
static const char *GetOutputFilename(int argc, char **argv);

int main(int argc, char **argv) {
  const char *out;
  FILE *f;
  gct_color_t *imageData;
  int imageWidth, imageHeight;
  void *compressedData;
  gct_error_t err;
  gct_header_t hdr;
  gct_iptr compressedSize;

  out = GetOutputFilename(argc, argv);
  f = fopen(out, "wb");

  imageData = GetImageFile(argc, argv, &imageWidth, &imageHeight);

  /* Initialize header from image size and desired
   * output format */
  err = gct_InitHeader(&hdr, imageWidth, imageHeight, gct_HDR_TRANSP_FLAGS);
  if (err != gct_SUCCESS) {
    printf("ERROR: Cannot initialize image header! (%s)\n", gct_StrError(err));
    fclose(f);
    free(imageData);
    return EXIT_FAILURE;
  }

  /* Allocate compressed image data */
  compressedSize = gct_EncodedSize(&hdr);
  if (compressedSize < 0) {
    printf("ERROR: Cannot get gct image data size! (%s)\n", gct_StrError(compressedSize));
    fclose(f);
    free(imageData);
    return EXIT_FAILURE;
  }

  compressedData = malloc(compressedSize);

  /* Encode raw 32-bit image data into GCT image data */
  err = gct_Encode(&hdr, imageData, compressedData);
  if (err != gct_SUCCESS) {
    printf("ERROR: Unable to encode image data! (%s)\n", gct_StrError(err));
    fclose(f);
    free(imageData);
    free(compressedData);
    return EXIT_FAILURE;
  }

  /* Write header, then compressed data to file */
  fwrite(&hdr, 1, sizeof(hdr), f);
  fwrite(compressedData, 1, compressedSize, f);

  /* Free resources and exit */
  fclose(f);
  free(imageData);
  free(compressedData);

  return 0;
}

#define ARG_WIDTH 1
#define ARG_HEIGHT 2
#define ARG_INPUT 3
#define ARG_OUTPUT 4

static gct_color_t *GetImageFile(int argc, char **argv, int *width, int *height) {
  gct_color_t *ret;
  if (argc < 4) {
    size_t i;

    /* Initialize to white 64x32 image */
    *width = 64;
    *height = 32;

    ret = (gct_color_t*)malloc(sizeof(gct_color_t*) * 64*32);

    for (i = 0; i < 64*32; ++i)
      ret[i].r = ret[i].g = ret[i].b = ret[i].a = 255;
  } else {
    FILE *f;

    f = fopen(argv[ARG_INPUT], "rb");
    *width = atoi(argv[ARG_WIDTH]);
    *height = atoi(argv[ARG_HEIGHT]);

    ret = (gct_color_t*)malloc(sizeof(gct_color_t) * *width * *height);
    fread(ret, 1, sizeof(gct_color_t) * *width * *height, f);
    fclose(f);
  }

  return ret;
}

static const char *GetOutputFilename(int argc, char **argv) {
  if (argc < 5) return "sampleImage.gct";
  return argv[ARG_OUTPUT];
}
