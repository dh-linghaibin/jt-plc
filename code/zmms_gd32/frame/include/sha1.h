/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#ifndef _SHA1_H_
#define _SHA1_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f10x.h"

// don't just forward declare as we want to pass it around by value
typedef struct _sha1_digest
{
    uint32_t digest[5];
} sha1_digest;

sha1_digest Sha1Digest_fromStr (const char* src);
void       Sha1Digest_toStr   (const sha1_digest* digest, char* dst);

// Streamable hashing
typedef struct _Sha1Ctx Sha1Ctx;
Sha1Ctx*   Sha1Ctx_create    (void);
void       Sha1Ctx_reset     (Sha1Ctx*);
void       Sha1Ctx_write     (Sha1Ctx*, const void* msg, uint64_t bytes);
sha1_digest Sha1Ctx_getDigest (Sha1Ctx*);
void       Sha1Ctx_release   (Sha1Ctx*);

// Helper for one-off buffer hashing
sha1_digest sha1_get (const void* msg, uint64_t bytes);

#ifdef __cplusplus
}
#endif

#endif
