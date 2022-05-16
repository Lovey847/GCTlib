/******************************************************************************
 *
 * Copyright(c) 2022 Lian Ferrand
 * This file is part of GCTLib
 *
 * File description:
 *  GCT encoder
 *  NOTE: This encodes to a very specific subset of GCT, which is
 *  a normal image plane, then an alpha plane. Right now this cannot
 *  encode other types of GCTs.
 *
 ******************************************************************************/

#include "gct/gctlib.h"

#include <string.h>

// Header-only DXT1 encoder by fabian "ryg" giesen and stb
// Copyright(c) 2017 Sean Barrett
// Look in stb_dxt.h for license information
#define STB_DXT_IMPLEMENTATION
#define STB_DXT_STATIC
#include "thirdParty/stb_dxt.h"

static gct_b32 ValidImageSize(gct_i32 width, gct_i32 height) {
  return ((width > 7) && (height > 7) &&
          !(width & 7) && !(height & 7));
}

static gct_b32 SupportedImageFlags(gct_u32 flags) {
  return flags == (gct_HDR_UNK01|gct_HDR_ALPHA);
}

gct_error_t gct_InitHeader(gct_header_t *hdr, int width,
                           int height, gct_u32 flags)
{
  if (!hdr)
    return gct_ERR_NULL_POINTER;
  else if (!ValidImageSize(width, height))
    return gct_ERR_INVALID_SIZE;
  else if (!SupportedImageFlags(flags))
    return gct_ERR_UNSUPPORTED_FLAGS;

  gct_STORE_BIG32(hdr->width, width);
  gct_STORE_BIG32(hdr->height, height);
  hdr->width2 = hdr->width;
  hdr->height2 = hdr->height;

  gct_STORE_BIG32(hdr->flags, flags);

  // This may be wrong
  gct_STORE_BIG32(hdr->orientation, 0);

  // Zero out padding
  hdr->pad[0] = hdr->pad[1] = 0;

  return gct_SUCCESS;
}

gct_iptr gct_EncodedSize(const gct_header_t *hdr) {
  gct_i32 width;
  gct_i32 height;

  if (!hdr) return -gct_ERR_NULL_POINTER;

  width = gct_SIGNED_BIG32(hdr->width);
  height = gct_SIGNED_BIG32(hdr->height);

  if ((width != gct_SIGNED_BIG32(hdr->width2)) ||
      (height != gct_SIGNED_BIG32(hdr->height2)) ||
      !ValidImageSize(width, height))
    return -gct_ERR_INVALID_SIZE;
  else if (!SupportedImageFlags(gct_BIG32(hdr->flags)))
    return -gct_ERR_UNSUPPORTED_FLAGS;

  // CMPR is 4-bits per pixel
  // Alpha channel is stored as second plane
  return width*height;
}

// Get 4x4 group of RGBA pixels from image
static void GetImageRect(const gct_color_t *src, gct_uptr stride,
                         gct_iptr x, gct_iptr y, gct_color_t *output)
{
  const gct_iptr xEnd = x+4;
  const gct_iptr yEnd = y+4;
  const gct_iptr xOrig = x;

  for (; y < yEnd; ++y) {
    for (; x < xEnd; ++x, ++output) {
      *output = src[y*stride + x];
    }

    x = xOrig;
  }
}

// Get 4x4 group of alpha pixels from image,
// coded as green pixels
static void GetAlphaRect(const gct_color_t *src, gct_uptr stride,
                         gct_iptr x, gct_iptr y, gct_color_t *output)
{
  const gct_iptr xEnd = x+4;
  const gct_iptr yEnd = y+4;
  const gct_iptr xOrig = x;

  for (; y < yEnd; ++y) {
    for (; x < xEnd; ++x, ++output) {
      output->r = output->b = output->a = 0;
      output->g = src[y*stride + x].a;
    }

    x = xOrig;
  }
}

// Flip 2-bit components in byte
static unsigned char FlipByte(unsigned char c) {
  return ((c<<6) |
          ((c<<2)&0x30) |
          ((c>>2)&0x0c) |
          (c>>6));
}

// We need to write the DXT1 in big endian, stb_compress_dxt_block writes it
// in little endian
static void SwapDXT(unsigned char *block) {
  unsigned char tmp;

  tmp = block[0];
  block[0] = block[1];
  block[1] = tmp;

  tmp = block[2];
  block[2] = block[3];
  block[3] = tmp;

  // stb_compress_dxt_block writes table
  // from lsb to msb, in CMPR it's from
  // msb to lsb
  block[4] = FlipByte(block[4]);
  block[5] = FlipByte(block[5]);
  block[6] = FlipByte(block[6]);
  block[7] = FlipByte(block[7]);
}

#define ENCODE_SUBTILE(_xOff, _yOff)                                    \
  GetImageRect(input, width, x+(_xOff), y+(_yOff), rect);               \
  stb_compress_dxt_block(block, (unsigned char*)rect,                   \
                         0, STB_DXT_HIGHQUAL);                          \
  SwapDXT(block);                                                       \
  block += 8;                                                           \
                                                                        \
  GetAlphaRect(input, width, x+(_xOff), y+(_yOff), rect);               \
  stb_compress_dxt_block(alpha, (unsigned char*)rect,                   \
                         0, STB_DXT_HIGHQUAL);                          \
  SwapDXT(alpha);                                                       \
  alpha += 8

gct_error_t gct_Encode(const gct_header_t *hdr,
                       const gct_color_t *input, void *output)
{
  gct_iptr x, y;
  gct_color_t rect[16];
  unsigned char *block, *alpha;
  gct_i32 width;
  gct_i32 height;

  // These will give "unused function" warnings otherwise,
  // kinda wish there was a way to disable their inclusion
  (void)stb_compress_bc5_block;
  (void)stb_compress_bc4_block;

  if (!hdr || !input || !output)
    return gct_ERR_NULL_POINTER;

  width = gct_SIGNED_BIG32(hdr->width);
  height = gct_SIGNED_BIG32(hdr->height);

  if ((width != gct_SIGNED_BIG32(hdr->width2)) ||
      (height != gct_SIGNED_BIG32(hdr->height2)) ||
      !ValidImageSize(width, height))
    return gct_ERR_INVALID_SIZE;
  else if (!SupportedImageFlags(gct_BIG32(hdr->flags)))
    return gct_ERR_UNSUPPORTED_FLAGS;

  block = (unsigned char*)output;
  alpha = block + ((width*height) >> 1);
  for (y = 0; y < height; y += 8) {
    for (x = 0; x < width; x += 8) {
      ENCODE_SUBTILE(0, 0);
      ENCODE_SUBTILE(4, 0);
      ENCODE_SUBTILE(0, 4);
      ENCODE_SUBTILE(4, 4);
    }
  }

  return gct_SUCCESS;
}
