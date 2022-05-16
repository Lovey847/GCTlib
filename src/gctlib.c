/******************************************************************************
 *
 * Copyright(c) 2022 Lian Ferrand
 * This file is part of GCTLib
 *
 * File description:
 *  Base GCTLib components
 *
 ******************************************************************************/

#include "gct/gctlib.h"

const char *gct_StrError(gct_error_t err) {
  static const char * const ErrorTable[gct_NUM_ERR_CODES] = {
    "Success", // gct_SUCCESS
    "Invalid image size", // gct_ERR_INVALID_SIZE
    "Unsupported image flags", // gct_ERR_UNSUPPORTED_FLAGS
    "Invalid NULL pointer", // gct_ERR_NULL_POINTER
  };

  if (err < 0) err = -err;
  if (err >= gct_NUM_ERR_CODES) return NULL;

  return ErrorTable[err];
}
