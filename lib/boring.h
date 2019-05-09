/* BoringSSL compatibility include file */

#ifndef _BORING_H
#define _BORING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <openssl/curve25519.h>

/* Most of the rest of this include file was taken from OpenSSL */
/* From opensslconf.h */
#ifndef OPENSSL_FILE
# ifdef OPENSSL_NO_FILENAMES
#  define OPENSSL_FILE ""
#  define OPENSSL_LINE 0
# else
#  define OPENSSL_FILE __FILE__
#  define OPENSSL_LINE __LINE__
# endif
#endif

#include <openssl/err.h>
/* From err.h */
# define EVPerr(f,r)  ERR_put_error(ERR_LIB_EVP,(f),(r),OPENSSL_FILE,OPENSSL_LINE)

#include <openssl/mem.h>
inline void *OPENSSL_zalloc(int num)
{
    void *p = OPENSSL_malloc(num);
    if (p)
        OPENSSL_cleanse(p, num);
    return p;
}

inline void *OPENSSL_memdup(void *data, size_t s)
{
    void *p = OPENSSL_malloc(s);
    if (p)
        memcpy(p, data, s);
    return p;
}

/* From evp.h */
# define         EVP_AEAD_TLS1_AAD_LEN           13
# define         EVP_CTRL_AEAD_TLS1_AAD          0x16

/* From evperr.h */
# define EVP_F_CHACHA20_POLY1305_CTRL                     182
# define EVP_R_INITIALIZATION_ERROR                       134
# define EVP_R_COPY_ERROR                                 173

/* From openssl include/openssl/obj_mac.h */
/* ALREADY DEFINED IN BORINGSSL:
#define SN_chacha20_poly1305            "ChaCha20-Poly1305"
#define LN_chacha20_poly1305            "chacha20-poly1305"
#define NID_chacha20_poly1305           1018
*/
#define SN_chacha20             "ChaCha20"
#define LN_chacha20             "chacha20"
#define NID_chacha20            1019


/* Directly from openssl crypto/include/internal/chacha.h */
/*
 * Copyright 2015-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef HEADER_CHACHA_H
#define HEADER_CHACHA_H

#include <stddef.h>

/*
 * ChaCha20_ctr32 encrypts |len| bytes from |inp| with the given key and
 * nonce and writes the result to |out|, which may be equal to |inp|.
 * The |key| is not 32 bytes of verbatim key material though, but the
 * said material collected into 8 32-bit elements array in host byte
 * order. Same approach applies to nonce: the |counter| argument is
 * pointer to concatenated nonce and counter values collected into 4
 * 32-bit elements. This, passing crypto material collected into 32-bit
 * elements as opposite to passing verbatim byte vectors, is chosen for
 * efficiency in multi-call scenarios.
 */
/*
void ChaCha20_ctr32(unsigned char *out, const unsigned char *inp,
                    size_t len, const unsigned int key[8],
                    const unsigned int counter[4]);
*/
/*
 * You can notice that there is no key setup procedure. Because it's
 * as trivial as collecting bytes into 32-bit elements, it's reckoned
 * that below macro is sufficient.
 */
#define CHACHA_U8TOU32(p)  ( \
                ((unsigned int)(p)[0])     | ((unsigned int)(p)[1]<<8) | \
                ((unsigned int)(p)[2]<<16) | ((unsigned int)(p)[3]<<24)  )

#define CHACHA_KEY_SIZE         32
#define CHACHA_CTR_SIZE         16
#define CHACHA_BLK_SIZE         64

#endif

#include <openssl/poly1305.h>
/* From openssl: crypto/include/internal/poly1305.h: */
/*
 * Copyright 2015-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */
/*
#include <stddef.h>
*/
#define POLY1305_BLOCK_SIZE  16
#define POLY1305_DIGEST_SIZE 16
#define POLY1305_KEY_SIZE    32

//typedef struct poly1305_context POLY1305;
typedef uint8_t poly1305_state[512];
typedef poly1305_state POLY1305;

inline size_t Poly1305_ctx_size(void) { return sizeof(POLY1305); }

#define Poly1305_Init CRYPTO_poly1305_init
#define Poly1305_Update CRYPTO_poly1305_update
#define Poly1305_Final CRYPTO_poly1305_finish

const EVP_CIPHER *EVP_chacha20(void);
const EVP_CIPHER *EVP_chacha20_poly1305(void);

#ifdef __cplusplus
}
#endif

#endif
