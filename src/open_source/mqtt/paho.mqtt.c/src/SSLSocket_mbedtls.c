/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Mbedtls Adaptation Layer of MQTT
 * Author: LiteOS Team
 * Create: 2020-08-10
 */
#if defined(MBEDTLS)

#include "SocketBuffer.h"
#include "MQTTClient.h"
#include "MQTTProtocolOut.h"
#include "SSLSocket.h"
#include "Log.h"
#include "StackTrace.h"
#include "Socket.h"

#include "Heap.h"

#include "securec.h"

#if defined(IOT_CONNECT)
#include "atiny_mqtt_commu.h"
#include "soc_socket.h"
#endif

#include <string.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ssl.h>
#include <mbedtls/x509.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/platform.h>
#if !defined(IOT_LITEOS_ADAPT)
#include <ssl_misc.h>
#endif

extern Sockets mod_s;
static ssl_mutex_type sslCoreMutex;

static int SSL_create_mutex(ssl_mutex_type* mutex);
static int SSL_lock_mutex(ssl_mutex_type* mutex);
static int SSL_unlock_mutex(ssl_mutex_type* mutex);
static void SSL_destroy_mutex(ssl_mutex_type* mutex);
static int SSLSocket_createContext(networkHandles* net, MQTTClient_SSLOptions* opts);
static void SSLSocket_destroyContext(networkHandles* net);
static void SSLSocket_addPendingRead(int sock);

#if defined(WIN32) || defined(WIN64)
#define iov_len len
#define iov_base buf
#endif


#if !defined(ARRAY_SIZE)
/**
 * Macro to calculate the number of entries in an array
 */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#endif

#if defined(SEND_MAX_LEN)
#define LOG_LEN 150
#define SSL_HEADER_LEN 64
#endif

static int SSL_create_mutex(ssl_mutex_type* mutex)
{
	int rc = 0;
	if (mutex == NULL)
		return -1;

	FUNC_ENTRY;
#if defined(WIN32) || defined(WIN64)
	*mutex = CreateMutex(NULL, 0, NULL);
#elif defined(COMPAT_CMSIS)
	*mutex = osMutexNew(NULL);
	rc = (*mutex == NULL) ? -1 : 0;
#else
	rc = pthread_mutex_init(mutex, NULL);
#endif
	FUNC_EXIT_RC(rc);
	return rc;
}

static int SSL_lock_mutex(ssl_mutex_type* mutex)
{
	int rc = -1;
	if (mutex == NULL)
		return rc;

	/* don't add entry/exit trace points, as trace gets lock too, and it might happen quite frequently  */
#if defined(WIN32) || defined(WIN64)
	if (WaitForSingleObject(*mutex, INFINITE) != WAIT_FAILED)
#elif defined(COMPAT_CMSIS)
	if ((rc = osMutexAcquire(*mutex, osWaitForever)) == osOK)
#else
	if ((rc = pthread_mutex_lock(mutex)) == 0)
#endif
		rc = 0;

	return rc;
}

static int SSL_unlock_mutex(ssl_mutex_type* mutex)
{
	int rc = -1;
	if (mutex == NULL)
		return rc;

	/* don't add entry/exit trace points, as trace gets lock too, and it might happen quite frequently  */
#if defined(WIN32) || defined(WIN64)
	if (ReleaseMutex(*mutex) != 0)
#elif defined (COMPAT_CMSIS)
	if ((rc = osMutexRelease(*mutex)) == osOK)
#else
	if ((rc = pthread_mutex_unlock(mutex)) == 0)
#endif
		rc = 0;

	return rc;
}

static void SSL_destroy_mutex(ssl_mutex_type* mutex)
{
	int rc = 0;
	if (mutex == NULL)
		return;

	FUNC_ENTRY;
#if defined(WIN32) || defined(WIN64)
	if (*mutex != NULL)
		rc = CloseHandle(*mutex);
#elif defined(COMPAT_CMSIS)
	if (*mutex != NULL)
		rc = osMutexDelete(*mutex);
#else
	rc = pthread_mutex_destroy(mutex);
#endif
	FUNC_EXIT_RC(rc);
}

#ifdef MBEDTLS_USE_CRT
/*
 * custom verify on mbedtls, when ssl option verify is not set.
 */
