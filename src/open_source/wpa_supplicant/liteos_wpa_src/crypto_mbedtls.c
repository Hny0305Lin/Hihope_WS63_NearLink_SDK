/*
 * Wrapper functions for mbedtls libcrypto
 * Copyright (c) 2004-2017, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "crypto_mbedtls.h"
#include "securec.h"
#include "utils/common.h"
#include "crypto/crypto.h"
#include "mbedtls/ecp.h"
#include "mbedtls/bignum.h"
#include "mbedtls/dhm.h"
#include "mbedtls/md.h"
#include "mbedtls/cmac.h"
#include "mbedtls/aes.h"
#include "mbedtls/pkcs5.h"
#include "mbedtls/version.h"
#if defined(CONFIG_ECC) && defined(MBEDTLS_ECP_RESTARTABLE)
#include "mbedtls/ecdh.h"
#include "mbedtls/pk.h"
#endif

#ifdef SUPPORT_WATCHDOG
#include "watchdog.h"
#endif

#if defined(MBEDTLS_NIST_KW_C)
#include "mbedtls/nist_kw.h"
#endif
#include "utils/const_time.h"
#if defined(__LITEOS__)
#include "trng.h"
#endif

__attribute__((weak)) int md5_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac);

static int get_trng(void *p_rng, unsigned char *buf, size_t len)
{
	(void)p_rng;
#if defined(__LITEOS__)
	return uapi_drv_cipher_trng_get_random_bytes(buf, len) ? -1 : 0;
#else
	return 0;
#endif
}

int crypto_get_random(void *buf, size_t len)
{
#if defined(__LITEOS__) && !defined(WS53_PRODUCT_FPGA)
	return uapi_drv_cipher_trng_get_random_bytes(buf, len) ? -1 : 0;
#else
	return 0;
#endif
}

static int mbedtls_digest_vector(const mbedtls_md_info_t *md_info, size_t num_elem,
                                 const u8 *addr[], const size_t *len, u8 *mac)
{
	mbedtls_md_context_t ctx;
	size_t i;
	int ret;

	if (md_info == NULL || addr == NULL || len == NULL || mac == NULL)
		return MBEDTLS_ERR_MD_BAD_INPUT_DATA;

	mbedtls_md_init(&ctx);

	if ((ret = mbedtls_md_setup(&ctx, md_info, 0)) != 0)
		goto cleanup;

	if ((ret = mbedtls_md_starts(&ctx)) != 0)
		goto cleanup;

	for (i = 0; i < num_elem; i++) {
		if ((ret = mbedtls_md_update(&ctx, addr[i], len[i])) != 0)
			goto cleanup;
	}

	if ((ret = mbedtls_md_finish(&ctx, mac)) != 0)
		goto cleanup;

cleanup:
	mbedtls_md_free(&ctx);

	return ret;
}

#ifndef CONFIG_FIPS
int md4_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
#if (MBEDTLS_VERSION_NUMBER < 0x03000000)
	return mbedtls_digest_vector(mbedtls_md_info_from_type(MBEDTLS_MD_MD4), num_elem, addr, len, mac);
#else
	return mbedtls_digest_vector(NULL, num_elem, addr, len, mac);
#endif
}
#endif /* CONFIG_FIPS */

#ifndef CONFIG_FIPS
int md5_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
	return mbedtls_digest_vector(mbedtls_md_info_from_type(MBEDTLS_MD_MD5), num_elem, addr, len, mac);
}
#endif /* CONFIG_FIPS */

int sha1_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
	return mbedtls_digest_vector(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), num_elem, addr, len, mac);
}

#ifndef NO_SHA256_WRAPPER
int sha256_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
	return mbedtls_digest_vector(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), num_elem, addr, len, mac);
}
#endif

#ifndef NO_SHA384_WRAPPER
int sha384_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
	return mbedtls_digest_vector(mbedtls_md_info_from_type(MBEDTLS_MD_SHA384), num_elem, addr, len, mac);
}
#endif

#ifndef NO_SHA512_WRAPPER
int sha512_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
	return mbedtls_digest_vector(mbedtls_md_info_from_type(MBEDTLS_MD_SHA512), num_elem, addr, len, mac);
}
#endif

int mbedtls_hmac_vector(const mbedtls_md_info_t *md_info,
                        const u8 *key, size_t keylen, size_t num_elem,
                        const u8 *addr[], const size_t *len, u8 *mac)
{
	mbedtls_md_context_t ctx;
	size_t i;
	int ret;

	if (md_info == NULL || key == NULL || addr == NULL || len == NULL || mac == NULL)
		return MBEDTLS_ERR_MD_BAD_INPUT_DATA;

	mbedtls_md_init(&ctx);

	if ((ret = mbedtls_md_setup(&ctx, md_info, 1)) != 0)
		goto cleanup;

	if ((ret = mbedtls_md_hmac_starts(&ctx, key, keylen)) != 0)
		goto cleanup;

	for (i = 0; i < num_elem; i++) {
		if ((ret = mbedtls_md_hmac_update(&ctx, addr[i], len[i])) != 0)
			goto cleanup;
	}

	if ((ret = mbedtls_md_hmac_finish(&ctx, mac)) != 0)
		goto cleanup;

cleanup:
	mbedtls_md_free(&ctx);

	return ret;
}

int pbkdf2_sha1(const char *passphrase, const u8 *ssid, size_t ssid_len, int iterations, u8 *buf, size_t buflen)
{
	mbedtls_md_context_t sha1_ctx;
	const mbedtls_md_info_t *info_sha1 = NULL;
	int ret;
	if ((passphrase == NULL) || (ssid == NULL) || (buf == NULL) ||
		(ssid_len == 0) || (buflen == 0))
		return -1;
	mbedtls_md_init(&sha1_ctx);
	info_sha1 = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
	if (info_sha1 == NULL) {
		ret = -1;
		goto cleanup;
	}
	if ((ret = mbedtls_md_setup(&sha1_ctx, info_sha1, 1)) != 0) {
		ret = -1;
		goto cleanup;
	}

	if (mbedtls_pkcs5_pbkdf2_hmac(&sha1_ctx, (const unsigned char *)passphrase, os_strlen(passphrase),
		ssid, ssid_len, iterations, buflen, buf) != 0) {
		ret =  -1;
		goto cleanup;
	}
cleanup:
	mbedtls_md_free(&sha1_ctx);
	return ret;
}

