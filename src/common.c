/******************************************************************************
 *
 * Copyright(c) 2022 Lian Ferrand
 * This file is part of GCTLib
 *
 * File description:
 *  Common internal resources
 *
 ******************************************************************************/

#include "gct/gctlib.h"
#include "common.h"

gct_b32 ValidImageSize(gct_i32 width, gct_i32 height) {
  return ((width > 7) && (height > 7) &&
          !(width & 7) && !(height & 7));
}

gct_b32 SupportedImageFlags(gct_hdr_flags_t flags) {
  return flags == gct_HDR_TRANSP_FLAGS;
}
