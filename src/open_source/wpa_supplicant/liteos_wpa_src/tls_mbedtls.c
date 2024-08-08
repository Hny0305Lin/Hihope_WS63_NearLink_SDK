/*
 * SSL/TLS interface functions for mbedtls
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "library/common.h"
#include "includes.h"
#include "common.h"
#include "tls.h"
#include "mbedtls/ssl.h"
#include "mbedtls/ssl_ciphersuites.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/debug.h"
#include "mbedtls/version.h"

#define TLS_KEY_RANDOM_LEN  32

typedef struct eap_tls_keys {
	unsigned char         master_secret[48];
	unsigned char         randbytes[TLS_KEY_RANDOM_LEN * 2];
	mbedtls_tls_prf_types tls_prf_type;
} eap_tls_keys;

struct tls_connection {
	mbedtls_ssl_context      *ssl;
	mbedtls_ssl_config       *conf;
	mbedtls_ctr_drbg_context *ctr_drbg;
	mbedtls_entropy_context  *entropy;
	mbedtls_x509_crt         *ca_cert;
	mbedtls_x509_crt         *cli_cert;
	mbedtls_pk_context       *pk;
	const unsigned char      *in_buf;
	size_t                    in_buf_len;

	struct wpabuf            *out_buf;
	int                       failed;
	int                       status;
	unsigned int              client_hello_generated:1;
	unsigned int              server:1;
	eap_tls_keys              eap_tls_keying;
};

static ext_wifi_ent_import_callback g_tls_import_cb = { NULL };

#if (MBEDTLS_VERSION_NUMBER < 0x03000000)
static int tls_key_derivation(void *p_expkey,
                              const unsigned char *ms,
                              const unsigned char *kb,
                              size_t maclen,
                              size_t keylen,
                              size_t ivlen,
                              const unsigned char client_random[TLS_KEY_RANDOM_LEN],
                              const unsigned char server_random[TLS_KEY_RANDOM_LEN],
                              mbedtls_tls_prf_types tls_prf_type)
{
	if ((p_expkey == NULL) || (ms == NULL))
		return -1;
	eap_tls_keys *keys = (eap_tls_keys *)p_expkey;

	(void)kb;
	(void)maclen;
	(void)keylen;
	(void)ivlen;

	(void)os_memcpy(keys->master_secret, ms, sizeof(keys->master_secret));
	(void)os_memcpy(keys->randbytes, client_random, TLS_KEY_RANDOM_LEN);
	(void)os_memcpy(keys->randbytes + TLS_KEY_RANDOM_LEN, server_random, TLS_KEY_RANDOM_LEN);
	keys->tls_prf_type = tls_prf_type;

	return 0;
}
#else
static void tls_key_derivation(void *p_expkey,
                               mbedtls_ssl_key_export_type secret_type,
                               const unsigned char *secret,
                               size_t secret_len,
                               const unsigned char client_random[TLS_KEY_RANDOM_LEN],
                               const unsigned char server_random[TLS_KEY_RANDOM_LEN],
                               mbedtls_tls_prf_types tls_prf_type)
{
	if ((p_expkey == NULL) || (secret == NULL))
		return;

	eap_tls_keys *keys = (eap_tls_keys *)p_expkey;
	if (secret_len != sizeof(keys->master_secret))
		return;

	(void)secret_type;

	(void)os_memcpy(keys->master_secret, secret, sizeof(keys->master_secret));
	(void)os_memcpy(keys->randbytes, client_random, TLS_KEY_RANDOM_LEN);
	(void)os_memcpy(keys->randbytes + TLS_KEY_RANDOM_LEN, server_random, TLS_KEY_RANDOM_LEN);
	keys->tls_prf_type = tls_prf_type;
}
#endif

int tls_get_errors(void *tls_ctx)
{
	(void)tls_ctx;
	return 0;
}

static int tls_wpa_send(void *ctx, const unsigned char *buf, size_t len)
{
	struct tls_connection *conn = NULL;

	if ((ctx == NULL) || (buf == NULL) || (len == 0))
		return 0;

	conn = (struct tls_connection *)ctx;
	if (conn->out_buf == NULL) {
		conn->out_buf = wpabuf_alloc_copy(buf, len);
		if (conn->out_buf == NULL)
			return MBEDTLS_ERR_SSL_WANT_WRITE;
	} else {
		if (wpabuf_resize(&conn->out_buf, len) == 0)
			wpabuf_put_data(conn->out_buf, buf, len);
		else
			return MBEDTLS_ERR_SSL_WANT_WRITE;
	}
	return len;
}

static int tls_wpa_recv(void *ctx, unsigned char *buf, size_t len)
{
	struct tls_connection *conn = NULL;
	size_t data_len = len;

	if ((ctx == NULL) || (buf == NULL))
		return 0;

	conn = (struct tls_connection *)ctx;
	if ((conn->in_buf == NULL) || (conn->in_buf_len == 0))
		return MBEDTLS_ERR_SSL_WANT_READ;

	if (conn->in_buf_len < len)
		data_len = conn->in_buf_len;

	(void)os_memcpy(buf, conn->in_buf, data_len);

	conn->in_buf     += data_len;
	conn->in_buf_len -= data_len;

	return data_len;
}

#if defined(CONFIG_WPA_ENTERPRISE_DEBUG)
static void tls_debug(void *ctx, int level,
                      const char *file, int line,
                      const char *str)
{
	(void)ctx;
	const char *p = NULL;
	const char *basename = NULL;
	for (p = basename = file; (p != NULL) && (*p != '\0'); p++) {
		if (*p == '/' || *p == '\\')
			basename = p + 1;
	}

	wpa_printf(MSG_DEBUG, "%s:%04d: |%d| %s", basename, line, level, str);
}
#endif

struct tls_connection *tls_connection_init(void *tls_ctx)
{
	int ret = 0;
	const char *pers = "eap tls";
	struct tls_connection *conn = NULL;
	mbedtls_ssl_context *ssl = NULL;
	mbedtls_entropy_context *entropy = NULL;
	mbedtls_ctr_drbg_context *ctr_drbg = NULL;
	mbedtls_ssl_config *conf = NULL;
	int role = MBEDTLS_SSL_IS_CLIENT;

	conn = os_zalloc(sizeof(*conn));
	if (conn == NULL)
		return NULL;

	ssl = os_zalloc(sizeof(*ssl));
	conf = os_zalloc(sizeof(*conf));
	ctr_drbg = os_zalloc(sizeof(*ctr_drbg));
	entropy = os_zalloc(sizeof(*entropy));
	if (ssl == NULL || conf == NULL || ctr_drbg == NULL || entropy == NULL) {
		os_free(ssl);
		os_free(conf);
		os_free(ctr_drbg);
		os_free(entropy);
		os_free(conn);
		return NULL;
	}

	conn->ssl = ssl;
	conn->conf = conf;
	conn->ctr_drbg = ctr_drbg;
	conn->entropy = entropy;

	/* Initialize the RNG and the session data */
	mbedtls_ssl_init(ssl);
	mbedtls_ssl_config_init(conf);
	mbedtls_ctr_drbg_init(ctr_drbg);

	mbedtls_entropy_init(entropy);
	ret = mbedtls_ctr_drbg_seed(ctr_drbg, mbedtls_entropy_func, entropy,
	                            (const unsigned char *)pers,
	                            strlen(pers));
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "TLS: Failed to generate random number(%d)!", ret);
		goto exit;
	}

	/* Setup stuff */
	ret = mbedtls_ssl_config_defaults(conf, role,
	                                  MBEDTLS_SSL_TRANSPORT_STREAM,
	                                  MBEDTLS_SSL_PRESET_DEFAULT);
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "TLS: Failed to load default config(%d)!", ret);
		goto exit;
	}

	/* REQUIRED is the recommended mode security on client */
	mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_REQUIRED);
	mbedtls_ssl_conf_rng(conf, mbedtls_ctr_drbg_random, ctr_drbg);

	ret = mbedtls_ssl_setup(ssl, conf);
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "TLS: Failed to setup ssl(%d)!", ret);
		goto exit;
	}

	mbedtls_ssl_set_bio(ssl, conn, tls_wpa_send, tls_wpa_recv, NULL);