int hmac_sha1_vector(const u8 *key, size_t key_len, size_t num_elem,
	const u8 *addr[], const size_t *len, u8 *mac)
{
	return mbedtls_hmac_vector(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), key ,key_len, num_elem, addr, len, mac);
}
int hmac_sha1(const u8 *key, size_t key_len, const u8 *data, size_t data_len, u8 *mac)
{
	return hmac_sha1_vector(key, key_len, 1, &data, &data_len, mac);
}

#ifdef CONFIG_SHA256
int hmac_sha256_vector(const u8 *key, size_t key_len, size_t num_elem,
	const u8 *addr[], const size_t *len, u8 *mac)
{
	return mbedtls_hmac_vector(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), key ,key_len, num_elem, addr, len, mac);
}
int hmac_sha256(const u8 *key, size_t key_len, const u8 *data, size_t data_len, u8 *mac)
{
	return hmac_sha256_vector(key, key_len, 1, &data, &data_len, mac);
}
#endif /* CONFIG_SHA256 */

#ifdef CONFIG_SHA384
int hmac_sha384_vector(const u8 *key, size_t key_len, size_t num_elem,
	const u8 *addr[], const size_t *len, u8 *mac)
{
	return mbedtls_hmac_vector(mbedtls_md_info_from_type(MBEDTLS_MD_SHA384), key ,key_len, num_elem, addr, len, mac);
}

int hmac_sha384(const u8 *key, size_t key_len, const u8 *data, size_t data_len, u8 *mac)
{
	return hmac_sha384_vector(key, key_len, 1, &data, &data_len, mac);
}
#endif /* CONFIG_SHA384 */

#ifdef CONFIG_SHA512
int hmac_sha512_vector(const u8 *key, size_t key_len, size_t num_elem,
	const u8 *addr[], const size_t *len, u8 *mac)
{
	return mbedtls_hmac_vector(mbedtls_md_info_from_type(MBEDTLS_MD_SHA512), key ,key_len, num_elem, addr, len, mac);
}

int hmac_sha512(const u8 *key, size_t key_len, const u8 *data, size_t data_len, u8 *mac)
{
	return hmac_sha512_vector(key, key_len, 1, &data, &data_len, mac);
}
#endif /* CONFIG_SHA512 */

static void get_group5_prime(mbedtls_mpi *p)
{
	static const unsigned char RFC3526_PRIME_1536[] = {
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC9,0x0F,0xDA,0xA2,
		0x21,0x68,0xC2,0x34,0xC4,0xC6,0x62,0x8B,0x80,0xDC,0x1C,0xD1,
		0x29,0x02,0x4E,0x08,0x8A,0x67,0xCC,0x74,0x02,0x0B,0xBE,0xA6,
		0x3B,0x13,0x9B,0x22,0x51,0x4A,0x08,0x79,0x8E,0x34,0x04,0xDD,
		0xEF,0x95,0x19,0xB3,0xCD,0x3A,0x43,0x1B,0x30,0x2B,0x0A,0x6D,
		0xF2,0x5F,0x14,0x37,0x4F,0xE1,0x35,0x6D,0x6D,0x51,0xC2,0x45,
		0xE4,0x85,0xB5,0x76,0x62,0x5E,0x7E,0xC6,0xF4,0x4C,0x42,0xE9,
		0xA6,0x37,0xED,0x6B,0x0B,0xFF,0x5C,0xB6,0xF4,0x06,0xB7,0xED,
		0xEE,0x38,0x6B,0xFB,0x5A,0x89,0x9F,0xA5,0xAE,0x9F,0x24,0x11,
		0x7C,0x4B,0x1F,0xE6,0x49,0x28,0x66,0x51,0xEC,0xE4,0x5B,0x3D,
		0xC2,0x00,0x7C,0xB8,0xA1,0x63,0xBF,0x05,0x98,0xDA,0x48,0x36,
		0x1C,0x55,0xD3,0x9A,0x69,0x16,0x3F,0xA8,0xFD,0x24,0xCF,0x5F,
		0x83,0x65,0x5D,0x23,0xDC,0xA3,0xAD,0x96,0x1C,0x62,0xF3,0x56,
		0x20,0x85,0x52,0xBB,0x9E,0xD5,0x29,0x07,0x70,0x96,0x96,0x6D,
		0x67,0x0C,0x35,0x4E,0x4A,0xBC,0x98,0x04,0xF1,0x74,0x6C,0x08,
		0xCA,0x23,0x73,0x27,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	};
	mbedtls_mpi_init(p);
	(void)mbedtls_mpi_read_binary(p, RFC3526_PRIME_1536, sizeof(RFC3526_PRIME_1536));
	return;
}

void *aes_encrypt_init(const u8 *key, size_t len)
{
	mbedtls_aes_context *ctx = NULL;

	if (key == NULL)
		return NULL;
	ctx = os_zalloc(sizeof(mbedtls_aes_context));
	if (ctx == NULL)
		return NULL;
	mbedtls_aes_init(ctx);
	if (mbedtls_aes_setkey_enc(ctx, key, (len * 8)) != 0) {
		os_free(ctx);
		ctx = NULL;
	}
	return ctx;
}

int aes_encrypt(void *ctx, const u8 *plain, u8 *crypt)
{
	if (ctx == NULL || plain == NULL || crypt == NULL)
		return -1;

	return (mbedtls_internal_aes_encrypt(ctx, plain, crypt) < 0) ? -1 : 0;
}

void aes_encrypt_deinit(void *ctx)
{
	if (ctx == NULL)
		return;
	mbedtls_aes_free(ctx);
	os_free(ctx);
}

void *aes_decrypt_init(const u8 *key, size_t len)
{
	mbedtls_aes_context *ctx = NULL;

	if (key == NULL)
		return NULL;

	ctx = os_zalloc(sizeof(mbedtls_aes_context));
	if (ctx == NULL)
		return NULL;
	mbedtls_aes_init(ctx);
	if (mbedtls_aes_setkey_dec(ctx, key, (len * 8)) != 0) {
		os_free(ctx);
		ctx = NULL;
	}

	return ctx;
}

int aes_decrypt(void *ctx, const u8 *crypt, u8 *plain)
{
	if (ctx == NULL || plain == NULL || crypt == NULL)
		return -1;

	return (mbedtls_internal_aes_decrypt(ctx, crypt, plain) < 0) ? -1 : 0;
}

