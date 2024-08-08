#if defined(VENDOR_TEE_WRAP_C)
#include "common.h"

#include <string.h>

#include "mbedtls/hw_tee_wrapper.h"
#include "mbedtls/aes.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#if defined(VENDOR_REDEFINE_TEE_API_C)
#include "mbedtls/hw_redefine_tee_api.h"
#endif
#include "mbedtls/hw_tee_tag.h"

#if defined(MBEDTLS_AESNI_C)
#include "aesni.h"
#endif

#include <stdio.h>

static const unsigned char FSb[256] =
{
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5,
    0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0,
    0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC,
    0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A,
    0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0,
    0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B,
    0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85,
    0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5,
    0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17,
    0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88,
    0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C,
    0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9,
    0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6,
    0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E,
    0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94,
    0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68,
    0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};
/*
 * Round constants
 */
static const uint32_t RCON[10] =
{
    0x00000001, 0x00000002, 0x00000004, 0x00000008,
    0x00000010, 0x00000020, 0x00000040, 0x00000080,
    0x0000001B, 0x00000036
};

/*
 * Reverse S-box
 */
static const unsigned char RSb[256] =
{
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38,
    0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87,
    0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D,
    0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2,
    0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
    0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16,
    0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
    0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA,
    0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A,
    0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
    0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02,
    0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
    0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA,
    0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85,
    0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
    0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89,
    0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
    0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20,
    0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31,
    0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
    0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D,
    0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
    0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0,
    0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26,
    0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

/* Parameter validation macros based on platform_util.h */
#define AES_VALIDATE_RET( cond )    \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_AES_BAD_INPUT_DATA )

extern uint32_t FT0[256];
#if !defined(MBEDTLS_AES_FEWER_TABLES)
extern uint32_t FT1[256];
extern uint32_t FT2[256];
extern uint32_t FT3[256];
#endif /* !MBEDTLS_AES_FEWER_TABLES */

extern uint32_t RT0[256];
#if !defined(MBEDTLS_AES_FEWER_TABLES)
extern uint32_t RT1[256];
extern uint32_t RT2[256];
extern uint32_t RT3[256];
#endif /* !MBEDTLS_AES_FEWER_TABLES */

#if defined(MBEDTLS_AES_FEWER_TABLES)

#define ROTL8(x)  ( (uint32_t)( ( x ) <<  8 ) + (uint32_t)( ( x ) >> 24 ) )
#define ROTL16(x) ( (uint32_t)( ( x ) << 16 ) + (uint32_t)( ( x ) >> 16 ) )
#define ROTL24(x) ( (uint32_t)( ( x ) << 24 ) + (uint32_t)( ( x ) >>  8 ) )

#define AES_RT0(idx) RT0[idx]
#define AES_RT1(idx) ROTL8(  RT0[idx] )
#define AES_RT2(idx) ROTL16( RT0[idx] )
#define AES_RT3(idx) ROTL24( RT0[idx] )

#define AES_FT0(idx) FT0[idx]
#define AES_FT1(idx) ROTL8(  FT0[idx] )
#define AES_FT2(idx) ROTL16( FT0[idx] )
#define AES_FT3(idx) ROTL24( FT0[idx] )

#else /* MBEDTLS_AES_FEWER_TABLES */

#define AES_RT0(idx) RT0[idx]
#define AES_RT1(idx) RT1[idx]
#define AES_RT2(idx) RT2[idx]
#define AES_RT3(idx) RT3[idx]

#define AES_FT0(idx) FT0[idx]
#define AES_FT1(idx) FT1[idx]
#define AES_FT2(idx) FT2[idx]
#define AES_FT3(idx) FT3[idx]

#endif /* MBEDTLS_AES_FEWER_TABLES */

#define AES_FROUND(X0,X1,X2,X3,Y0,Y1,Y2,Y3)                 \
    do                                                      \
    {                                                       \
        (X0) = *RK++ ^ AES_FT0( MBEDTLS_BYTE_0( Y0 ) ) ^    \
                       AES_FT1( MBEDTLS_BYTE_1( Y1 ) ) ^    \
                       AES_FT2( MBEDTLS_BYTE_2( Y2 ) ) ^    \
                       AES_FT3( MBEDTLS_BYTE_3( Y3 ) );     \
                                                            \
        (X1) = *RK++ ^ AES_FT0( MBEDTLS_BYTE_0( Y1 ) ) ^    \
                       AES_FT1( MBEDTLS_BYTE_1( Y2 ) ) ^    \
                       AES_FT2( MBEDTLS_BYTE_2( Y3 ) ) ^    \
                       AES_FT3( MBEDTLS_BYTE_3( Y0 ) );     \
                                                            \
        (X2) = *RK++ ^ AES_FT0( MBEDTLS_BYTE_0( Y2 ) ) ^    \
                       AES_FT1( MBEDTLS_BYTE_1( Y3 ) ) ^    \
                       AES_FT2( MBEDTLS_BYTE_2( Y0 ) ) ^    \
                       AES_FT3( MBEDTLS_BYTE_3( Y1 ) );     \
                                                            \
        (X3) = *RK++ ^ AES_FT0( MBEDTLS_BYTE_0( Y3 ) ) ^    \
                       AES_FT1( MBEDTLS_BYTE_1( Y0 ) ) ^    \
                       AES_FT2( MBEDTLS_BYTE_2( Y1 ) ) ^    \
                       AES_FT3( MBEDTLS_BYTE_3( Y2 ) );     \
    } while( 0 )

#define AES_RROUND(X0,X1,X2,X3,Y0,Y1,Y2,Y3)                 \
    do                                                      \
    {                                                       \
        (X0) = *RK++ ^ AES_RT0( MBEDTLS_BYTE_0( Y0 ) ) ^    \
                       AES_RT1( MBEDTLS_BYTE_1( Y3 ) ) ^    \
                       AES_RT2( MBEDTLS_BYTE_2( Y2 ) ) ^    \
                       AES_RT3( MBEDTLS_BYTE_3( Y1 ) );     \
                                                            \
        (X1) = *RK++ ^ AES_RT0( MBEDTLS_BYTE_0( Y1 ) ) ^    \
                       AES_RT1( MBEDTLS_BYTE_1( Y0 ) ) ^    \
                       AES_RT2( MBEDTLS_BYTE_2( Y3 ) ) ^    \
                       AES_RT3( MBEDTLS_BYTE_3( Y2 ) );     \
                                                            \
        (X2) = *RK++ ^ AES_RT0( MBEDTLS_BYTE_0( Y2 ) ) ^    \
                       AES_RT1( MBEDTLS_BYTE_1( Y1 ) ) ^    \
                       AES_RT2( MBEDTLS_BYTE_2( Y0 ) ) ^    \
                       AES_RT3( MBEDTLS_BYTE_3( Y3 ) );     \
                                                            \
        (X3) = *RK++ ^ AES_RT0( MBEDTLS_BYTE_0( Y3 ) ) ^    \
                       AES_RT1( MBEDTLS_BYTE_1( Y2 ) ) ^    \
                       AES_RT2( MBEDTLS_BYTE_2( Y1 ) ) ^    \
                       AES_RT3( MBEDTLS_BYTE_3( Y0 ) );     \
    } while( 0 )


FLA_TAG STROBF_TAG
int32_t wrap_key(const uint8_t *raw_key, uint32_t raw_size, uint8_t *out_key, uint32_t out_size)
{
    if (raw_key == NULL || out_key == NULL || out_size != raw_size)
        return -1;

    uint8_t random[] = {
        0x8A, 0x18, 0x0A, 0xFE, 0x30, 0xB3, 0x03, 0x12, 0x19, 0xB5, 0xF7, 0xC2, 0xB6, 0x6E, 0x0F, 0xB0,
        0xCF, 0xAB, 0x38, 0x54, 0xBC, 0xEB, 0xFD, 0x1A, 0xF3, 0x0F, 0x74, 0x82, 0x64, 0x21, 0xF1, 0x9C,
        0x8A, 0x18, 0x0A, 0xFE, 0x30, 0xB3, 0x03, 0x12, 0x19, 0xB5, 0xF7, 0xC2, 0xB6, 0x6E, 0x0F, 0xB0,
        0xCF, 0xAB, 0x38, 0x54, 0xBC, 0xEB, 0xFD, 0x1A, 0xF3, 0x0F, 0x74, 0x82, 0x64, 0x21, 0xF1, 0x9C,
        0x8A, 0x18, 0x0A, 0xFE, 0x30, 0xB3, 0x03, 0x12, 0x19, 0xB5, 0xF7, 0xC2, 0xB6, 0x6E, 0x0F, 0xB0,
        0xCF, 0xAB, 0x38, 0x54, 0xBC, 0xEB, 0xFD, 0x1A, 0xF3, 0x0F, 0x74, 0x82, 0x64, 0x21, 0xF1, 0x9C,
        0x8A, 0x18, 0x0A, 0xFE, 0x30, 0xB3, 0x03, 0x12, 0x19, 0xB5, 0xF7, 0xC2, 0xB6, 0x6E, 0x0F, 0xB0,
        0xCF, 0xAB, 0x38, 0x54, 0xBC, 0xEB, 0xFD, 0x1A, 0xF3, 0x0F, 0x74, 0x82, 0x64, 0x21, 0xF1, 0x9C,
        0x8A, 0x18, 0x0A, 0xFE, 0x30, 0xB3, 0x03, 0x12, 0x19, 0xB5, 0xF7, 0xC2, 0xB6, 0x6E, 0x0F, 0xB0,
        0xCF, 0xAB, 0x38, 0x54, 0xBC, 0xEB, 0xFD, 0x1A, 0xF3, 0x0F, 0x74, 0x82, 0x64, 0x21, 0xF1, 0x9C,
        0x8A, 0x18, 0x0A, 0xFE, 0x30, 0xB3, 0x03, 0x12, 0x19, 0xB5, 0xF7, 0xC2, 0xB6, 0x6E, 0x0F, 0xB0,
        0xCF, 0xAB, 0x38, 0x54, 0xBC, 0xEB, 0xFD, 0x1A, 0xF3, 0x0F, 0x74, 0x82, 0x64, 0x21, 0xF1, 0x9C,
        0x8A, 0x18, 0x0A, 0xFE, 0x30, 0xB3, 0x03, 0x12, 0x19, 0xB5, 0xF7, 0xC2, 0xB6, 0x6E, 0x0F, 0xB0,
        0xCF, 0xAB, 0x38, 0x54, 0xBC, 0xEB, 0xFD, 0x1A, 0xF3, 0x0F, 0x74, 0x82, 0x64, 0x21, 0xF1, 0x9C,
        0x8A, 0x18, 0x0A, 0xFE, 0x30, 0xB3, 0x03, 0x12, 0x19, 0xB5, 0xF7, 0xC2, 0xB6, 0x6E, 0x0F, 0xB0,
        0xCF, 0xAB, 0x38, 0x54, 0xBC, 0xEB, 0xFD, 0x1A, 0xF3, 0x0F, 0x74, 0x82, 0x64, 0x21, 0xF1, 0x9C,
        0x8A, 0x18, 0x0A, 0xFE, 0x30, 0xB3, 0x03, 0x12, 0x19, 0xB5, 0xF7, 0xC2, 0xB6, 0x6E, 0x0F, 0xB0,
        0xCF, 0xAB, 0x38, 0x54, 0xBC, 0xEB, 0xFD, 0x1A, 0xF3, 0x0F, 0x74, 0x82, 0x64, 0x21, 0xF1, 0x9C,
    };

    if (raw_size > sizeof(random))
        return -1;

    for (uint32_t i = 0; i < raw_size; i++) {
        out_key[i] = raw_key[i] ^ random[i];
    }
    return 0;
}

FLA_TAG
int unwrap_key(uint32_t type, const uint8_t *wkey, uint32_t wrap_size,
    uint8_t *raw_key, uint32_t size)
{
    int32_t rc;
    if (wkey == NULL || raw_key == NULL || wrap_size != size)
        return -1;

    if (type == 0x01) {
        rc = wrap_key(wkey, wrap_size, raw_key, size);
    } else if (type == 0x00) {
        rc = memcpy_s(raw_key, size, wkey, size);
    } else {
        rc = -1;
    }
    return rc;
}

/*
 * AES key schedule (encryption)
 */
#if !defined(MBEDTLS_AES_SETKEY_ENC_ALT)
FLA_TAG STROBF_TAG
static int internal_aes_setkey_enc(mbedtls_aes_context *ctx, unsigned int type,
    const unsigned char *key, unsigned int keybits)
{
    unsigned int i;
    uint32_t *RK;
    (void)type;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( key != NULL );

    switch( keybits )
    {
        case 128: ctx->nr = 10; break;
        case 192: ctx->nr = 12; break;
        case 256: ctx->nr = 14; break;
        default : return( MBEDTLS_ERR_AES_INVALID_KEY_LENGTH );
    }

#if !defined(MBEDTLS_AES_ROM_TABLES)
    if( aes_init_done == 0 )
    {
        aes_gen_tables();
        aes_init_done = 1;
    }
#endif

#if defined(MBEDTLS_PADLOCK_C) && defined(MBEDTLS_PADLOCK_ALIGN16)
    if( aes_padlock_ace == -1 )
        aes_padlock_ace = mbedtls_padlock_has_support( MBEDTLS_PADLOCK_ACE );

    if( aes_padlock_ace )
        ctx->rk = RK = MBEDTLS_PADLOCK_ALIGN16( ctx->buf );
    else
#endif
    ctx->rk = RK = ctx->buf;
    
    uint8_t *raw_key = tee_malloc(keybits / 8);
    if (raw_key == NULL)
        return -1;

    int ret = unwrap_key(type, key, keybits / 8, (uint8_t *)raw_key, keybits / 8);
    if (ret != 0) {
        tee_free(raw_key, keybits / 8);
        return ret;
    }

    uint32_t temp[68] = {0};
    RK = temp;
    ret = unwrap_key(ctx->key_type, (uint8_t *)ctx->rk, sizeof(ctx->buf), (uint8_t *)RK, sizeof(temp));
    if (ret != 0) {
        tee_free(raw_key, keybits / 8);
        return ret;
    }

#if defined(MBEDTLS_AESNI_C) && defined(MBEDTLS_HAVE_X86_64)
    if( mbedtls_aesni_has_support( MBEDTLS_AESNI_AES ) ) {
        ret = mbedtls_aesni_setkey_enc( (unsigned char *) ctx->rk, (uint8_t *)raw_key, keybits );
        tee_free(raw_key, keybits / 8);
        return ret;
    }
#endif
    for( i = 0; i < ( keybits >> 5 ); i++ ) {
        RK[i] = MBEDTLS_GET_UINT32_LE( raw_key, i << 2 );
    }

    switch( ctx->nr )
    {
        case 10:

            for( i = 0; i < 10; i++, RK += 4 )
            {
                RK[4]  = RK[0] ^ RCON[i] ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_1( RK[3] ) ]       ) ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_2( RK[3] ) ] <<  8 ) ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_3( RK[3] ) ] << 16 ) ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_0( RK[3] ) ] << 24 );

                RK[5]  = RK[1] ^ RK[4];
                RK[6]  = RK[2] ^ RK[5];
                RK[7]  = RK[3] ^ RK[6];
            }
            break;

        case 12:

            for( i = 0; i < 8; i++, RK += 6 )
            {
                RK[6]  = RK[0] ^ RCON[i] ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_1( RK[5] ) ]       ) ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_2( RK[5] ) ] <<  8 ) ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_3( RK[5] ) ] << 16 ) ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_0( RK[5] ) ] << 24 );

                RK[7]  = RK[1] ^ RK[6];
                RK[8]  = RK[2] ^ RK[7];
                RK[9]  = RK[3] ^ RK[8];
                RK[10] = RK[4] ^ RK[9];
                RK[11] = RK[5] ^ RK[10];
            }
            break;

        case 14:

            for( i = 0; i < 7; i++, RK += 8 )
            {
                RK[8]  = RK[0] ^ RCON[i] ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_1( RK[7] ) ]       ) ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_2( RK[7] ) ] <<  8 ) ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_3( RK[7] ) ] << 16 ) ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_0( RK[7] ) ] << 24 );

                RK[9]  = RK[1] ^ RK[8];
                RK[10] = RK[2] ^ RK[9];
                RK[11] = RK[3] ^ RK[10];

                RK[12] = RK[4] ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_0( RK[11] ) ]       ) ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_1( RK[11] ) ] <<  8 ) ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_2( RK[11] ) ] << 16 ) ^
                ( (uint32_t) FSb[ MBEDTLS_BYTE_3( RK[11] ) ] << 24 );

                RK[13] = RK[5] ^ RK[12];
                RK[14] = RK[6] ^ RK[13];
                RK[15] = RK[7] ^ RK[14];
            }
            break;
    }

    uint32_t out_key[68] = {0};
    ret = wrap_key((uint8_t *)temp, sizeof(temp), (uint8_t *)out_key, sizeof(out_key));
    tee_free(raw_key, keybits / 8);
    raw_key = NULL;
    (void)memset_s(temp, sizeof(temp), 0, sizeof(temp));
    if (ret != 0)
        return ret;

    ctx->key_type = 0x01;
    ret = memcpy_s((uint8_t *)ctx->rk, sizeof(ctx->buf), out_key, sizeof(out_key));
    (void)memset_s(out_key, sizeof(out_key), 0, sizeof(out_key));
    if (ret != 0)
        return ret;
    return( 0 );
}

