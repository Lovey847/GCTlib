/******************************************************************************
 *
 * Copyright(c) 2022 Lian Ferrand
 * This file is part of GCTLib
 *
 * File description:
 *  GCTLib main header file, include this to use GCTLib in your C project
 *
 ******************************************************************************/

#ifndef _GCT_GCTLIB_H
#define _GCT_GCTLIB_H

#include "gct/types.h"

typedef struct gct_color_s {
  gct_u8 r, g, b, a;
} gct_color_t;

/* GCT texture flags, barely know what they mean
 *
 * Either 16 or 17 means the texture is mipmapped,
 * I think one of them means it's flipped */
#define gct_HDR_UNK00 0x00000001
#define gct_HDR_UNK01 0x00000002
#define gct_HDR_UNK02 0x00000004
#define gct_HDR_ALPHA 0x00000008 /* The texture has an alpha channel */
#define gct_HDR_UNK04 0x00000010
#define gct_HDR_UNK05 0x00000020
#define gct_HDR_UNK06 0x00000040
#define gct_HDR_UNK07 0x00000080
#define gct_HDR_UNK08 0x00000100
#define gct_HDR_UNK09 0x00000200
#define gct_HDR_UNK10 0x00000400
#define gct_HDR_UNK11 0x00000800
#define gct_HDR_UNK12 0x00001000
#define gct_HDR_UNK13 0x00002000
#define gct_HDR_UNK14 0x00004000
#define gct_HDR_UNK15 0x00008000
#define gct_HDR_UNK16 0x00010000
#define gct_HDR_UNK17 0x00020000
#define gct_HDR_UNK18 0x00040000
#define gct_HDR_UNK19 0x00080000
#define gct_HDR_UNK20 0x00100000
#define gct_HDR_UNK21 0x00200000
#define gct_HDR_UNK22 0x00400000
#define gct_HDR_UNK23 0x00800000
#define gct_HDR_UNK24 0x01000000
#define gct_HDR_UNK25 0x02000000
#define gct_HDR_UNK26 0x04000000
#define gct_HDR_UNK27 0x08000000
#define gct_HDR_UNK28 0x10000000
#define gct_HDR_UNK29 0x20000000
#define gct_HDR_UNK30 0x40000000
#define gct_HDR_UNK31 0x80000000

/* Header to GCT texture file
 *
 * NOTE: I don't know a lot about this header,
 * so this is probably inaccurate and unfinished */
typedef struct gct_header_s {
  gct_be32_t width, height;

  /* Don't know why width and height are specified twice */
  gct_be32_t width2, height2;

  /* Header flags, don't know what any of them mean */
  gct_be32_t flags;

  /* This seems to be 0 when the image is upright,
   * and -1 when the image is vertically flipped.
   * I may be wrong about this */
  gct_be32_t orientation;

  /* Image data starts at 0x20 */
  gct_u32 pad[2];
} gct_header_t;

/* GCTlib error codes
 *
 * Typedef the actual type as an int since
 * C++ won't let us treat the enum as an integer
 * unless we specify a ton of operators */
enum gct_error_e {
  /* Function returned successfully */
  gct_SUCCESS = 0,

  /* Invalid width or height
   *
   * A width/height is invalid when:
   *  it's <= 0
   *  it's not a mutliple of 8 */
  gct_ERR_INVALID_SIZE,

  /* Invalid NULL data pointer */
  gct_ERR_NULL_POINTER,

  gct_NUM_ERR_CODES
};
typedef int gct_error_t;

/* Convert GCTlib error code to string describing the error
 *
 * err: Error code to be converted into a string, can be negative
 *
 * Return value:
 *  Pointer to string corresponding to error code if err is valid
 *  NULL if err is invalid */
const char *gct_StrError(gct_error_t err);

/* Get size of CMPR data, based on image size
 *
 * width: Image width
 * height: Image height
 *
 * Return value:
 *  Size of encoded data on success
 *  -gct_ERR_INVALID_SIZE if width or height is invalid */
gct_iptr gct_EncodedSize(int width, int height);

/* Initialize gct header from image size
 *
 * hdr: Output pointer to header
 * width: Image width
 * height: Image height
 * flipped: Whether the image is vertically flipped or not
 *
 * Return value:
 *  gct_SUCCESS if header was successfully initialized
 *  gct_ERR_INVALID_SIZE if width or height are invalid
 *  gct_ERR_NULL_POINTER if hdr is NULL */
gct_error_t gct_InitHeader(gct_header_t *hdr, int width, int height);

/* Encode raw image data to CMPR
 *
 * width: Image width
 * height: Image height
 * input: Raw RGBA input data (size in bytes = width * height * 4)
 * output: CMPR output (size in bytes = gct_EncodedSize(width, height))
 *
 * Return value:
 *  gct_SUCCESS if input was successfully encoded to output
 *  gct_ERR_INVALID_SIZE if width or height is invalid
 *  gct_ERR_NULL_POINTER if input or output are NULL
 *    translucent == gct_false */
gct_error_t gct_Encode(int width, int height,
                       const gct_color_t *input, void *output);

#endif /*_GCT_GCTLIB_H*/