void aes_decrypt_deinit(void *ctx)
{
	if (ctx == NULL)
		return;
	mbedtls_aes_free(ctx);
	os_free(ctx);
}

int aes_128_cbc_encrypt(const u8 *key, const u8 *iv, u8 *data, size_t data_len)
{
	mbedtls_aes_context ctx = { 0 };
	u8 temp_iv[16] = { 0 };  /* 16: iv length */
	int ret;

	if (key == NULL || iv == NULL || data == NULL)
		return -1;
	(void)os_memcpy(temp_iv, iv, 16);
	mbedtls_aes_init(&ctx);
	if (mbedtls_aes_setkey_enc(&ctx, key, AES_128_CRYPTO_LEN) != 0)
		return -1;

	ret = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, data_len, temp_iv, data, data);
	mbedtls_aes_free(&ctx);
	return ret;
}

int aes_128_cbc_decrypt(const u8 *key, const u8 *iv, u8 *data, size_t data_len)
{
	mbedtls_aes_context ctx = { 0 };
	u8 temp_iv[16] = { 0 };  /* 16: iv length */
	int ret;

	if (key == NULL || iv == NULL || data == NULL)
		return -1;
	(void)os_memcpy(temp_iv, iv, 16);
	mbedtls_aes_init(&ctx);
	if (mbedtls_aes_setkey_dec(&ctx, key, AES_128_CRYPTO_LEN) != 0)
		return -1;

	ret = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, data_len, temp_iv, data, data);
	mbedtls_aes_free(&ctx);
	return ret;
}

void *dh5_init(struct wpabuf **priv, struct wpabuf **publ)
{
	mbedtls_dhm_context *dh = NULL;
	struct wpabuf *pubkey = NULL;
	struct wpabuf *privkey = NULL;
	size_t publen, privlen;
	unsigned char *export = NULL;
	size_t exportlen;
	if ((priv == NULL) || (publ == NULL))
		return NULL;
	if (*priv != NULL) {
		wpabuf_free(*priv);
		*priv = NULL;
	}
	if (*publ != NULL) {
		wpabuf_free(*publ);
		*publ = NULL;
	}
	dh = os_zalloc(sizeof(*dh));
	if (dh == NULL)
		return NULL;

	mbedtls_dhm_init(dh);
	mbedtls_mpi_init(&dh->G);
	if (mbedtls_mpi_lset(&dh->G, DHM_PARM_G_LEN) != 0) {
		os_free(dh);
		return NULL;
	}
	get_group5_prime(&dh->P);

	export = os_zalloc(DHM_PARM_MEM);
	if (export == NULL)
		goto err;
	if (mbedtls_dhm_make_params(dh, (int)mbedtls_mpi_size(&dh->P), export, &exportlen, get_trng, NULL) != 0)
		goto err;

	os_free(export);
	export = NULL;
	publen = mbedtls_mpi_size((const mbedtls_mpi *)&(dh->GX));
	pubkey = wpabuf_alloc(publen);
	if (pubkey == NULL)
		goto err;

	privlen = mbedtls_mpi_size((const mbedtls_mpi *)&dh->X);
	privkey = wpabuf_alloc(privlen);
	if (privkey == NULL)
		goto err;

	mbedtls_mpi_write_binary((const mbedtls_mpi *)&dh->GX, wpabuf_put(pubkey, publen), publen);
	mbedtls_mpi_write_binary((const mbedtls_mpi *)&dh->X, wpabuf_put(privkey, privlen), privlen);
	*priv = privkey;
	*publ = pubkey;
	return dh;
err:
	wpabuf_clear_free(pubkey);
	wpabuf_clear_free(privkey);
	mbedtls_dhm_free(dh);
	os_free(export);
	os_free(dh);
	return NULL;
}

void * dh5_init_fixed(const struct wpabuf *priv, const struct wpabuf *publ)
{
	mbedtls_dhm_context *dh = NULL;
	unsigned char *export = NULL;
	size_t exportlen;
	struct wpabuf *pubkey = NULL;
	struct wpabuf *privkey = NULL;
	size_t publen, privlen;
	if ((priv == NULL) || (publ == NULL))
		return NULL;

	dh = os_zalloc(sizeof(*dh));
	if (dh == NULL)
		return NULL;

	mbedtls_dhm_init(dh);
	mbedtls_mpi_init(&dh->G);
	if (mbedtls_mpi_lset(&dh->G, DHM_PARM_G_LEN) != 0) {
		os_free(dh);
		return NULL;
	}
	get_group5_prime(&dh->P);

	if (mbedtls_mpi_read_binary(&dh->X, wpabuf_head(priv), wpabuf_len(priv)) != 0)
		goto err;

	if (mbedtls_mpi_read_binary(&dh->GX, wpabuf_head(publ), wpabuf_len(publ)) != 0)
		goto err;

	export = os_zalloc(DHM_PARM_MEM);
	if (export == NULL)
		goto err;
	if (mbedtls_dhm_make_params(dh, (int)mbedtls_mpi_size(&dh->P), export, &exportlen, get_trng, NULL) != 0)
		goto err;

	os_free(export);
	export = NULL;
	publen = mbedtls_mpi_size((const mbedtls_mpi *)&(dh->GX));
	pubkey = wpabuf_alloc(publen);
	if (pubkey == NULL)
		goto err;

	privlen = mbedtls_mpi_size((const mbedtls_mpi *)&dh->X);
	privkey = wpabuf_alloc(privlen);
	if (privkey == NULL)
		goto err;

	mbedtls_mpi_write_binary((const mbedtls_mpi *)&dh->GX, wpabuf_put(pubkey, publen), publen);
	mbedtls_mpi_write_binary((const mbedtls_mpi *)&dh->X, wpabuf_put(privkey, privlen), privlen);
	wpabuf_clear_free(pubkey);
	wpabuf_clear_free(privkey);
	return dh;
err:
	wpabuf_clear_free(pubkey);
	wpabuf_clear_free(privkey);
	mbedtls_dhm_free(dh);
	os_free(export);
	os_free(dh);
	return NULL;
}

struct wpabuf * dh5_derive_shared(void *ctx, const struct wpabuf *peer_public, const struct wpabuf *own_private)
{
	struct wpabuf *res = NULL;
	size_t rlen;
	mbedtls_dhm_context *dh = ctx;
	size_t keylen;
	(void)own_private;
	if ((ctx == NULL) || (peer_public == NULL))
		return NULL;
	const u8 *p = wpabuf_head(peer_public);
	if (p == NULL)
		return NULL;

