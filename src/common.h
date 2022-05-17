/******************************************************************************
 *
 * Copyright(c) 2022 Lian Ferrand
 * This file is part of GCTLib
 *
 * File description:
 *  Common internal resources
 *
 ******************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

#include "gct/gctlib.h"

// Check if image size is valid
gct_b32 ValidImageSize(gct_i32 width, gct_i32 height);

// Check if GCT image flags are supported
gct_b32 SupportedImageFlags(gct_hdr_flags_t flags);

#endif //_COMMON_H