#if (MBEDTLS_VERSION_NUMBER < 0x03000000)
	mbedtls_ssl_conf_export_keys_ext_cb(conf, tls_key_derivation, &conn->eap_tls_keying);
#else
	mbedtls_ssl_set_export_keys_cb(ssl, tls_key_derivation, &conn->eap_tls_keying);
#endif

#if defined(CONFIG_WPA_ENTERPRISE_DEBUG)
	mbedtls_debug_set_threshold(3);
	mbedtls_ssl_conf_dbg(conf, tls_debug, NULL);
#endif

	return conn;
exit:
	tls_connection_deinit(tls_ctx, conn);
	return NULL;
}

void tls_connection_deinit(void *tls_ctx, struct tls_connection *conn)
{
	(void)tls_ctx;

	if (conn == NULL)
		return;

	forced_memzero(&(conn->eap_tls_keying), sizeof(eap_tls_keys));
	if (conn->ca_cert != NULL) {
		mbedtls_x509_crt_free(conn->ca_cert);
		os_free(conn->ca_cert);
		conn->ca_cert = NULL;
	}
	if (conn->cli_cert != NULL) {
		mbedtls_x509_crt_free(conn->cli_cert);
		os_free(conn->cli_cert);
		conn->cli_cert = NULL;
	}
	if (conn->pk != NULL) {
		mbedtls_pk_free(conn->pk);
		os_free(conn->pk);
		conn->pk = NULL;
	}
	if (conn->entropy != NULL) {
		mbedtls_entropy_free(conn->entropy);
		os_free(conn->entropy);
		conn->entropy = NULL;
	}
	if (conn->ctr_drbg != NULL) {
		mbedtls_ctr_drbg_free(conn->ctr_drbg);
		os_free(conn->ctr_drbg);
		conn->ctr_drbg = NULL;
	}
	if (conn->ssl != NULL) {
		mbedtls_ssl_free(conn->ssl);
		os_free(conn->ssl);
		conn->ssl = NULL;
	}
	if (conn->conf != NULL) {
		mbedtls_ssl_config_free(conn->conf);
		os_free(conn->conf);
		conn->conf = NULL;
	}
	os_free(conn);
}