	if (mbedtls_mpi_read_binary(&dh->GY, p, wpabuf_len(peer_public)) != 0)
		goto err;

	rlen = mbedtls_mpi_size((const mbedtls_mpi *)&(dh->P));
	res = wpabuf_alloc(rlen);
	if (res == NULL)
		goto err;

	if (mbedtls_dhm_calc_secret(dh, wpabuf_mhead(res), rlen, &keylen, get_trng, NULL) != 0)
		goto err;

	wpabuf_put(res, keylen);
	mbedtls_mpi_free(&dh->GY);
	return res;
err:
	mbedtls_mpi_free(&dh->GY);
	wpabuf_clear_free(res);
	return NULL;
}

void dh5_free(void *ctx)
{
	mbedtls_dhm_context *dh = NULL;
	if (ctx == NULL)
		return;
	dh = ctx;
	mbedtls_dhm_free(dh);
	os_free(dh);
}


struct crypto_bignum *crypto_bignum_init(void)
{
	mbedtls_mpi *p = NULL;
	p = os_zalloc(sizeof(*p));
	if (p == NULL)
		return NULL;

	mbedtls_mpi_init(p);
	return p;
}

struct crypto_bignum *crypto_bignum_init_set(const u8 *buf, size_t len)
{
	int ret;
	mbedtls_mpi *p = NULL;

	if (buf == NULL || len == 0)
		return NULL;

	p = crypto_bignum_init();
	if (p == NULL)
		return NULL;

	ret = mbedtls_mpi_read_binary(p, buf, len);
	if (ret != 0) {
		crypto_bignum_deinit(p, 1);
		p = NULL;
	}
	return p;
}

struct crypto_bignum * crypto_bignum_init_uint(unsigned int val)
{
	unsigned int i;
	unsigned int tmp = val;
	u8 buf[sizeof(unsigned int)] = {0};
	struct crypto_bignum *p = NULL;

	p = crypto_bignum_init();
	if (p == NULL)
		return NULL;

	for (i = 0; i < sizeof(unsigned int); i++) {
		buf[i] = tmp & 0xFF;
		tmp >>= 8; // 8 bits
	}
	if (mbedtls_mpi_read_binary_le(p, buf, sizeof(unsigned int)) != 0) {
		crypto_bignum_deinit(p, 1);
		p = NULL;
	}

	return p;
}

void crypto_bignum_deinit(struct crypto_bignum *n, int clear)
{
	(void)clear;
	if (n == NULL)
		return;

	mbedtls_mpi_free(n);
	os_free(n);
}

int crypto_bignum_to_bin(const struct crypto_bignum *a, u8 *buf, size_t buflen, size_t padlen)
{
	int ret;
	size_t num_bytes;
	size_t offset;

	if (a == NULL || buf == NULL || padlen > buflen)
		return -1;

	num_bytes = mbedtls_mpi_size((const mbedtls_mpi *)a);
	if (num_bytes > buflen)
		return -1;

	if (padlen > num_bytes)
		offset = padlen - num_bytes;
	else
		offset = 0;

	(void)os_memset(buf, 0, offset);
	ret = mbedtls_mpi_write_binary((const mbedtls_mpi *)a, buf + offset, num_bytes);
	if (ret)
		return -1;

	return num_bytes + offset;
}

int crypto_bignum_rand(struct crypto_bignum *r, const struct crypto_bignum *m)
{
	int count = 0;
	unsigned cmp = 0;
	size_t n_size;
	if ((m == NULL) || (r == NULL))
		return -1;
	n_size = mbedtls_mpi_size(m);
	if (n_size == 0)
		return -1;
	do {
		if (mbedtls_mpi_fill_random(r, n_size, get_trng, NULL))
			return -1;

		if (mbedtls_mpi_shift_r(r, 8 * n_size - mbedtls_mpi_bitlen(m)))
			return -1;

		if (++count > 30)
			return -1;

		if (mbedtls_mpi_lt_mpi_ct(r, m, &cmp))
			return -1;
	} while (mbedtls_mpi_cmp_int(r, 1 ) < 0 || cmp != 1);

	return 0;
}

int crypto_bignum_add(const struct crypto_bignum *a, const struct crypto_bignum *b, struct crypto_bignum *c)
{
	if (a == NULL || b == NULL || c == NULL)
		return -1;
	return mbedtls_mpi_add_mpi((mbedtls_mpi *)c, (const mbedtls_mpi *)a, (const mbedtls_mpi *)b);
}

int crypto_bignum_mod(const struct crypto_bignum *a, const struct crypto_bignum *b, struct crypto_bignum *c)
{
	if (a == NULL || b == NULL || c == NULL)
		return -1;
	return mbedtls_mpi_mod_mpi((mbedtls_mpi *)c, (const mbedtls_mpi *)a, (const mbedtls_mpi *)b);
}

int crypto_bignum_exptmod(const struct crypto_bignum *a, const struct crypto_bignum *b,
                          const struct crypto_bignum *c, struct crypto_bignum *d)
{
	if (a == NULL || b == NULL || c == NULL || d == NULL)
		return -1;
	/* It takes 2.7 seconds for two basic boards to interact at one time.
	   If 10 basic boards interact at the same time, the watchdog cannot be feeded in time,
	   resulting in system abnormality. */
#ifdef SUPPORT_WATCHDOG
	uapi_watchdog_kick();
#endif
	return mbedtls_mpi_exp_mod((mbedtls_mpi *)d, (const mbedtls_mpi *)a,
	                           (const mbedtls_mpi *)b, (const mbedtls_mpi *)c,
	                           NULL);
}

int crypto_bignum_inverse(const struct crypto_bignum *a, const struct crypto_bignum *b, struct crypto_bignum *c)
{
	if (a == NULL || b == NULL || c == NULL)
		return -1;
	return mbedtls_mpi_inv_mod((mbedtls_mpi *)c, (const mbedtls_mpi *)a, (const mbedtls_mpi *)b);
}

int crypto_bignum_sub(const struct crypto_bignum *a, const struct crypto_bignum *b, struct crypto_bignum *c)
{
	if (a == NULL || b == NULL || c == NULL)
		return -1;
	return mbedtls_mpi_sub_mpi((mbedtls_mpi *)c, (const mbedtls_mpi *)a, (const mbedtls_mpi *)b);
}