static int SSL_verify_discard(void* data, mbedtls_x509_crt* crt, int depth, uint32_t* flags)
{
	char buf[512];
	((void) data);
	(void)crt;
	(void)depth;
	if (flags != NULL && (*flags) != 0)
	{
		mbedtls_x509_crt_verify_info(buf, sizeof(buf), NULL, *flags);
		Log(TRACE_PROTOCOL, 1,  "Warnning! flags:%u %s", *flags, buf);

		/* Discard CN mismatch when ssl options verify unset */
		if (*flags == MBEDTLS_X509_BADCERT_CN_MISMATCH)
			*flags = 0;
	}

	return 0;
}
#endif

void SSLSocket_handleOpensslInit(int bool_value)
{
	(void)bool_value;
	return;
}


int SSLSocket_initialize(void)
{
	int rc = 0;
	FUNC_ENTRY;

	SSL_create_mutex(&sslCoreMutex);

	FUNC_EXIT_RC(rc);
	return rc;
}

void SSLSocket_terminate(void)
{
	FUNC_ENTRY;

	SSL_destroy_mutex(&sslCoreMutex);

	FUNC_EXIT;
}

static int SSL_loadClientCrt(networkHandles* net, const MQTTClient_SSLOptions* opts)
{
	int rc;
#if !defined (IOT_CONNECT) && !defined(IOT_LITEOS_ADAPT)
	if (opts->keyStore != NULL && opts->privateKey != NULL)
#else
	if (opts->los_keyStore != NULL && opts->los_privateKey != NULL)
#endif
	{
		/* parse client cert */
#if !defined (IOT_CONNECT) && !defined(IOT_LITEOS_ADAPT)
		rc = mbedtls_x509_crt_parse_file(&net->ctx->clicert, opts->keyStore);
#else
		rc = mbedtls_x509_crt_parse( &net->ctx->clicert, opts->los_keyStore->body, opts->los_keyStore->size );
#endif
		if (rc != 0)
		{
			Log(TRACE_PROTOCOL, -1, "failed ! mbedtls_x509_crt_parse_file");
			return rc;
		}

		/* parse client key */
#if !defined (IOT_CONNECT) && !defined(IOT_LITEOS_ADAPT)
		rc = mbedtls_pk_parse_keyfile(&net->ctx->pkey, opts->privateKey, opts->privateKeyPassword);
#else
		if (opts->privateKeyPassword == NULL)
			rc = mbedtls_pk_parse_key( &net->ctx->pkey, opts->los_privateKey->body, opts->los_privateKey->size, NULL, 0,
				NULL, NULL);
		else
			rc = mbedtls_pk_parse_key( &net->ctx->pkey, opts->los_privateKey->body, opts->los_privateKey->size,
				(const unsigned char *) opts->privateKeyPassword, strlen( opts->privateKeyPassword ), NULL, NULL);
#endif
		if (rc != 0)
		{
			Log(TRACE_PROTOCOL, -1, "failed ! mbedtls_pk_parse_keyfile");
			return rc;
		}

		/* config own cert */
		rc = mbedtls_ssl_conf_own_cert(&net->ctx->conf, &net->ctx->clicert, &net->ctx->pkey);
		if (rc != 0)
		{
			Log(TRACE_PROTOCOL, -1, "failed ! mbedtls_ssl_conf_own_cert");
			return rc;
		}
	}
	return 0;
}