int tls_connection_established(void *tls_ctx, struct tls_connection *conn)
{
	(void)tls_ctx;

	if (conn == NULL)
		return 0;
	return conn->status;
}

int tls_connection_shutdown(void *tls_ctx, struct tls_connection *conn)
{
	(void)tls_ctx;

	if (conn == NULL)
		return -1;

	(void)mbedtls_ssl_session_reset(conn->ssl);
	conn->status = 0;
	conn->failed = 0;
	conn->in_buf = NULL;
	conn->in_buf_len = 0;
	conn->client_hello_generated = 0;
	return 0;
}

static int tls_connection_ca_cert(struct tls_connection *conn,
                                  const char *ca_cert, const u8 *ca_cert_blob,
                                  size_t ca_cert_blob_len, const char *ca_path)
{
	mbedtls_ssl_context *ssl = NULL;
	mbedtls_ssl_config *conf = NULL;
	unsigned char *cert_buf = NULL;
	mbedtls_x509_crt *cacert = NULL;
	unsigned int cert_len = 0;
	int ret;

	(void)ca_cert;
	(void)ca_cert_blob;
	(void)ca_cert_blob_len;
	(void)ca_path;

	if ((conn == NULL) || (conn->ssl == NULL))
		return -1;

	ssl = conn->ssl;
	conf = (mbedtls_ssl_config *)ssl->conf;

	if (g_tls_import_cb.ca_cert_import == NULL) {
		wpa_printf(MSG_ERROR, "TLS: ca_cert == NULL!");
		return -1;
	}

	g_tls_import_cb.ca_cert_import(&cert_buf, &cert_len);
	if ((cert_buf == NULL) || (cert_len == 0)) {
		wpa_printf(MSG_ERROR, "TLS: Failed to read ca cert!");
		return -1;
	}

	cacert = os_zalloc(sizeof(*cacert));
	if (cacert == NULL)
		return -1;

	mbedtls_x509_crt_init(cacert);
	ret = mbedtls_x509_crt_parse(cacert, cert_buf, (size_t)cert_len);
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "TLS: Failed to parse crt!");
		ret = -1;
		goto exit;
	}

	mbedtls_ssl_conf_ca_chain(conf, cacert, NULL);
	conn->ca_cert = cacert;

	return 0;

exit:
	mbedtls_x509_crt_free(cacert);
	os_free(cacert);
	return ret;
}