int crypto_bignum_div(const struct crypto_bignum *a, const struct crypto_bignum *b, struct crypto_bignum *c)
{
	if (a == NULL || b == NULL || c == NULL)
		return -1;
	return mbedtls_mpi_div_mpi((mbedtls_mpi *)c, NULL, (const mbedtls_mpi *)a, (const mbedtls_mpi *)b);
}

int crypto_bignum_addmod(const struct crypto_bignum *a, const struct crypto_bignum *b,
                         const struct crypto_bignum *c, struct crypto_bignum *d)
{
	int ret;
	mbedtls_mpi mul;
	if (a == NULL || b == NULL || c == NULL || d == NULL)
		return -1;
	mbedtls_mpi_init(&mul);
	ret = mbedtls_mpi_add_mpi(&mul, (const mbedtls_mpi *)a, (const mbedtls_mpi *)b);
	if (ret == 0)
		ret = mbedtls_mpi_mod_mpi((mbedtls_mpi *)d, &mul, (const mbedtls_mpi *)c);

	mbedtls_mpi_free(&mul);
	return (ret == 0) ? 0 : -1;
}

int crypto_bignum_mulmod(const struct crypto_bignum *a, const struct crypto_bignum *b,
                         const struct crypto_bignum *c, struct crypto_bignum *d)
{
	int ret;
	mbedtls_mpi mul;
	if (a == NULL || b == NULL || c == NULL || d == NULL)
		return -1;
	mbedtls_mpi_init(&mul);
	ret = mbedtls_mpi_mul_mpi(&mul, (const mbedtls_mpi *)a, (const mbedtls_mpi *)b);
	if (ret == 0)
		ret = mbedtls_mpi_mod_mpi((mbedtls_mpi *)d, &mul, (const mbedtls_mpi *)c);

	mbedtls_mpi_free(&mul);
	return (ret == 0) ? 0 : -1;
}

int crypto_bignum_sqrmod(const struct crypto_bignum *a,
			 const struct crypto_bignum *b,
			 struct crypto_bignum *c)
{
	if ((a == NULL) || (b == NULL) || (c == NULL))
		return -1;
	return crypto_bignum_mulmod(a, a, b, c);
}

int crypto_bignum_rshift(const struct crypto_bignum *a, int n, struct crypto_bignum *r)
{
	int ret;
	if ((a == NULL) || (r == NULL) || (n <= 0))
		return -1;
	ret = mbedtls_mpi_copy((mbedtls_mpi *)r, (const mbedtls_mpi *)a);
	if (ret == 0)
		ret = mbedtls_mpi_shift_r((mbedtls_mpi *)r, n);

	return ret;
}

int crypto_bignum_cmp(const struct crypto_bignum *a, const struct crypto_bignum *b)
{
	if ((a == NULL) || (b == NULL)) {
		if (a != NULL)
			return -1;
		else if (b != NULL)
			return 1;
		else
			return 0;
	}
	return mbedtls_mpi_cmp_mpi((const mbedtls_mpi *)a, (const mbedtls_mpi *)b);
}

int crypto_bignum_bits(const struct crypto_bignum *a)
{
	if (a == NULL)
		return -1;
	return mbedtls_mpi_bitlen((const mbedtls_mpi *)a);
}

int crypto_bignum_is_zero(const struct crypto_bignum *a)
{
	if (a == NULL)
		return 0;
	return (mbedtls_mpi_cmp_int((const mbedtls_mpi *)a, 0) == 0) ? 1 : 0;
}

int crypto_bignum_is_one(const struct crypto_bignum *a)
{
	if (a == NULL)
		return 0;
	return (mbedtls_mpi_cmp_int((const mbedtls_mpi *)a, 1) == 0) ? 1 : 0;
}

int crypto_bignum_is_odd(const struct crypto_bignum *a)
{
	if (a == NULL)
		return 0;
	return ((a != NULL) && (a->p != NULL) && (a->n > 0) && (a->p[0] & 0x01)) ? 1 : 0;
}

int crypto_bignum_legendre(const struct crypto_bignum *a, const struct crypto_bignum *p)
{
	int ret;
	int res = -2;
	unsigned int mask;
	mbedtls_mpi t;
	mbedtls_mpi exp;

	if (a == NULL || p == NULL)
		return res;

	mbedtls_mpi_init(&t);
	mbedtls_mpi_init(&exp);

	/* exp = (p-1) / 2 */
	ret = mbedtls_mpi_sub_int(&exp, (const mbedtls_mpi *)p, 1);
	if (ret == 0)
		ret = mbedtls_mpi_shift_r(&exp, 1);

	if (ret == 0)
		ret = crypto_bignum_exptmod(a, (const struct crypto_bignum *)&exp,
									p, (struct crypto_bignum *)&t);

	if (ret == 0) {
		/* Return 1 if tmp == 1, 0 if tmp == 0, or -1 otherwise. Need to use
		 * constant time selection to avoid branches here. */
		res = -1;
		mask = const_time_eq(crypto_bignum_is_one((const struct crypto_bignum *)&t), 1);
		res = const_time_select_int(mask, 1, res);
		mask = const_time_eq(crypto_bignum_is_zero((const struct crypto_bignum *)&t), 1);
		res = const_time_select_int(mask, 0, res);
	}

	mbedtls_mpi_free(&exp);
	mbedtls_mpi_free(&t);
	return res;
}

struct crypto_ec *crypto_ec_init(int group)
{
	mbedtls_ecp_group_id id;

	/* convert IANA ECC group ID to mbedtls ECC group ID */
	switch (group) {
		case 19:
			id = MBEDTLS_ECP_DP_SECP256R1; /* for SAE */
			break;
		case 28:
			id = MBEDTLS_ECP_DP_BP256R1; /* for MESH */
			break;
		default:
			return NULL;
	}

	struct crypto_ec *ec = os_zalloc(sizeof(struct crypto_ec));
	if (ec == NULL)
		return NULL;

	mbedtls_ecp_group *e = &ec->ecp;
	mbedtls_ecp_group_init(e);
	if (mbedtls_ecp_group_load(e, id)) {
		crypto_ec_deinit(ec);
		return NULL;
	}

	int ret;
	mbedtls_mpi_init(&ec->a);
	if (e->A.p == NULL)
		ret = mbedtls_mpi_sub_int(&ec->a, &e->P, 3);  /* 3: in mbedtls A = p - 3 */
	else
		ret = mbedtls_mpi_copy(&ec->a, &e->A);

