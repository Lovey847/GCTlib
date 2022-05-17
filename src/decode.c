/******************************************************************************
 *
 * Copyright(c) 2022 Lian Ferrand
 * This file is part of GCTLib
 *
 * File description:
 *  GCT decoder
 *  NOTE: This decodes a very specific subset of GCT, which is
 *  a normal image plane, then an alpha plane. Right now this cannot
 *  decode other types of GCTs.
 *
 ******************************************************************************/

#include "gct/gctlib.h"

// 16-bit color type
typedef union color16_s {
  struct {
    gct_u16 b : 5;
    gct_u16 g : 6;
    gct_u16 r : 5;
  } c;

  gct_u16 p;
} color16_t;
typedef gct_be16_t color16_be_t;

// DXT1 block to decode
typedef struct block_s {
  color16_be_t col0, col1;
  gct_be32_t pixelTable;
} block_t;

// Alpha component
typedef gct_u8 gct_alpha_t;

static gct_b32 ValidImageSize(gct_i32 width, gct_i32 height) {
  return ((width > 7) && (height > 7) &&
          !(width & 7) && !(height & 7));
}

static gct_b32 SupportedImageFlags(gct_hdr_flags_t flags) {
  return flags == gct_HDR_TRANSP_FLAGS;
}

gct_iptr gct_DecodedSize(const void *file) {
  gct_i32 width, height;
  const gct_header_t * const hdr = (gct_header_t*)file;

  if (!file) return -gct_ERR_NULL_POINTER;

  width = gct_SIGNED_BIG32(hdr->width);
  height = gct_SIGNED_BIG32(hdr->height);

  if ((width != gct_SIGNED_BIG32(hdr->width2)) ||
      (height != gct_SIGNED_BIG32(hdr->height2)) ||
      !ValidImageSize(width, height))
    return -gct_ERR_INVALID_IMAGE;
  else if (!SupportedImageFlags(gct_BIG32(hdr->flags)))
    return -gct_ERR_UNSUPPORTED_IMAGE;

  // Size of raw image data
  return width*height * sizeof(gct_color_t);
}

// Convert color16_t into gct_color_t
static void Color16To32(color16_t c16, gct_color_t *c32) {
  // Round colors
  c32->r = (c16.c.r << 3) | (c16.c.r >> 2);
  c32->g = (c16.c.g << 2) | (c16.c.g >> 4);
  c32->b = (c16.c.b << 3) | (c16.c.b >> 2);
}

// Convert alpha color16_t into gct_color_t
static void Alpha16To32(color16_t a16, gct_alpha_t *out) {
  // Round alpha
  *out = (a16.c.g << 2) | (a16.c.g >> 4);
}

// Lerp 1 third of color a, and 2 thirds of color b
static void Lerp13(const gct_color_t *a, const gct_color_t *b, gct_color_t *out) {
  out->r = ((gct_u32)a->r + (gct_u32)b->r*2) / 3;
  out->g = ((gct_u32)a->g + (gct_u32)b->g*2) / 3;
  out->b = ((gct_u32)a->b + (gct_u32)b->b*2) / 3;
}

// Extract info from file block
static void ExtractBlock(const block_t *blk, const block_t *ablk,
                         gct_u32 *pixelTable, gct_u32 *aPixelTable,
                         gct_color_t *pal, gct_alpha_t *apal)
{
  color16_t col0, col1, acol0, acol1;

  col0.p = gct_BIG16(blk->col0);
  col1.p = gct_BIG16(blk->col1);
  acol0.p = gct_BIG16(ablk->col0);
  acol1.p = gct_BIG16(ablk->col1);

  Color16To32(col0, pal);
  Color16To32(col1, pal+1);
  Lerp13(pal+1, pal, pal+2);
  Lerp13(pal, pal+1, pal+3);

  Alpha16To32(acol0, apal);
  Alpha16To32(acol1, apal+1);

  // Lerp alpha values
  apal[2] = (apal[0]*2 + apal[1]) / 3;
  apal[3] = (apal[0] + apal[1]*2) / 3;

  *pixelTable = gct_BIG32(blk->pixelTable);
  *aPixelTable = gct_BIG32(ablk->pixelTable);
}

// Decode DXT1 block into image data
static void DecodeDXT1(const block_t *blk, const block_t *ablk,
                       gct_iptr stride, gct_color_t *out)
{
  gct_color_t pal[4];
  gct_alpha_t apal[4];
  gct_u32 pixelTable, aPixelTable;
  gct_u32 i;

  stride -= 4;

  ExtractBlock(blk, ablk, &pixelTable, &aPixelTable, pal, apal);

  // Write colors into image based on pixel table
  for (i = 0; i < 16; ++i, ++out, pixelTable <<= 2, aPixelTable <<= 2) {
    *out = pal[pixelTable >> 30];
    out->a = apal[aPixelTable >> 30];

    if ((i&3) == 3) out += stride;
  }
}

gct_error_t gct_Decode(const void *file, int *width,
                       int *height, gct_color_t *output)
{
  gct_i32 w, h;
  const gct_header_t * const hdr = (gct_header_t*)file;
  gct_iptr x;
  gct_iptr y;
  const block_t *blk, *ablk;

  if (!file || !width || !height || !output)
    return gct_ERR_NULL_POINTER;

  w = gct_SIGNED_BIG32(hdr->width);
  h = gct_SIGNED_BIG32(hdr->height);

  if ((w != gct_SIGNED_BIG32(hdr->width2)) ||
      (h != gct_SIGNED_BIG32(hdr->height2)) ||
      !ValidImageSize(w, h))
    return gct_ERR_INVALID_IMAGE;
  else if (!SupportedImageFlags(gct_BIG32(hdr->flags)))
    return gct_ERR_UNSUPPORTED_IMAGE;

  *width = w;
  *height = h;

  blk = (block_t*)(hdr+1);
  ablk = blk + w*h/16;
  for (y = 0; y < h; y += 8) {
    for (x = 0; x < w; x += 8) {
      // Decode in CMPR subtile arrangement
      DecodeDXT1(blk++, ablk++, w, output);
      DecodeDXT1(blk++, ablk++, w, output+4);
      DecodeDXT1(blk++, ablk++, w, output + w*4);
      DecodeDXT1(blk++, ablk++, w, output+4 + w*4);

      output += 8;
    }

    output += w*7;
  }

  return gct_SUCCESS;
}