FLA_TAG STROBF_TAG
int mbedtls_aes_set_wrap_key_enc(mbedtls_aes_context *ctx, uint32_t type, const unsigned char *key,
    unsigned int keybits)
{
    return internal_aes_setkey_enc(ctx, type, key, keybits);
}

#endif /* !MBEDTLS_AES_SETKEY_ENC_ALT */

/*
 * AES key schedule (decryption)
 */
#if !defined(MBEDTLS_AES_SETKEY_DEC_ALT)
FLA_TAG STROBF_TAG
static int internal_aes_setkey_dec( mbedtls_aes_context *ctx, unsigned int type,
    const unsigned char *key, unsigned int keybits )
{
    int i, j, ret;
    mbedtls_aes_context cty;
    uint32_t *RK;
    uint32_t *SK;
    (void)type;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( key != NULL );

    mbedtls_aes_init( &cty );

#if defined(MBEDTLS_PADLOCK_C) && defined(MBEDTLS_PADLOCK_ALIGN16)
    if( aes_padlock_ace == -1 )
        aes_padlock_ace = mbedtls_padlock_has_support( MBEDTLS_PADLOCK_ACE );

    if( aes_padlock_ace )
        ctx->rk = RK = MBEDTLS_PADLOCK_ALIGN16( ctx->buf );
    else
#endif
    ctx->rk = RK = ctx->buf;

    uint32_t temp[68] = {0};
    RK = temp;
    ret = unwrap_key(ctx->key_type, (uint8_t *)ctx->rk, sizeof(ctx->buf),
        (uint8_t *)RK, sizeof(temp));
    if (ret != 0)
        goto exit;

    /* Also checks keybits */
    if( ( ret = internal_aes_setkey_enc( &cty, type, key, keybits ) ) != 0 )
        goto exit;

    uint32_t out_cty_rk[68] = {0};
    ret = unwrap_key(cty.key_type, (uint8_t *)cty.rk, sizeof(cty.buf),
        (uint8_t *)out_cty_rk, sizeof(out_cty_rk));
    if (ret != 0)
        goto exit;

    ctx->nr = cty.nr;

#if defined(MBEDTLS_AESNI_C) && defined(MBEDTLS_HAVE_X86_64)
    if( mbedtls_aesni_has_support( MBEDTLS_AESNI_AES ) )
    {
        mbedtls_aesni_inverse_key( (unsigned char *) ctx->rk,
                           (const unsigned char *) out_cty_rk, ctx->nr );
        goto exit;
    }
#endif
    SK = (uint32_t *)out_cty_rk + cty.nr * 4;

    *RK++ = *SK++;
    *RK++ = *SK++;
    *RK++ = *SK++;
    *RK++ = *SK++;

    for( i = ctx->nr - 1, SK -= 8; i > 0; i--, SK -= 8 )
    {
        for( j = 0; j < 4; j++, SK++ )
        {
            *RK++ = AES_RT0( FSb[ MBEDTLS_BYTE_0( *SK ) ] ) ^
                    AES_RT1( FSb[ MBEDTLS_BYTE_1( *SK ) ] ) ^
                    AES_RT2( FSb[ MBEDTLS_BYTE_2( *SK ) ] ) ^
                    AES_RT3( FSb[ MBEDTLS_BYTE_3( *SK ) ] );
        }
    }

    *RK++ = *SK++;
    *RK++ = *SK++;
    *RK++ = *SK++;
    *RK++ = *SK++;

    uint32_t out_key[68] = {0};
    ret = wrap_key((uint8_t *)temp, sizeof(temp), (uint8_t *)out_key, sizeof(out_key));
    if (ret != 0)
        goto exit;

    ctx->key_type = 0x01;
    ret = memcpy_s((uint8_t *)ctx->rk, sizeof(ctx->buf), out_key, sizeof(out_key));
exit:
    mbedtls_aes_free( &cty );
    (void)memset_s(out_key, sizeof(out_key), 0, sizeof(out_key));
    (void)memset_s(temp, sizeof(temp), 0, sizeof(temp));
    (void)memset_s(out_cty_rk, sizeof(out_cty_rk), 0, sizeof(out_cty_rk));
    return( ret );
}