	if (ret != 0) {
		crypto_ec_deinit(ec);
		return NULL;
	}
	return ec;
}

void crypto_ec_deinit(struct crypto_ec *ec)
{
	if (ec != NULL) {
		mbedtls_ecp_group_free(&ec->ecp);
		mbedtls_mpi_free(&ec->a);
		os_free(ec);
	}
}

struct crypto_ec_point *crypto_ec_point_init(struct crypto_ec *ec)
{
	if (ec == NULL)
		return NULL;

	crypto_ec_point *p = os_zalloc(sizeof(*p));
	if (p == NULL)
		return NULL;

	mbedtls_ecp_point_init(p);
	if (mbedtls_mpi_lset(&p->Z, 1)) { // affine coordinate
		crypto_ec_point_deinit(p, 1);
		return NULL;
	}

	return p;
}

void crypto_ec_point_deinit(struct crypto_ec_point *e, int clear)
{
	(void)clear;
	if (e == NULL)
		return;

	mbedtls_ecp_point_free(e);
	os_free(e);
}

size_t crypto_ec_prime_len(struct crypto_ec *ec)
{
	if (ec == NULL)
		return 0;
	mbedtls_ecp_group *e = &ec->ecp;
	return mbedtls_mpi_size(&e->P);
}

size_t crypto_ec_prime_len_bits(struct crypto_ec *ec)
{
	if (ec == NULL)
		return 0;
	mbedtls_ecp_group *e = &ec->ecp;
	return mbedtls_mpi_bitlen(&e->P);
}

size_t crypto_ec_order_len(struct crypto_ec *ec)
{
	if (ec == NULL)
		return 0;
	mbedtls_ecp_group *e = &ec->ecp;
	return mbedtls_mpi_size(&e->N);
}

const struct crypto_bignum *crypto_ec_get_prime(struct crypto_ec *ec)
{
	if (ec == NULL)
		return NULL;
	mbedtls_ecp_group *e = &ec->ecp;
	return (const struct crypto_bignum *)&e->P;
}

const struct crypto_bignum *crypto_ec_get_order(struct crypto_ec *ec)
{
	if (ec == NULL)
		return NULL;
	mbedtls_ecp_group *e = &ec->ecp;
	return (const struct crypto_bignum *)&e->N;
}

/* only for Weierstrass curves */
const struct crypto_bignum *crypto_ec_get_a(struct crypto_ec *ec)
{
	if (ec == NULL)
		return NULL;

	return (const struct crypto_bignum *)&ec->a; // a is member of ec.
}

/* only for Weierstrass curves */
const struct crypto_bignum *crypto_ec_get_b(struct crypto_ec *ec)
{
	if (ec == NULL)
		return NULL;
	mbedtls_ecp_group *e = &ec->ecp;
	return (const struct crypto_bignum *) &e->B;
}

int crypto_ec_point_to_bin(struct crypto_ec *ec, const struct crypto_ec_point *point, u8 *x, u8 *y)
{
	int ret = -1;
	size_t len;
	if ((ec == NULL) || (point == NULL))
		return -1;
	mbedtls_ecp_group *e = &ec->ecp;
	len = mbedtls_mpi_size(&e->P);
	if (x != NULL) {
		ret = mbedtls_mpi_write_binary(&(((const mbedtls_ecp_point *)point)->X), x, len);
		if (ret)
			return ret;
	}
	if (y != NULL) {
		ret = mbedtls_mpi_write_binary(&(((const mbedtls_ecp_point *)point)->Y), y, len);
		if (ret)
			return ret;
	}

	return ret;
}

struct crypto_ec_point *crypto_ec_point_from_bin(struct crypto_ec *ec, const u8 *val)
{
	int ret;
	size_t len;
	if (ec == NULL || val == NULL)
		return NULL;

	crypto_ec_point *p = crypto_ec_point_init(ec);
	if (p == NULL)
		return NULL;

	mbedtls_ecp_group *e = &ec->ecp;
	len = mbedtls_mpi_size(&e->P);
	ret = mbedtls_mpi_read_binary(&p->X, val, len);
	if (ret == 0)
		ret = mbedtls_mpi_read_binary(&p->Y, val + len, len);

	if (ret) {
		mbedtls_ecp_point_free(p);
		os_free(p);
		return NULL;
	}
	return p;
}

int crypto_ec_point_add(struct crypto_ec *ec, const struct crypto_ec_point *a,
                        const struct crypto_ec_point *b, struct crypto_ec_point *c)
{
	int ret;
	if (ec == NULL || a == NULL || b == NULL || c == NULL)
		return -1;
	mbedtls_ecp_group *e = &ec->ecp;
#ifdef SUPPORT_WATCHDOG
	uapi_watchdog_kick();
#endif
	crypto_bignum one;
	mbedtls_mpi_init(&one);
	ret = mbedtls_mpi_lset(&one, 1);
	if (ret == 0)
		ret = mbedtls_ecp_muladd(e, c, &one, a, &one, b);

	mbedtls_mpi_free(&one);
	return ret;
}

/*
 * Multiplication res = b * p
 */
int crypto_ec_point_mul(struct crypto_ec *ec, const struct crypto_ec_point *p,
                        const struct crypto_bignum *b,
                        struct crypto_ec_point *res)
{
	if (ec == NULL || p == NULL || b == NULL || res == NULL)
		return -1;
	mbedtls_ecp_group *e = &ec->ecp;
#ifdef SUPPORT_WATCHDOG
	uapi_watchdog_kick();
#endif
	return mbedtls_ecp_mul(e, res, b, p, get_trng, NULL) ? -1 : 0;
}

int crypto_ec_point_invert(struct crypto_ec *ec, struct crypto_ec_point *p)
{
	if (ec == NULL || p == NULL)
		return -1;
	mbedtls_ecp_group *e = &ec->ecp;
	return mbedtls_mpi_sub_mpi(&p->Y, &e->P, &p->Y);
}