static int tls_connection_client_cert(struct tls_connection *conn,
                                      const char *private_key,
                                      const char *client_cert,
                                      const u8 *client_cert_blob,
                                      size_t client_cert_blob_len)
{
	mbedtls_ssl_context *ssl = NULL;
	mbedtls_ssl_config *conf = NULL;
	unsigned char *cli_buf = NULL;
	unsigned char *key_buf = NULL;
	mbedtls_x509_crt *clicert = NULL;
	mbedtls_pk_context *pk = NULL;
	unsigned int    cli_len = 0;
	unsigned int    key_len = 0;
	int ret;

	(void)private_key;
	(void)client_cert;
	(void)client_cert_blob;
	(void)client_cert_blob_len;

	if ((conn == NULL) || (conn->ssl == NULL))
		return -1;

	ssl = conn->ssl;
	conf = (mbedtls_ssl_config *)ssl->conf;

	if ((g_tls_import_cb.cli_cert_import == NULL) || (g_tls_import_cb.cli_key_import == NULL)) {
		wpa_printf(MSG_ERROR, "TLS: cli_cert/cli_key == NULL!");
		return -1;
	}

	g_tls_import_cb.cli_cert_import(&cli_buf, &cli_len);
	if ((cli_buf == NULL) || (cli_len == 0)) {
		wpa_printf(MSG_ERROR, "TLS: Failed to read client cert!");
		return -1;
	}
	g_tls_import_cb.cli_key_import(&key_buf, &key_len);
	if ((key_buf == NULL) || (key_len == 0)) {
		wpa_printf(MSG_ERROR, "TLS: Failed to read client key!");
		return -1;
	}

	clicert = os_zalloc(sizeof(*clicert));
	if (clicert == NULL)
		return -1;

	mbedtls_x509_crt_init(clicert);
	ret = mbedtls_x509_crt_parse(clicert, cli_buf, (size_t)cli_len);
	if (ret != 0)
		goto exit;

	pk = os_zalloc(sizeof(*pk));
	if (pk == NULL)
		goto exit;

	mbedtls_pk_init(pk);
#if (MBEDTLS_VERSION_NUMBER < 0x03000000)
	ret = mbedtls_pk_parse_key(pk, key_buf, (size_t)key_len, NULL, 0);
#else
	ret = mbedtls_pk_parse_key(pk, key_buf, (size_t)key_len, NULL, 0,
	                           mbedtls_ctr_drbg_random, conn->ctr_drbg);
#endif
	if (ret != 0)
		goto exit;

	ret = mbedtls_ssl_conf_own_cert(conf, clicert, pk);
	if (ret != 0)
		goto exit;

	conn->cli_cert = clicert;
	conn->pk       = pk;

	return ret;

exit:
	if (pk != NULL) {
		mbedtls_pk_free(pk);
		os_free(pk);
	}
	if (clicert != NULL) {
		mbedtls_x509_crt_free(clicert);
		os_free(clicert);
	}
	return -1;
}

int tls_connection_set_params(void *tls_ctx, struct tls_connection *conn,
                              const struct tls_connection_params *params)
{
	(void)tls_ctx;

	if ((conn == NULL) || (params == NULL))
		return -1;

	if (tls_connection_ca_cert(conn,
	                           params->ca_cert,
	                           params->ca_cert_blob,
	                           params->ca_cert_blob_len,
	                           params->ca_path)) {
		wpa_printf(MSG_ERROR, "TLS: Failed to set CA certificate!");
		return -1;
	}

	if (tls_connection_client_cert(conn,
	                               params->private_key,
	                               params->client_cert,
	                               params->client_cert_blob,
	                               params->client_cert_blob_len)) {
		wpa_printf(MSG_ERROR, "TLS: Failed to set client certificate!");
		return -1;
	}

	return 0;
}

int tls_connection_get_random(void *tls_ctx, struct tls_connection *conn, struct tls_random *data)
{
	(void)tls_ctx;

	if ((conn == NULL) || (data == NULL))
		return -1;

	os_memset(data, 0, sizeof(*data));
	data->client_random     = conn->eap_tls_keying.randbytes;
	data->client_random_len = TLS_KEY_RANDOM_LEN;
	data->server_random     = conn->eap_tls_keying.randbytes + TLS_KEY_RANDOM_LEN;
	data->server_random_len = TLS_KEY_RANDOM_LEN;
	return 0;
}

int tls_connection_export_key(void *tls_ctx, struct tls_connection *conn,
                              const char *label, const u8 *context,
                              size_t context_len, u8 *out, size_t out_len)
{
	eap_tls_keys *keys = NULL;
	(void)tls_ctx;
	(void)context;

	if ((conn == NULL) || (label == NULL) || (out == NULL))
		return -1;

	keys = &conn->eap_tls_keying;
	if (mbedtls_ssl_tls_prf(keys->tls_prf_type,
	                        keys->master_secret,
	                        sizeof(keys->master_secret),
	                        label,
	                        keys->randbytes,
	                        sizeof(keys->randbytes),
	                        out, out_len) != 0)
		return -1;

	return 0;
}