FLA_TAG STROBF_TAG
int mbedtls_aes_set_wrap_key_dec(mbedtls_aes_context *ctx, uint32_t type,
    const unsigned char *key, unsigned int keybits)
{
    return internal_aes_setkey_dec(ctx, type, key, keybits);
}
#endif /* !MBEDTLS_AES_SETKEY_DEC_ALT */

#if !defined(MBEDTLS_AES_ENCRYPT_ALT)
FLA_TAG STROBF_TAG
int mbedtls_internal_aes_encrypt( mbedtls_aes_context *ctx,
                                  const unsigned char input[16],
                                  unsigned char output[16] )
{
    int i;
    uint32_t *RK = ctx->rk;
    struct
    {
        uint32_t X[4];
        uint32_t Y[4];
    } t;

    uint32_t out[68] = {0};
    RK = out;
    int ret = unwrap_key(ctx->key_type, (uint8_t *)ctx->rk, sizeof(ctx->buf), (uint8_t *)out, sizeof(out));
    if (ret != 0)
        return -1;

    t.X[0] = MBEDTLS_GET_UINT32_LE( input,  0 ); t.X[0] ^= *RK++;
    t.X[1] = MBEDTLS_GET_UINT32_LE( input,  4 ); t.X[1] ^= *RK++;
    t.X[2] = MBEDTLS_GET_UINT32_LE( input,  8 ); t.X[2] ^= *RK++;
    t.X[3] = MBEDTLS_GET_UINT32_LE( input, 12 ); t.X[3] ^= *RK++;

    for( i = ( ctx->nr >> 1 ) - 1; i > 0; i-- )
    {
        AES_FROUND( t.Y[0], t.Y[1], t.Y[2], t.Y[3], t.X[0], t.X[1], t.X[2], t.X[3] );
        AES_FROUND( t.X[0], t.X[1], t.X[2], t.X[3], t.Y[0], t.Y[1], t.Y[2], t.Y[3] );
    }

    AES_FROUND( t.Y[0], t.Y[1], t.Y[2], t.Y[3], t.X[0], t.X[1], t.X[2], t.X[3] );

    t.X[0] = *RK++ ^ \
            ( (uint32_t) FSb[ MBEDTLS_BYTE_0( t.Y[0] ) ]       ) ^
            ( (uint32_t) FSb[ MBEDTLS_BYTE_1( t.Y[1] ) ] <<  8 ) ^
            ( (uint32_t) FSb[ MBEDTLS_BYTE_2( t.Y[2] ) ] << 16 ) ^
            ( (uint32_t) FSb[ MBEDTLS_BYTE_3( t.Y[3] ) ] << 24 );

    t.X[1] = *RK++ ^ \
            ( (uint32_t) FSb[ MBEDTLS_BYTE_0( t.Y[1] ) ]       ) ^
            ( (uint32_t) FSb[ MBEDTLS_BYTE_1( t.Y[2] ) ] <<  8 ) ^
            ( (uint32_t) FSb[ MBEDTLS_BYTE_2( t.Y[3] ) ] << 16 ) ^
            ( (uint32_t) FSb[ MBEDTLS_BYTE_3( t.Y[0] ) ] << 24 );

    t.X[2] = *RK++ ^ \
            ( (uint32_t) FSb[ MBEDTLS_BYTE_0( t.Y[2] ) ]       ) ^
            ( (uint32_t) FSb[ MBEDTLS_BYTE_1( t.Y[3] ) ] <<  8 ) ^
            ( (uint32_t) FSb[ MBEDTLS_BYTE_2( t.Y[0] ) ] << 16 ) ^
            ( (uint32_t) FSb[ MBEDTLS_BYTE_3( t.Y[1] ) ] << 24 );

    t.X[3] = *RK++ ^ \
            ( (uint32_t) FSb[ MBEDTLS_BYTE_0( t.Y[3] ) ]       ) ^
            ( (uint32_t) FSb[ MBEDTLS_BYTE_1( t.Y[0] ) ] <<  8 ) ^
            ( (uint32_t) FSb[ MBEDTLS_BYTE_2( t.Y[1] ) ] << 16 ) ^
            ( (uint32_t) FSb[ MBEDTLS_BYTE_3( t.Y[2] ) ] << 24 );

    MBEDTLS_PUT_UINT32_LE( t.X[0], output,  0 );
    MBEDTLS_PUT_UINT32_LE( t.X[1], output,  4 );
    MBEDTLS_PUT_UINT32_LE( t.X[2], output,  8 );
    MBEDTLS_PUT_UINT32_LE( t.X[3], output, 12 );

    mbedtls_platform_zeroize( &t, sizeof( t ) );
    (void)memset_s(out, sizeof(out), 0, sizeof(out));
    return( 0 );
}
#endif /* !MBEDTLS_AES_ENCRYPT_ALT */