static int SSL_loadKey(networkHandles* net, const MQTTClient_SSLOptions* opts)
{
	int rc;
#ifdef MBEDTLS_USE_PSK
	rc = mbedtls_ssl_conf_psk(&net->ctx->conf, opts->psk, opts->psk_len, opts->psk_id, opts->psk_id_len);
	if (rc != 0)
	{
		Log(TRACE_PROTOCOL, -1, "failed ! mbedtls_ssl_conf_psk");
		return rc;
	}
#endif /* MBEDTLS_USE_PSK */

#ifdef MBEDTLS_USE_CRT
	rc = SSL_loadClientCrt(net, opts);
	if (rc != 0) {
		return rc;
	}

#if !defined (IOT_CONNECT) && !defined(IOT_LITEOS_ADAPT)
	if (opts->trustStore != NULL)
#else
	if (opts->los_trustStore != NULL)
#endif
	{
		/* parse CA file */
#if !defined (IOT_CONNECT) && !defined(IOT_LITEOS_ADAPT)
		rc = mbedtls_x509_crt_parse_file(&net->ctx->cacert, opts->trustStore);
#else
		rc = mbedtls_x509_crt_parse( &net->ctx->cacert, opts->los_trustStore->body, opts->los_trustStore->size );
#endif
		if (rc != 0)
		{
			Log(TRACE_PROTOCOL, -1, "failed ! mbedtls_x509_crt_parse_file");
			return rc;
		}
		/* set the ca certificate chain */
		mbedtls_ssl_conf_ca_chain(&net->ctx->conf, &net->ctx->cacert, NULL);
	}

	if (opts->enableServerCertAuth != 0)
		mbedtls_ssl_conf_authmode(&net->ctx->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
	else
		mbedtls_ssl_conf_authmode(&net->ctx->conf, MBEDTLS_SSL_VERIFY_NONE);

	/* custom mbedtls verify */
	if (opts->verify == 0)
		mbedtls_ssl_conf_verify(&net->ctx->conf, SSL_verify_discard, NULL);
#endif /* MBEDTLS_USE_CRT */
	return 0;
}

static void SSL_setVersion(networkHandles* net, const MQTTClient_SSLOptions* opts)
{
	int sslVersion = MQTT_SSL_VERSION_DEFAULT;
	if (opts->struct_version >= 1)
		sslVersion = opts->sslVersion;
	switch (sslVersion)
	{
	case MQTT_SSL_VERSION_DEFAULT:
		break;
#if !defined(IOT_LITEOS_ADAPT)
	case MQTT_SSL_VERSION_TLS_1_0:
		mbedtls_ssl_conf_min_version(&net->ctx->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_1);
		mbedtls_ssl_conf_max_version(&net->ctx->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_1);
		break;
	case MQTT_SSL_VERSION_TLS_1_1:
		mbedtls_ssl_conf_min_version(&net->ctx->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_2);
		mbedtls_ssl_conf_max_version(&net->ctx->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_2);
		break;
#endif
	case MQTT_SSL_VERSION_TLS_1_2:
		mbedtls_ssl_conf_min_version(&net->ctx->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
		mbedtls_ssl_conf_max_version(&net->ctx->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
		break;
	default:
		break;
	}
}

static int SSL_tlsInit(networkHandles* net, const MQTTClient_SSLOptions* opts)
{
	int rc = 0;
	/* RNG related string */
	static const char personalization[] = "paho_mbedtls_entropy";

	net->ctx = (SSL_CTX*)malloc(sizeof(SSL_CTX));
	if (net->ctx == NULL)
	{
		Log(TRACE_PROTOCOL, -1, "allocate context failed.");
		return PAHO_MEMORY_ERROR;
	}

	/* initialise the mbedtls context */
	mbedtls_ssl_config_init(&net->ctx->conf);
	/* initialise RNG */
	mbedtls_entropy_init(&net->ctx->entropy);
	mbedtls_ctr_drbg_init(&net->ctx->ctr_drbg);
#ifdef MBEDTLS_USE_CRT
	/* init certificates */
	mbedtls_x509_crt_init(&net->ctx->cacert);
	mbedtls_x509_crt_init(&net->ctx->clicert);
	mbedtls_pk_init(&net->ctx->pkey);
#endif /* MBEDTLS_USE_CRT */
	if ((rc = mbedtls_ctr_drbg_seed(
			&net->ctx->ctr_drbg, mbedtls_entropy_func,
			&net->ctx->entropy, (const unsigned char*)personalization,
				sizeof(personalization))) != 0)
	{
		Log(TRACE_PROTOCOL, -1, "failed ! mbedtls_ctr_drbg_seed returned %d", rc);
		SSLSocket_destroyContext(net);
		return rc;
	}

	mbedtls_ssl_conf_rng(&net->ctx->conf, mbedtls_ctr_drbg_random,
							&net->ctx->ctr_drbg);

	/* load default config */
	if ((rc = mbedtls_ssl_config_defaults(&net->ctx->conf,
			MBEDTLS_SSL_IS_CLIENT,
			MBEDTLS_SSL_TRANSPORT_STREAM,
			MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
	{
		Log(TRACE_PROTOCOL, -1, "failed ! mbedtls_ssl_config_defaults returned %d", rc);
		SSLSocket_destroyContext(net);
		return rc;
	}

	SSL_setVersion(net, opts);

	return rc;
}

int SSLSocket_createContext(networkHandles* net, MQTTClient_SSLOptions* opts)
{
	int rc = 0;

	FUNC_ENTRY;
	if (net->ctx == NULL)
	{
		rc = SSL_tlsInit(net, opts);
		if (rc != 0)
			goto exit;

	}

	rc = SSL_loadKey(net, opts);
	if (rc != 0)
		SSLSocket_destroyContext(net);

exit:
	FUNC_EXIT_RC(rc);
	return rc;
}

#define SSL_MAX_COUNT 65535
/**
 * SSLSocket_setSocketForSSL
 * @return 0 is failure, 1 is success
 */
int SSLSocket_setSocketForSSL(networkHandles* net, MQTTClient_SSLOptions* opts,
	const char* hostname, size_t hostname_len)
{
	int rc = 0;
	int ret_state = 0;

	if (net == NULL || opts == NULL || hostname == NULL)
		goto exit;
	if (hostname_len > SSL_MAX_COUNT)
		goto exit;

	FUNC_ENTRY;
	if (net->ctx != NULL || (ret_state = SSLSocket_createContext(net, opts)) == 0)
	{
		if (net->ssl == NULL)
		{
			net->ssl = malloc(sizeof(mbedtls_ssl_context)); // free in SSLSocket_close
			if (net->ssl == NULL)
			{
				Log(TRACE_PROTOCOL, -1, "allocate ssl context failed.\n");
				goto exit;
			}
			mbedtls_ssl_init(net->ssl);
		}
		if ((ret_state = mbedtls_ssl_setup(net->ssl, &net->ctx->conf)) != 0)
		{
			Log(TRACE_PROTOCOL, 1, "failed! mbedtls_ssl_setup returned %d \n", ret_state);
			goto exit;
		}
#ifdef MBEDTLS_USE_CRT
		char *hostname_plus_null;
		hostname_plus_null = malloc(hostname_len + 1u );
		if (hostname_plus_null == NULL)
		{
			Log(TRACE_PROTOCOL, -1, "allocate hostname_plus_null failed.\n");
			goto exit;
		}
		MQTTStrncpy(hostname_plus_null, hostname, hostname_len + 1u);
		if ((ret_state = mbedtls_ssl_set_hostname(net->ssl, hostname_plus_null)) != 0)
		{
			Log(TRACE_PROTOCOL, 1, "failed! mbedtls_ssl_set_hostname returned %d \n", ret_state);
			free(hostname_plus_null);
			goto exit;
		}
		free(hostname_plus_null);
#endif
		mbedtls_ssl_set_bio(net->ssl, &net->socket, mbedtls_net_send, mbedtls_net_recv, NULL);
	}

	if (ret_state == 0)
		rc = 1;

exit:
	FUNC_EXIT_RC(rc);
	return rc;
}

/*
 * Return value: 1 - success, TCPSOCKET_INTERRUPTED - try again, anything else is failure
 */
int SSLSocket_connect(SSL* ssl, int sock, const char* hostname, int verify, int (*cb)(const char *str, size_t len, void *u), void* u)
{
	int rc = 1;
	int ret_state = 0;
	(void)sock;
	(void)hostname;
	(void)verify;
	(void)cb;
	(void)u;
	FUNC_ENTRY;

	ret_state = mbedtls_ssl_handshake(ssl);

	if (ret_state == MBEDTLS_ERR_SSL_WANT_READ ||
			ret_state == MBEDTLS_ERR_SSL_WANT_WRITE ||
			ret_state == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS ||
			ret_state == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS)
	{
		rc = TCPSOCKET_INTERRUPTED;
	}
	else if (ret_state == 0)
	{
		/* handshake complete check server certificate */
		Log(TRACE_MIN, -1, "ssl handshake complete.\n");
#if !defined(IOT_LITEOS_ADAPT)
		rc = 1;
#else
        rc = 0;
#endif
	} else {
		rc = SSL_FATAL;
		Log(TRACE_PROTOCOL, -1, "failed! mbedtls_ssl_handshake returned -0x%x\n", ret_state);
	}
	FUNC_EXIT_RC(rc);
	return rc;
}


/**
 *  Reads one byte from a socket
 *  @param socket the socket to read from
 *  @param c the character read, returned
 *  @return completion code
 */
int SSLSocket_getch(SSL* ssl, int socket, char* c)
{
	int rc = SOCKET_ERROR;
	if (ssl == NULL || c == NULL)
		goto exit;

	FUNC_ENTRY;
	if ((rc = SocketBuffer_getQueuedChar(socket, c)) != SOCKETBUFFER_INTERRUPTED)
		goto exit;

	if ((rc = mbedtls_ssl_read(ssl, (unsigned char *)c, (size_t)1)) < 0)
	{
		Log(TRACE_MIN, -1, "[%s,%d]rc = %d", __func__, __LINE__, rc);
		if (rc == MBEDTLS_ERR_SSL_WANT_READ || rc == MBEDTLS_ERR_SSL_WANT_WRITE)
		{
			rc = TCPSOCKET_INTERRUPTED;
			SocketBuffer_interrupted(socket, 0);
		}
		else
		{
			rc = SOCKET_ERROR;
		}
	}
	else if (rc == 0)
	{
		rc = SOCKET_ERROR;  /* The return value from recv is 0 when the peer has performed an orderly shutdown. */
	}
	else if (rc == 1)
	{
		SocketBuffer_queueChar(socket, *c);
		rc = TCPSOCKET_COMPLETE;
	}
exit:
	FUNC_EXIT_RC(rc);
	return rc;
}


/**
 *  Attempts to read a number of bytes from a socket, non-blocking. If a previous read did not
 *  finish, then retrieve that data.
 *  @param socket the socket to read from
 *  @param bytes the number of bytes to read
 *  @param actual_len the actual number of bytes read
 *  @return completion code
 */
char *SSLSocket_getdata(SSL* ssl, int socket, size_t bytes, size_t* actual_len, int* rc)
{
	char* buf = NULL;
	if (ssl == NULL || actual_len == NULL || rc == NULL)
		goto exit;

	FUNC_ENTRY;
	if (bytes == 0)
	{
		buf = SocketBuffer_complete(socket);
		goto exit;
	}

	buf = SocketBuffer_getQueuedData(socket, bytes, actual_len);

	if ((*rc = mbedtls_ssl_read(ssl, (unsigned char *)(buf + (*actual_len)), (int)(bytes - (*actual_len)))) < 0)
	{
		Log(TRACE_MIN, -1, "[%s,%d]rc = %d", __func__, __LINE__, *rc);
		if (*rc != MBEDTLS_ERR_SSL_WANT_READ && *rc != MBEDTLS_ERR_SSL_WANT_WRITE)
		{
			buf = NULL;
			goto exit;
		}
	}
	else if (*rc == 0) /* rc 0 means the other end closed the socket */
	{
		buf = NULL;
		goto exit;
	}
	else
	{
		*actual_len += *rc;
	}

	if (*actual_len == bytes)
	{
		SocketBuffer_complete(socket);
		/* if we read the whole packet, there might still be data waiting in the SSL buffer, which
		isn't picked up by select.  So here we should check for any data remaining in the SSL buffer, and
		if so, add this socket to a new "pending SSL reads" list.
		*/
		if (mbedtls_ssl_get_bytes_avail(ssl) > 0) /* return no of bytes pending */
			SSLSocket_addPendingRead(socket);
	}
	else /* we didn't read the whole packet */
	{
		SocketBuffer_interrupted(socket, *actual_len);
		Log(TRACE_MAX, -1, "SSL_read: %u bytes expected but %u bytes now received", bytes, *actual_len);
	}
exit:
	FUNC_EXIT;
	return buf;
}

void SSLSocket_destroyContext(networkHandles* net)
{
	FUNC_ENTRY;
	if (net != NULL && net->ctx != NULL)
	{
		mbedtls_ssl_config_free(&net->ctx->conf);
		mbedtls_ctr_drbg_free(&net->ctx->ctr_drbg);
		mbedtls_entropy_free(&net->ctx->entropy);
#ifdef MBEDTLS_USE_CRT
		mbedtls_x509_crt_free(&net->ctx->cacert);
		mbedtls_x509_crt_free(&net->ctx->clicert);
		mbedtls_pk_free(&net->ctx->pkey);
#endif
		free(net->ctx);
		net->ctx = NULL;
	}
	FUNC_EXIT;
}

static List pending_reads = {NULL, NULL, NULL, 0, 0};

int SSLSocket_close(networkHandles* net)
{
	int rc = 1;
	if (net == NULL)
		return rc;

	FUNC_ENTRY;
	/* clean up any pending reads for this socket */
	if (pending_reads.count > 0 && ListFindItem(&pending_reads, &net->socket, intcompare) != NULL)
		ListRemoveItem(&pending_reads, &net->socket, intcompare);

	if (net->ssl != NULL)
	{
		rc = mbedtls_ssl_close_notify(net->ssl);
		mbedtls_ssl_free(net->ssl);
		free(net->ssl);
		net->ssl = NULL;
	}
	SSLSocket_destroyContext(net);
	FUNC_EXIT_RC(rc);
	return rc;
}

static int SSLSocket_buflenCheck(const iobuf* iovec)
{

	if (iovec->iov_len > SSL_MAX_COUNT)
	{
		return SOCKET_ERROR;
	}

#if defined(SEND_MAX_LEN)
	if (iovec->iov_len + SSL_HEADER_LEN > SEND_MAX_LEN)
	{
		char array[LOG_LEN];
		int rc = sprintf_s(array, LOG_LEN, "[Error]:Please don't send a message longer than %d bytes."
				" Message length which contains header and payload is %u bytes.\r\n", SEND_MAX_LEN,
				iovec->iov_len + SSL_HEADER_LEN);
		if (rc != -1)
			app_at_send_at_rsp_string_lines_with_claim_and_log_restricted(array);
		return  EXT_SOCKET_RET_MESSAGE_TOO_LONG;
	}
#endif
	return 0;
}

static int SSLSocket_putdatasub(iobuf* iovec, const char* buf0, size_t buf0len, PacketBuffers* bufs)
{
	int i;
	char *ptr;
	int len = 0;
	int mem_ret = -1;

	ptr = iovec->iov_base = (char *)malloc(iovec->iov_len);
	len = iovec->iov_len;
	if (ptr == NULL)
	{
		return PAHO_MEMORY_ERROR;
	}
	mem_ret = memcpy_s(ptr, len, buf0, buf0len);
	if (mem_ret != 0)
	{
		free(iovec->iov_base);
		iovec->iov_base = NULL;
		return PAHO_MEMORY_ERROR;
	}
	ptr += buf0len;
	len -= buf0len;
	for (i = 0; i < bufs->count; i++)
	{
		if (bufs->buffers[i] != NULL && bufs->buflens[i] > 0)
		{
			mem_ret = memcpy_s(ptr, len, bufs->buffers[i], bufs->buflens[i]);
			if (mem_ret != 0)
			{
				free(iovec->iov_base);
				iovec->iov_base = NULL;
				return PAHO_MEMORY_ERROR;
			}
			ptr += bufs->buflens[i];
			len -= bufs->buflens[i];
		}
	}
	return 0;
}

static int SSL_write(SSL* ssl, int socket, iobuf* iovec)
{
	int rc;
	if ((rc = mbedtls_ssl_write(ssl, iovec->iov_base, iovec->iov_len)) == (int)iovec->iov_len)
	{
		return TCPSOCKET_COMPLETE;
	}
	else
	{
		Log(TRACE_MIN, -1, "[%s,%d]rc = %d", __func__, __LINE__, rc);
		if (rc == MBEDTLS_ERR_SSL_WANT_READ ||
			rc == MBEDTLS_ERR_SSL_WANT_WRITE ||
			rc == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS ||
			rc == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS ||
			rc == MBEDTLS_ERR_SSL_CLIENT_RECONNECT)
		{
			int* sockmem = (int*)malloc(sizeof(int));
			int free = 1;

			if (sockmem != NULL)
			{
				Log(TRACE_MIN, -1, "Partial write: incomplete write of %d bytes on SSL socket %d",
					iovec->iov_len, socket);
				SocketBuffer_pendingWrite(socket, ssl, 1, iovec, &free, iovec->iov_len, 0);
				*sockmem = socket;
				ListAppend(mod_s.write_pending, sockmem, sizeof(int));
#if defined(USE_SELECT)
				FD_SET(socket, &(mod_s.pending_wset));
#endif
				return TCPSOCKET_INTERRUPTED;
			}
			else
			{
				return PAHO_MEMORY_ERROR;
			}
		}
#if defined(IOT_CONNECT)
		else if (rc == EXT_SOCKET_RET_SLIDING_WINDOW_FULL)
		{
			return EXT_SOCKET_RET_SLIDING_WINDOW_FULL;
		}
#endif
		else
		{
			return SOCKET_ERROR;
		}
	}
	return 0;
}

/* No SSL_writev() provided by OpenSSL. Boo. */
int SSLSocket_putdatas(SSL* ssl, int socket, char* buf0, size_t buf0len, PacketBuffers bufs)
{
	int rc = 0;
	int i;
	iobuf iovec;

	if (ssl == NULL || buf0 == NULL || bufs.buffers == NULL || bufs.buflens == NULL || bufs.frees == NULL)
	{
		rc = SOCKET_ERROR;
		goto exit;
	}
	if (bufs.count > SSL_MAX_COUNT)
	{
		rc = SOCKET_ERROR;
		goto exit;
	}

	FUNC_ENTRY;
	iovec.iov_len = (ULONG)buf0len;
	for (i = 0; i < bufs.count; i++)
		iovec.iov_len += (ULONG)bufs.buflens[i];

	rc = SSLSocket_buflenCheck(&iovec);
	if (rc != 0)
		goto exit;

	rc = SSLSocket_putdatasub(&iovec, buf0, buf0len, &bufs);
	if (rc != 0)
		goto exit;

	SSL_lock_mutex(&sslCoreMutex);
	rc = SSL_write(ssl, socket, &iovec);
	SSL_unlock_mutex(&sslCoreMutex);

	if (rc != TCPSOCKET_INTERRUPTED)
	{
		free(iovec.iov_base);
		iovec.iov_base = NULL;
	}
	else
	{
		free(buf0);
		for (i = 0; i < bufs.count; ++i)
		{
			if (bufs.frees[i] != 0)
			{
				free(bufs.buffers[i]);
				bufs.buffers[i] = NULL;
			}
		}
	}

exit:
	FUNC_EXIT_RC(rc);
	return rc;
}


void SSLSocket_addPendingRead(int sock)
{
	FUNC_ENTRY;
	if (ListFindItem(&pending_reads, &sock, intcompare) == NULL) /* make sure we don't add the same socket twice */
	{
		int* psock = (int*)malloc(sizeof(sock));
		if (psock != NULL)
		{
			*psock = sock;
			ListAppend(&pending_reads, psock, sizeof(sock));
		}
	}
	else
	{
		Log(TRACE_MIN, -1, "SSLSocket_addPendingRead: socket %d already in the list", sock);
	}

	FUNC_EXIT;
}


int SSLSocket_getPendingRead(void)
{
	int sock = -1;

	if (pending_reads.count > 0)
	{
		sock = *(int*)(pending_reads.first->content);
#if defined(IOT_CONNECT) || defined(IOT_LITEOS_ADAPT)
		_ListRemoveHead(&pending_reads); // conflict with libbt_host.a
#else
		ListRemoveHead(&pending_reads);
#endif
	}
	return sock;
}


int SSLSocket_continueWrite(pending_writes* pw)
{
	int rc = 0;
	if (pw == NULL)
		return rc;

	#if defined(SEND_MAX_LEN)
		if (pw->iovecs[0].iov_len + SSL_HEADER_LEN > SEND_MAX_LEN)
		{
			char array[LOG_LEN];
			int rc = sprintf_s(array, LOG_LEN, "[Error]:Please don't send a message longer than %d bytes."
					" Message length which contains header and payload is %u bytes.\r\n", SEND_MAX_LEN,
					pw->iovecs[0].iov_len + SSL_HEADER_LEN);
			if (rc != -1)
				app_at_send_at_rsp_string_lines_with_claim_and_log_restricted(array);
			return EXT_SOCKET_RET_MESSAGE_TOO_LONG;
		}
	#endif

	FUNC_ENTRY;
	if ((rc = mbedtls_ssl_write(pw->ssl, pw->iovecs[0].iov_base, pw->iovecs[0].iov_len)) == (int)pw->iovecs[0].iov_len)
	{
		/* topic and payload buffers are freed elsewhere, when all references to them have been removed */
		free(pw->iovecs[0].iov_base);
		pw->iovecs[0].iov_base = NULL;
		Log(TRACE_MIN, -1, "SSL continueWrite: partial write now complete for socket %d", pw->socket);
		rc = 1;
	}
	else
	{
		Log(TRACE_MIN, -1, "[%s,%d]rc = %d", __func__, __LINE__, rc);
		if (rc == MBEDTLS_ERR_SSL_WANT_READ ||
			rc == MBEDTLS_ERR_SSL_WANT_WRITE ||
			rc == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS ||
			rc == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS ||
			rc == MBEDTLS_ERR_SSL_CLIENT_RECONNECT)
			rc = 0; /* indicate we haven't finished writing the payload yet */
	}
	FUNC_EXIT_RC(rc);
	return rc;
}

#endif
