/******************************************************************************
 *
 * Copyright(c) 2022 Lian Ferrand
 * This file is part of GCTlib
 *
 * File description:
 *  Convert a gct file to raw 32-bit image data using GCTlib
 *  NOTE: No error checking is done on CRT functions for brevity,
 *        this program may crash!
 *
 ******************************************************************************/

#include "gct/gctlib.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>

// Function prototypes
static void *GetGCTFile(int argc, char **argv);
static void WriteFile(int argc, char **argv, void *imageData, gct_iptr dataSize);

int main(int argc, char **argv) {
  void *gctFile = GetGCTFile(argc, argv);

  // Get decoded image data size
  gct_iptr dataSize = gct_DecodedSize(gctFile);
  if (dataSize < 0) {
    printf("ERROR: Cannot get decoded image data size! (%s)\n", gct_StrError(dataSize));
    free(gctFile);
    return 1;
  }

  gct_color_t *imageData = (gct_color_t*)malloc(dataSize);

  // Decode GCT file into imageData
  int width, height;
  gct_error_t err = gct_Decode(gctFile, &width, &height, imageData);
  if (err != gct_SUCCESS) {
    printf("ERROR: Cannot decode GCT image data! (%s)\n", gct_StrError(err));
    free(gctFile);
    free(imageData);
    return 1;
  }

  WriteFile(argc, argv, imageData, dataSize);

  printf("Image size: %d %d\n", width, height);

  // Free resources and exit
  free(gctFile);
  free(imageData);

  return 0;
}

// Arguments slots
#define ARG_INPUT 1
#define ARG_OUTPUT 2

// Open GCT file, and return file data
static void *GetGCTFile(int argc, char **argv) {
  const char *inputName;
  if (argc < 2) inputName = "sampleImage.gct";
  else inputName = argv[ARG_INPUT];

  FILE *f = fopen(inputName, "rb");

  // Get file size
  fseek(f, 0, SEEK_END);
  const size_t retSize = ftell(f);
  fseek(f, 0, SEEK_SET);

  void *ret = malloc(retSize);
  fread(ret, 1, retSize, f);
  fclose(f);

  return ret;
}

// Write raw data to file
static void WriteFile(int argc, char **argv, void *imageData, gct_iptr dataSize) {
  const char *outputName;
  if (argc < 3) outputName = "sampleImage.data";
  else outputName = argv[ARG_OUTPUT];

  FILE *f = fopen(outputName, "wb");
  fwrite(imageData, 1, dataSize, f);
  fclose(f);
}