#if !defined(MBEDTLS_AES_DECRYPT_ALT)
FLA_TAG STROBF_TAG
int mbedtls_internal_aes_decrypt( mbedtls_aes_context *ctx,
                                  const unsigned char input[16],
                                  unsigned char output[16] )
{
    int i;
    uint32_t *RK = ctx->rk;
    struct
    {
        uint32_t X[4];
        uint32_t Y[4];
    } t;

    uint32_t out[68] = {0};
    RK = out;
    int ret = unwrap_key(ctx->key_type, (uint8_t *)ctx->rk, sizeof(ctx->buf), (uint8_t *)out, sizeof(out));
    if (ret != 0)
        return -1;

    t.X[0] = MBEDTLS_GET_UINT32_LE( input,  0 ); t.X[0] ^= *RK++;
    t.X[1] = MBEDTLS_GET_UINT32_LE( input,  4 ); t.X[1] ^= *RK++;
    t.X[2] = MBEDTLS_GET_UINT32_LE( input,  8 ); t.X[2] ^= *RK++;
    t.X[3] = MBEDTLS_GET_UINT32_LE( input, 12 ); t.X[3] ^= *RK++;

    for( i = ( ctx->nr >> 1 ) - 1; i > 0; i-- )
    {
        AES_RROUND( t.Y[0], t.Y[1], t.Y[2], t.Y[3], t.X[0], t.X[1], t.X[2], t.X[3] );
        AES_RROUND( t.X[0], t.X[1], t.X[2], t.X[3], t.Y[0], t.Y[1], t.Y[2], t.Y[3] );
    }

    AES_RROUND( t.Y[0], t.Y[1], t.Y[2], t.Y[3], t.X[0], t.X[1], t.X[2], t.X[3] );

    t.X[0] = *RK++ ^ \
            ( (uint32_t) RSb[ MBEDTLS_BYTE_0( t.Y[0] ) ]       ) ^
            ( (uint32_t) RSb[ MBEDTLS_BYTE_1( t.Y[3] ) ] <<  8 ) ^
            ( (uint32_t) RSb[ MBEDTLS_BYTE_2( t.Y[2] ) ] << 16 ) ^
            ( (uint32_t) RSb[ MBEDTLS_BYTE_3( t.Y[1] ) ] << 24 );

    t.X[1] = *RK++ ^ \
            ( (uint32_t) RSb[ MBEDTLS_BYTE_0( t.Y[1] ) ]       ) ^
            ( (uint32_t) RSb[ MBEDTLS_BYTE_1( t.Y[0] ) ] <<  8 ) ^
            ( (uint32_t) RSb[ MBEDTLS_BYTE_2( t.Y[3] ) ] << 16 ) ^
            ( (uint32_t) RSb[ MBEDTLS_BYTE_3( t.Y[2] ) ] << 24 );

    t.X[2] = *RK++ ^ \
            ( (uint32_t) RSb[ MBEDTLS_BYTE_0( t.Y[2] ) ]       ) ^
            ( (uint32_t) RSb[ MBEDTLS_BYTE_1( t.Y[1] ) ] <<  8 ) ^
            ( (uint32_t) RSb[ MBEDTLS_BYTE_2( t.Y[0] ) ] << 16 ) ^
            ( (uint32_t) RSb[ MBEDTLS_BYTE_3( t.Y[3] ) ] << 24 );

    t.X[3] = *RK++ ^ \
            ( (uint32_t) RSb[ MBEDTLS_BYTE_0( t.Y[3] ) ]       ) ^
            ( (uint32_t) RSb[ MBEDTLS_BYTE_1( t.Y[2] ) ] <<  8 ) ^
            ( (uint32_t) RSb[ MBEDTLS_BYTE_2( t.Y[1] ) ] << 16 ) ^
            ( (uint32_t) RSb[ MBEDTLS_BYTE_3( t.Y[0] ) ] << 24 );

    MBEDTLS_PUT_UINT32_LE( t.X[0], output,  0 );
    MBEDTLS_PUT_UINT32_LE( t.X[1], output,  4 );
    MBEDTLS_PUT_UINT32_LE( t.X[2], output,  8 );
    MBEDTLS_PUT_UINT32_LE( t.X[3], output, 12 );

    mbedtls_platform_zeroize( &t, sizeof( t ) );
    (void)memset_s(out, sizeof(out), 0, sizeof(out));
    return( 0 );
}
#endif /* !MBEDTLS_AES_DECRYPT_ALT */

