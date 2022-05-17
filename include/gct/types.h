/******************************************************************************
 *
 * Copyright(c) 2022 Lian Ferrand
 * This file is part of GCTlib
 *
 * File description:
 *  Base GCTlib types
 *
 ******************************************************************************/

#ifndef _GCT_TYPES_H
#define _GCT_TYPES_H

#include <stddef.h>
#include <limits.h>

/* 8 bit types */
#if CHAR_BIT == 8

typedef signed char gct_i8;
typedef unsigned char gct_u8;

#else
#error "Unknown 8-bit type!"
#endif

/* 16 bit types */
#if UINT_MAX == 65535

typedef int gct_i16;
typedef unsigned int gct_u16;

#elif USHRT_MAX == 65535

typedef short gct_i16;
typedef unsigned short gct_u16;

#else
#error "Unknown 16-bit type!"
#endif

/* 32 bit types */
#if ULONG_MAX == 4294967295

typedef long gct_i32;
typedef unsigned long gct_u32;

#elif UINT_MAX == 4294967295

typedef int gct_i32;
typedef unsigned int gct_u32;

#else
#error "Unknown 32-bit type!"
#endif

/* Pointer size types */
typedef ptrdiff_t gct_iptr;
typedef size_t gct_uptr;

/* Boolean types */
enum gct_bool_e {gct_false = 0, gct_true};

typedef gct_u8 gct_b8;
typedef gct_u16 gct_b16;
typedef gct_u32 gct_b32;
typedef gct_uptr gct_bptr;

/* File types, marked with endianness */
typedef struct gct_le16_s {
  gct_u8 le16[2];
} gct_le16_t;
typedef struct gct_be16_s {
  gct_u8 be16[2];
} gct_be16_t;
typedef struct gct_le32_s {
  gct_u8 le32[4];
} gct_le32_t;
typedef struct gct_be32_s {
  gct_u8 be32[4];
} gct_be32_t;

/* Macros to get values from endian-dependent fields */
#define gct_SIGNED_LITTLE16(_val) ((gct_i16)gct_LITTLE16(_val))
#define gct_SIGNED_BIG16(_val) ((gct_i16)gct_BIG16(_val))
#define gct_SIGNED_LITTLE32(_val) ((gct_i32)gct_LITTLE32(_val))
#define gct_SIGNED_BIG32(_val) ((gct_i32)gct_BIG32(_val))

#define gct_LITTLE16(_val) (                    \
    ((gct_u16)(_val).le16[0]     ) |            \
    ((gct_u16)(_val).le16[1] << 8)              \
    )

#define gct_BIG16(_val) (                       \
    ((gct_u16)(_val).be16[0] << 8) |            \
    ((gct_u16)(_val).be16[1]     )              \
    )

#define gct_LITTLE32(_val) (                    \
    ((gct_u32)(_val).le32[0]      ) |           \
    ((gct_u32)(_val).le32[1] <<  8) |           \
    ((gct_u32)(_val).le32[2] << 16) |           \
    ((gct_u32)(_val).le32[3] << 24)             \
    )

#define gct_BIG32(_val) (                       \
    ((gct_u32)(_val).be32[0] << 24) |           \
    ((gct_u32)(_val).be32[1] << 16) |           \
    ((gct_u32)(_val).be32[2] <<  8) |           \
    ((gct_u32)(_val).be32[3]      )             \
    )

/* Macros to store values in endian-dependent fields */
#define gct_STORE_LITTLE16(_field, _val) do {   \
    const gct_u16 v = (gct_u16)(_val);          \
    (_field).le16[0] = v & 0xff;                \
    (_field).le16[1] = v >> 8;                  \
  } while (0)

#define gct_STORE_BIG16(_field, _val) do {      \
    const gct_u16 v = (gct_u16)(_val);          \
    (_field).be16[0] = v >> 8;                  \
    (_field).be16[1] = v & 0xff;                \
  } while (0)

#define gct_STORE_LITTLE32(_field, _val) do {   \
    const gct_u32 v = (gct_u32)(_val);          \
    (_field).le32[0] = v & 0xff;                \
    (_field).le32[1] = (v >> 8) & 0xff;         \
    (_field).le32[2] = (v >> 16) & 0xff;        \
    (_field).le32[3] = v >> 24;                 \
  } while (0)

#define gct_STORE_BIG32(_field, _val) do {      \
    const gct_u32 v = (gct_u32)(_val);          \
    (_field).be32[0] = v >> 24;                 \
    (_field).be32[1] = (v >> 16) & 0xff;        \
    (_field).be32[2] = (v >> 8) & 0xff;         \
    (_field).be32[3] = v & 0xff;                \
  } while (0)

#endif /*_GCT_TYPES_H*/