static struct wpabuf *tls_wpa_handshake(struct tls_connection *conn, const struct wpabuf *in_data)
{
	int res;

	if (conn == NULL)
		return NULL;

	/* out_buf is freed by eap_tls_process_output */
	conn->out_buf = NULL;

	/* conn->in_buf is the pointer of in_data and shouldn't be freed */
	if (in_data != NULL && wpabuf_len(in_data) > 0) {
		conn->in_buf     = wpabuf_head(in_data);
		conn->in_buf_len = wpabuf_len(in_data);
	}

	/* Initiate TLS handshake or continue the existing handshake */
	res = mbedtls_ssl_handshake(conn->ssl);
	if (res != 0) {
		if (res == MBEDTLS_ERR_SSL_WANT_READ)
			wpa_printf(MSG_DEBUG, "TLS: SSL_connect - want more data");
		else if (res == MBEDTLS_ERR_SSL_WANT_WRITE)
			wpa_printf(MSG_DEBUG, "TLS: SSL_connect - want to write");
		else {
			conn->failed++;
			if (!conn->server && !conn->client_hello_generated) {
				/* The server would not understand TLS Alert
				 * before ClientHello, so simply terminate
				 * handshake on this type of error case caused
				 * by a likely internal error like no ciphers
				 * available. */
				wpa_printf(MSG_DEBUG, "TLS: Could not generate ClientHello");
			}
		}

		if (res != MBEDTLS_ERR_SSL_WANT_READ)
			goto exit;
	} else
		conn->status = 1;

	if (conn->out_buf == NULL) {
		wpa_printf(MSG_DEBUG, "TLS: Added dummy for ack.");
		/* return 0-size buf */
		conn->out_buf = wpabuf_alloc(0);
	}

	if (!conn->server && !conn->failed)
		conn->client_hello_generated = 1;
exit:
	return conn->out_buf;
}

struct wpabuf *tls_connection_handshake(void *tls_ctx,
                                        struct tls_connection *conn,
                                        const struct wpabuf *in_data,
                                        struct wpabuf **appl_data)
{
	(void)tls_ctx;
	(void)appl_data;
	return tls_wpa_handshake(conn, in_data);
}

struct wpabuf *tls_connection_encrypt(void *tls_ctx,
                                      struct tls_connection *conn,
                                      const struct wpabuf *in_data)
{
	(void)tls_ctx;
	(void)conn;
	(void)in_data;
	return NULL;
}

struct wpabuf *tls_connection_decrypt(void *tls_ctx,
                                      struct tls_connection *conn,
                                      const struct wpabuf *in_data)
{
	(void)tls_ctx;
	(void)conn;
	(void)in_data;
	return NULL;
}

int tls_connection_resumed(void *tls_ctx, struct tls_connection *conn)
{
	(void)tls_ctx;
	(void)conn;
	return 0;
}

int tls_get_version(void *ssl_ctx, struct tls_connection *conn, char *buf, size_t buflen)
{
	const char *name = NULL;
	(void)ssl_ctx;

	if ((conn == NULL) || (conn->ssl == NULL) || (buf == NULL) || (buflen == 0))
		return -1;

	name = mbedtls_ssl_get_version(conn->ssl);
	if (name == NULL)
		return -1;

	os_strlcpy(buf, name, buflen);
	return 0;
}

int tls_get_cipher(void *tls_ctx, struct tls_connection *conn, char *buf, size_t buflen)
{
	const char *name = NULL;
	(void)tls_ctx;

	if ((conn == NULL) || (conn->ssl == NULL) || (buf == NULL) || (buflen == 0))
		return -1;

	name = mbedtls_ssl_get_ciphersuite(conn->ssl);
	if (name == NULL) {
		return -1;
	}

	os_strlcpy(buf, name, buflen);
	return 0;
}

int tls_connection_get_failed(void *tls_ctx, struct tls_connection *conn)
{
	(void)tls_ctx;
	if (conn == NULL)
		return -1;
	return conn->failed;
}

int tls_set_import_cb(ext_wifi_ent_import_callback *cb)
{
	if (cb == NULL)
		return -1;
	(void)os_memcpy(&g_tls_import_cb, cb, sizeof(ext_wifi_ent_import_callback));
	return 0;
}