/* Parameter validation macros */
#define GCM_VALIDATE_RET( cond ) \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_GCM_BAD_INPUT )
extern int gcm_gen_table( mbedtls_gcm_context *ctx );
VMP_TAG
static int internal_gcm_setkey( mbedtls_gcm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        uint32_t type,
                        const unsigned char *key,
                        unsigned int keybits )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    const mbedtls_cipher_info_t *cipher_info;

    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( key != NULL );
    GCM_VALIDATE_RET( keybits == 128 || keybits == 192 || keybits == 256 );

    cipher_info = mbedtls_cipher_info_from_values( cipher, keybits,
                                                   MBEDTLS_MODE_ECB );
    if( cipher_info == NULL )
        return( MBEDTLS_ERR_GCM_BAD_INPUT );

    if( cipher_info->block_size != 16 )
        return( MBEDTLS_ERR_GCM_BAD_INPUT );

    mbedtls_cipher_free( &ctx->cipher_ctx );

    if( ( ret = mbedtls_cipher_setup( &ctx->cipher_ctx, cipher_info ) ) != 0 )
        return( ret );

    if( ( ret = mbedtls_cipher_set_wrap_key( &ctx->cipher_ctx, type, key, keybits,
                               MBEDTLS_ENCRYPT ) ) != 0 )
    {
        return( ret );
    }

    if( ( ret = gcm_gen_table( ctx ) ) != 0 )
        return( ret );

    return( 0 );
}