// y_bit (first byte of compressed point) mod 2 odd : r = p - r
int crypto_ec_point_solve_y_coord(struct crypto_ec *ec,
                                  struct crypto_ec_point *p,
                                  const struct crypto_bignum *x, int y_bit)
{
	int ret;
	mbedtls_mpi n;
	if (ec == NULL || p == NULL || x == NULL)
		return -1;
	// Calculate square root of r over finite field P:
	// r = sqrt(x^3 + ax + b) = (x^3 + ax + b) ^ ((P + 1) / 4) (mod P)
	struct crypto_bignum *y_sqr = crypto_ec_point_compute_y_sqr(ec, x);
	if (y_sqr == NULL)
		return -1;

	mbedtls_ecp_group *e = &ec->ecp;
	mbedtls_mpi_init(&n);

	ret = mbedtls_mpi_add_int(&n, &e->P, 1);
	if (ret == 0)
		ret = mbedtls_mpi_shift_r(&n, 2);

	if (ret == 0)
		ret = mbedtls_mpi_exp_mod(y_sqr, y_sqr, &n, &e->P, NULL);

	// If LSB(Y) != y_bit, then do Y = P - Y
	if ((y_bit != crypto_bignum_is_odd(y_sqr)) && (ret == 0))
		ret = mbedtls_mpi_sub_mpi(y_sqr, &e->P, y_sqr); // r = p - r

	if (ret == 0) {
		if ((mbedtls_mpi_copy(&p->X, x) != 0) ||
		    (mbedtls_mpi_copy(&p->Y, y_sqr) != 0))
			ret = -1;
		else
			ret = 0;
	}

	mbedtls_mpi_free(&n);
	crypto_bignum_deinit(y_sqr, 1);

	return ret;
}

struct crypto_bignum *crypto_ec_point_compute_y_sqr(struct crypto_ec *ec, const struct crypto_bignum *x)
{
	int ret;
	if (ec == NULL || x == NULL)
		return NULL;
	mbedtls_ecp_group *e = &ec->ecp;
	struct crypto_bignum *y2 = crypto_bignum_init();
	if (y2 == NULL)
		return NULL;

	ret = mbedtls_mpi_mul_mpi(y2, x, x);
	if (ret == 0)
		ret = mbedtls_mpi_mod_mpi(y2, y2, &e->P);

	if (ret == 0) {
		if (e->A.p == NULL)
			ret = mbedtls_mpi_sub_int(y2, y2, 3); // Special case where a is -3
		else
			ret = mbedtls_mpi_add_mpi(y2, y2, &e->A);

		if (ret == 0)
			ret = mbedtls_mpi_mod_mpi(y2, y2, &e->P);
	}
	if (ret == 0)
		ret = mbedtls_mpi_mul_mpi(y2, y2, x);

	if (ret == 0)
		ret = mbedtls_mpi_mod_mpi(y2, y2, &e->P);

	if (ret == 0)
		ret = mbedtls_mpi_add_mpi(y2, y2, &e->B);

	if (ret == 0)
		ret = mbedtls_mpi_mod_mpi(y2, y2, &e->P);

	if (ret) {
		crypto_bignum_deinit(y2, 1);
		return NULL;
	}

	return y2;
}

int crypto_ec_point_is_at_infinity(struct crypto_ec *ec, const struct crypto_ec_point *p)
{
	(void)ec;
	if (p == NULL)
		return 0;
	return mbedtls_ecp_is_zero((struct crypto_ec_point *)p);
}

int crypto_ec_point_is_on_curve(struct crypto_ec *ec, const struct crypto_ec_point *p)
{
	if (ec == NULL || p == NULL)
		return 0;
	mbedtls_ecp_group *e = &ec->ecp;
	return mbedtls_ecp_check_pubkey(e, p) ? 0 : 1;
}

int crypto_ec_point_cmp(const struct crypto_ec *ec,
                        const struct crypto_ec_point *a,
                        const struct crypto_ec_point *b)
{
	(void)ec;
	if (a == NULL || b == NULL)
		return -1;
	return mbedtls_ecp_point_cmp(a, b);
}

#if defined(CONFIG_ECC) && defined(MBEDTLS_ECP_RESTARTABLE)

static mbedtls_pk_context *crypto_alloc_key(void)
{
	mbedtls_pk_context *key = os_malloc(sizeof(*key));

	if (!key) {
		wpa_printf(MSG_ERROR, "%s: memory allocation failed\n", __func__);
		return NULL;
	}
	mbedtls_pk_init(key);

	return key;
}

void crypto_ec_free_key(mbedtls_pk_context *key)
{
	mbedtls_pk_free(key);
	os_free(key);
}

struct mbedtls_pk_context * crypto_ec_set_pubkey_point(const mbedtls_ecp_group *group,
		const u8 *buf, size_t len)
{
	mbedtls_ecp_point *point = NULL;
	int ret;
	mbedtls_pk_context *key = (mbedtls_pk_context *)crypto_alloc_key();

	if (!key) {
		wpa_printf(MSG_ERROR, "%s: memory allocation failed", __func__);
		return NULL;
	}

	point = (mbedtls_ecp_point *)crypto_ec_point_from_bin((struct crypto_ec *)group, buf);
	if (!point) {
		wpa_printf(MSG_ERROR, "%s: Point initialization failed", __func__);
		goto fail;
	}
	if (crypto_ec_point_is_at_infinity((struct crypto_ec *)group, (struct crypto_ec_point *)point)) {
		wpa_printf(MSG_ERROR, "Point is at infinity");
		goto fail;
	}
	if (!crypto_ec_point_is_on_curve((struct crypto_ec *)group, (struct crypto_ec_point *)point)) {
		wpa_printf(MSG_ERROR, "Point not on curve");
		goto fail;
	}

	if (mbedtls_ecp_check_pubkey((mbedtls_ecp_group *)group, point) < 0) { //typecast
		// ideally should have failed in upper condition, duplicate code??
		wpa_printf(MSG_ERROR, "Invalid key");
		goto fail;
	}
	/* Assign values */
	if( ( ret = mbedtls_pk_setup( key, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY) ) ) != 0 )
		goto fail;
	mbedtls_ecp_copy(&mbedtls_pk_ec(*key)->MBEDTLS_PRIVATE(Q), point);
	mbedtls_ecp_group_load(&mbedtls_pk_ec(*key)->MBEDTLS_PRIVATE(grp), MBEDTLS_ECP_DP_SECP256R1);

	crypto_ec_point_deinit((struct crypto_ec_point *)point, 0);
	return key;
fail:
	if (point)
		crypto_ec_point_deinit((struct crypto_ec_point *)point, 0);
	if (key)
		crypto_ec_free_key(key);
	return NULL;
}

void crypto_ecdh_deinit(struct crypto_ecdh *ecdh)
{
	mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)ecdh;
	if (!ctx) {
		return;
	}
	mbedtls_ecdh_free(ctx);
	os_free(ctx);
	ctx = NULL;
}

int crypto_mbedtls_get_grp_id(int group)
{
	switch(group) {
		case 19:
			return MBEDTLS_ECP_DP_SECP256R1;
		case 20:
			return MBEDTLS_ECP_DP_SECP384R1;
		case 21:
			return MBEDTLS_ECP_DP_SECP521R1;
		default:
			return MBEDTLS_ECP_DP_NONE;
	}
}

struct crypto_ecdh * crypto_ecdh_init(int group)
{
	mbedtls_ecdh_context *ctx;

	ctx = os_zalloc(sizeof(*ctx));
	if (!ctx) {
		wpa_printf(MSG_ERROR, "Memory allocation failed for ecdh context");
		goto fail;
	}
	mbedtls_ecdh_init(ctx);

	if ((mbedtls_ecp_group_load(&ctx->MBEDTLS_PRIVATE(grp), crypto_mbedtls_get_grp_id(group))) != 0) {
		wpa_printf(MSG_ERROR, "Failed to set up ECDH context with group info");
		goto fail;
	}

	/* Generates ECDH keypair on elliptic curve */
	if (mbedtls_ecdh_gen_public(&ctx->MBEDTLS_PRIVATE(grp), &ctx->MBEDTLS_PRIVATE(d),
		&ctx->MBEDTLS_PRIVATE(Q), get_trng, NULL) !=0) {
		wpa_printf(MSG_ERROR, "ECDH keypair on curve failed");
		goto fail;
	}

	return (struct crypto_ecdh *)ctx;
fail:
	if (ctx) {
		mbedtls_ecdh_free(ctx);
		os_free(ctx);
		ctx = NULL;
	}
	return NULL;
}

struct wpabuf * crypto_ecdh_get_pubkey(struct crypto_ecdh *ecdh, int y)
{
	struct wpabuf *public_key = NULL;
	uint8_t *buf = NULL;
	mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)ecdh;
	size_t prime_len = ctx->MBEDTLS_PRIVATE(grp).pbits/8;

	buf = os_zalloc(y ? prime_len : 2 * prime_len);
	if (!buf) {
		wpa_printf(MSG_ERROR, "Memory allocation failed");
		return NULL;
	}

	/* Export an MPI into unsigned big endian binary data of fixed size */
	mbedtls_mpi_write_binary(&ctx->MBEDTLS_PRIVATE(Q).X, buf, prime_len);
	public_key = wpabuf_alloc_copy(buf, 32);
	os_free(buf);
	return public_key;
}

struct wpabuf * crypto_ecdh_set_peerkey(struct crypto_ecdh *ecdh, int inc_y, const u8 *key, size_t len)
{
	uint8_t *secret = 0;
	size_t olen = 0, len_prime = 0;
	struct crypto_bignum *bn_x = NULL;
	struct crypto_ec_point *ec_pt = NULL;
	uint8_t *px = NULL, *py = NULL, *buf = NULL;
	mbedtls_pk_context *peer = NULL;
	struct wpabuf *sh_secret = NULL;
	int secret_key = 0;

	mbedtls_ecdh_context *ctx = (mbedtls_ecdh_context *)ecdh;

	len_prime = ctx->MBEDTLS_PRIVATE(grp).pbits/8;
	bn_x = crypto_bignum_init_set(key, len);

	/* Initialize data for EC point */
	ec_pt = crypto_ec_point_init((struct crypto_ec*)&ctx->MBEDTLS_PRIVATE(grp));
	if (!ec_pt) {
		wpa_printf(MSG_ERROR,"Initializing for EC point failed");
		goto cleanup;
	}

	if (crypto_ec_point_solve_y_coord((struct crypto_ec*)&ctx->MBEDTLS_PRIVATE(grp), ec_pt, bn_x, inc_y) != 0) {
		wpa_printf(MSG_ERROR,"Failed to solve for y coordinate");
		goto cleanup;
	}
	px = os_zalloc(len);
	py = os_zalloc(len);
	buf = os_zalloc(2*len);

	if (!px || !py || !buf) {
		wpa_printf(MSG_ERROR, "Memory allocation failed");
		goto cleanup;
	}
	if (crypto_ec_point_to_bin((struct crypto_ec*)&ctx->MBEDTLS_PRIVATE(grp), ec_pt, px, py) != 0) {
		wpa_printf(MSG_ERROR,"Failed to write EC point value as binary data");
		goto cleanup;
	}

	os_memcpy(buf, px, len);
	os_memcpy(buf+len, py, len);

	peer = crypto_ec_set_pubkey_point((mbedtls_ecp_group *)&ctx->MBEDTLS_PRIVATE(grp), buf, len);
	if (!peer) {
		wpa_printf(MSG_ERROR, "Failed to set point for peer's public key");
		goto cleanup;
	}

	/* Setup ECDH context from EC key */
	/* Call to mbedtls_ecdh_get_params() will initialize the context when not LEGACY context */
	if (ctx != NULL && peer != NULL) {
		mbedtls_ecp_copy(&ctx->MBEDTLS_PRIVATE(Qp), &(mbedtls_pk_ec(*peer))->MBEDTLS_PRIVATE(Q));
	} else {
		wpa_printf(MSG_ERROR, "Failed to set peer's ECDH context");
		goto cleanup;
	}
	int len_secret = inc_y ? 2*len : len;
	secret = os_zalloc(len_secret);
	if (!secret) {
		wpa_printf(MSG_ERROR, "Allocation failed for secret");
		goto cleanup;
	}

	/* Calculate secret
	z = F(DH(x,Y)) */
	secret_key = mbedtls_ecdh_calc_secret(ctx, &olen, secret, len_prime, get_trng, NULL);
	if (secret_key != 0) {
		wpa_printf(MSG_ERROR, "Calculation of secret failed");
		goto cleanup;
	}
	sh_secret = wpabuf_alloc_copy(secret, len_secret);

cleanup:
	os_free(px);
	os_free(py);
	os_free(buf);
	os_free(secret);
	crypto_ec_free_key(peer);
	crypto_bignum_deinit(bn_x, 1);
	crypto_ec_point_deinit(ec_pt, 1);
	return sh_secret;
}

#endif /* defined(CONFIG_ECC) && defined(MBEDTLS_ECP_RESTARTABLE) */