VMP_TAG
int mbedtls_gcm_set_wrap_key( mbedtls_gcm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        uint32_t type,
                        const unsigned char *key,
                        unsigned int keybits )
{
    return internal_gcm_setkey(ctx, cipher, type, key, keybits);
}

FLA_TAG STROBF_TAG
int mbedtls_gcm_set_wrap_key_ext( mbedtls_gcm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        uint32_t type,
                        const unsigned char *key,
                        unsigned int keybits )
{
    return mbedtls_gcm_set_wrap_key(ctx, cipher, type, key, keybits);
}

int mbedtls_gcm_auth_decrypt_ext( mbedtls_gcm_context *ctx,
                      size_t length,
                      const unsigned char *iv,
                      size_t iv_len,
                      const unsigned char *add,
                      size_t add_len,
                      const unsigned char *tag,
                      size_t tag_len,
                      const unsigned char *input,
                      unsigned char *output )
{
    return mbedtls_gcm_auth_decrypt(ctx, length, iv, iv_len, add, add_len, tag, tag_len, input, output);
}

#define CIPHER_VALIDATE_RET( cond )    \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA )
VMP_TAG
static int internal_cipher_setkey( mbedtls_cipher_context_t *ctx,
                           unsigned int type,
                           const unsigned char *key,
                           int key_bitlen,
                           const mbedtls_operation_t operation )
{
    CIPHER_VALIDATE_RET( ctx != NULL );
    CIPHER_VALIDATE_RET( key != NULL );
    CIPHER_VALIDATE_RET( operation == MBEDTLS_ENCRYPT ||
                         operation == MBEDTLS_DECRYPT );
    if( ctx->cipher_info == NULL )
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

    if( ( ctx->cipher_info->flags & MBEDTLS_CIPHER_VARIABLE_KEY_LEN ) == 0 &&
        (int) ctx->cipher_info->key_bitlen != key_bitlen )
    {
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }

    ctx->key_bitlen = key_bitlen;
    ctx->operation = operation;

    /*
     * For OFB, CFB and CTR mode always use the encryption key schedule
     */
    if( MBEDTLS_ENCRYPT == operation ||
        MBEDTLS_MODE_CFB == ctx->cipher_info->mode ||
        MBEDTLS_MODE_OFB == ctx->cipher_info->mode ||
        MBEDTLS_MODE_CTR == ctx->cipher_info->mode )
    {
        return( mbedtls_aes_set_wrap_key_enc( ctx->cipher_ctx, type, key, ctx->key_bitlen ) );
    }

    if( MBEDTLS_DECRYPT == operation )
        return( mbedtls_aes_set_wrap_key_dec( ctx->cipher_ctx, type, key, ctx->key_bitlen ) );

    return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
}

VMP_TAG
int mbedtls_cipher_set_wrap_key( mbedtls_cipher_context_t *ctx,
                           unsigned int key_type,
                           const unsigned char *key,
                           int key_bitlen,
                           const mbedtls_operation_t operation )
{
    return internal_cipher_setkey(ctx, key_type, key, key_bitlen, operation);
}
#endif /* VENDOR_TEE_WRAP_C */
