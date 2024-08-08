/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: In mbedtls lib, adaption for polarssl.
 */
#include "common.h"
#if defined(VENDOR_POLARSSL_API_C)
#include "mbedtls/x509_crl.h"
#include "mbedtls/hw_polarssl_api.h"
#include "mbedtls/debug.h"
#include "mbedtls/error.h"
#include "mbedtls/platform.h"
#include "ssl_misc.h"
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#ifndef O_NONBLOCK
#define O_NONBLOCK        00004000
#endif

typedef struct {
    unsigned int versions[4];
    unsigned int cipher_conf;
    unsigned int version_conf;
    unsigned int kex_size_conf;
    unsigned int new_board_conf;
} ssl_config_data_s;

/* 配置的版本信息，分别为TLSv1_2 TLSv1_3 */
static ssl_config_data_s gst_ssl_config = { {1, 1, 1, 1}, 1 , 1, 0, 0 };

mbedtls_ssl_sni sni_use;
static int g_enable_caupdate = 0;  /* CA证书更新功能的开关 */
static int g_ignore_checktime = 1;
static PdtCertTimeCheckFunc g_pdtCertTimeCheckFunc = NULL;
static PdtCrlTimeCheckFunc g_pdtCrlTimeCheckFunc = NULL;

/* 设置sni */
int polarssl_ssl_set_sni(mbedtls_ssl_adapt *ssl, const char *hostname)
{
    (void)ssl;
    if (strlen(hostname) >= MAX_SNI_LEN) {
        return 1;
    }
    sni_use.send_sni = 1;
    strncpy(sni_use.hostname, hostname, sizeof(sni_use.hostname) - 1);
    return 0;
}

/* 清理sni */
static void polarssl_ssl_clear_sni()
{
    memset(&sni_use, 0 , sizeof(mbedtls_ssl_sni));
}

static int ssl_deprecated_cipher_support(void)
{
    return gst_ssl_config.cipher_conf;
}

int ssl_deprecated_kexsize_support(void)
{
    return gst_ssl_config.kex_size_conf;
}

static int ssl_deprecated_newboard_support(void)
{
    return gst_ssl_config.new_board_conf;
}

int ssl_init_version(void)
{
    ssl_config_data_s ssl_config = { 0 };
    FILE *file = fopen("/var/hw_ssl_version", "r");
    if (file == NULL) {
        return 0;
    }

    (void)fread(&ssl_config, sizeof(ssl_config_data_s), 1, file);
    fclose(file);

    (void)memcpy(&gst_ssl_config, &ssl_config, sizeof(ssl_config));

    return 0;
}

/* 设置不校验时间标志 */
int SetIgnoreTime(int value)
{
    g_ignore_checktime = value;
    return 0;
}

int polarssl_get_checktime(void)
{
    /* 如果产品没有通过sntp或者olt完成时间同步，直接返回0认为时间校验通过 */
    if (access("/var/timesyncflag", F_OK) != 0) {
        return 0;
    }

    return 1;
}

/* 校验证书有效期的回调函数 */
int polarssl_cert_date_checker(mbedtls_x509_crt *crt)
{
    mbedtls_printf( "Use custome CRT time verification callback" );

    if ( polarssl_ischeck_crttime( crt ) == 0 )
        return 0;

    if( mbedtls_x509_time_is_past( &crt->valid_to ) )
        return MBEDTLS_X509_BADCERT_EXPIRED;

    if( mbedtls_x509_time_is_future( &crt->valid_from ) )
        return MBEDTLS_X509_BADCERT_FUTURE;

    return 0;
}

/* 校验CRL有效期的回调函数 */
int polarssl_crl_date_checker(mbedtls_x509_crl *crl)
{
    mbedtls_printf( "Use custome CRL time verification callback" );

    if ( polarssl_ischeck_crltime( crl ) == 0 )
        return 0;

    if( mbedtls_x509_time_is_past( &crl->next_update ) )
        return MBEDTLS_X509_BADCRL_EXPIRED;

    if( mbedtls_x509_time_is_future( &crl->this_update ) )
        return MBEDTLS_X509_BADCRL_FUTURE;

    return 0;
}

int SetPdtCertTimeCheckFunc(PdtCertTimeCheckFunc func)
{
    g_pdtCertTimeCheckFunc = func;
    hw_mbedtls_regist_cert_date_chcker( polarssl_cert_date_checker );
    return 0;
}

int polarssl_ischeck_crttime(mbedtls_x509_crt *crt)
{
    /* 如果没有设置产品决策函数，则默认校验证书时间 */
    if (g_pdtCertTimeCheckFunc == NULL) {
        return 1;
    }

    return g_pdtCertTimeCheckFunc((const mbedtls_x509_crt *)crt);
}

int SetPdtCrlTimeCheckFunc(PdtCrlTimeCheckFunc func)
{
    g_pdtCrlTimeCheckFunc = func;
    hw_mbedtls_regist_crl_date_chcker( polarssl_crl_date_checker );
    return 0;
}

int polarssl_ischeck_crltime(mbedtls_x509_crl *crl)
{
    /* 如果没有设置产品决策函数，则默认校验证书吊销文件时间 */
    if (g_pdtCrlTimeCheckFunc == NULL) {
        return 1;
    }

    return g_pdtCrlTimeCheckFunc((const mbedtls_x509_crl *)crl);
}

/* 安全随机函数 */
unsigned int get_random()
{
    /* 首先从random中获取，若没有就从urandom中获取 */
    const char *randomPath[] = {
        "/dev/random",
        "/dev/urandom"
    };
    int randomFlag[] = {
        O_RDONLY | O_NONBLOCK, /* 对于/dev/random 要设置非阻塞方式 */
        O_RDONLY /* /dev/urandom 本来就是非阻塞的, 直接读取就行了 */
    };

    unsigned int random = 0;
    for (unsigned int loop = 0; loop < 2; loop++) {
        int fd = open(randomPath[loop], randomFlag[loop], 0);
        if (fd != -1) {
            int readBytes = read(fd, (void *)&random, sizeof(unsigned int));
            close(fd);
            if (readBytes == (int)sizeof(unsigned int)) {
                break;
            }
        }
    }

    return random;
}

/* random函数 */
static int myrand( void *rng_state, unsigned char *output, size_t len )
{
    size_t use_len;
    int rnd;

    if (rng_state != NULL)
        rng_state = NULL;

    while (len > 0) {
        use_len = len;
        if (use_len > sizeof(int))
            use_len = sizeof(int);

        rnd = (int)get_random();
        memcpy(output, &rnd, use_len);
        output += use_len;
        len -= use_len;
    }

    return 0;
}

/* 设置协商时的算法集合,外部请使用静态变量 */
void polarssl_ssl_conf_ciphersuites(mbedtls_ssl_config_adapt *conf, const int *ciphersuites)
{
    if (conf == NULL || ciphersuites == NULL) {
        return ;
    }

    mbedtls_ssl_conf_ciphersuites(conf->config, ciphersuites);
}

/*
 * Derive and export the shared secret (G^Y)^X mod P
 */
int polarssl_dhm_calc_secret(mbedtls_dhm_context *ctx, unsigned char *output, size_t output_size, unsigned int *olen)
{
    return mbedtls_dhm_calc_secret(ctx, output, output_size, (size_t*)olen, myrand, NULL);
}

/*
 * Import X from unsigned binary data, big endian
 */
int polarssl_mpi_read_binary(mbedtls_mpi *X, const unsigned char *buf, unsigned int buflen)
{
    return mbedtls_mpi_read_binary(X, buf, buflen);
}

/*
 * Export X into unsigned binary data, big endian
 */
int polarssl_mpi_write_binary(const mbedtls_mpi *X, unsigned char *buf, size_t buflen)
{
    return mbedtls_mpi_write_binary(X, buf, buflen);
}

/* setAesKey */
int polarssl_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits)
{
    return mbedtls_aes_setkey_enc(ctx, key, keybits);
}

/*
 * AES-CBC buffer encryption/decryption
 */
int polarssl_aes_crypt_cbc(mbedtls_aes_context *ctx, int mode, unsigned int length, unsigned char iv[16],
                           const unsigned char *input, unsigned char *output)
{

    return mbedtls_aes_crypt_cbc(ctx,mode,length,iv,input,output);
}

/* setAesKey */
int polarssl_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits)
{

    return mbedtls_aes_setkey_dec(ctx, key, keybits);
}


/* init the para */
void polarssl_aes_init(mbedtls_aes_context *ctx)
{
    mbedtls_aes_init(ctx);
}

/*
 * Create own private value X and export G^X
 */
int polarssl_dhm_make_public(mbedtls_dhm_context *ctx, int x_size, unsigned char *output, size_t olen)
{
    return mbedtls_dhm_make_public(ctx, x_size, output, olen, myrand, NULL);
}

/*
 * Return the total size in bytes
 */
size_t polarssl_mpi_size(const mbedtls_mpi *X)
{
    return mbedtls_mpi_size(X);
}

/*
 * Import from an ASCII string
 */
int polarssl_mpi_read_string(mbedtls_mpi *X, int radix, const char *s)
{
    return mbedtls_mpi_read_string(X, radix, s);
}

/* init the dh */
void polarssl_dhm_init(mbedtls_dhm_context *ctx)
{
    mbedtls_dhm_init(ctx);
}

/*
 * Free the components of a DHM key
 */
void polarssl_dhm_free(mbedtls_dhm_context *ctx)
{
    mbedtls_dhm_free(ctx);
}

/* free pk */
void polarssl_pk_free(mbedtls_pk_context *ctx)
{
    mbedtls_pk_free(ctx);
}

/* sha512 */
void polarssl_sha512(const unsigned char *input, size_t ilen, unsigned char output[64], int is384)
{
    mbedtls_sha512(input, ilen, output, is384);
}


/* hmac512 */
int polarssl_md_hmac512(const unsigned char *key, size_t keylen, const unsigned char *input, size_t ilen,
                        unsigned char *output)
{
    return mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA512), key, keylen, input, ilen, output);
}

/* 通知对端关闭连接 */
int polarssl_shutdown(mbedtls_ssl_adapt *ssl)
{
    if (ssl == NULL) {
        return -1;
    }
    return mbedtls_ssl_close_notify(ssl->ssl);
}

/* 校验证书日期是否有效 */
int polarssl_x509_crt_check_date(const char* filename)
{
    return mbedtls_x509_crt_check_date(filename);
}

/* free crt 证书 */
void polarssl_x509_crt_free(mbedtls_x509_crt *crt)
{
    if (crt == NULL) {
        return ;
    }
    mbedtls_x509_crt_free(crt);
    mbedtls_free(crt);
    crt = NULL;
}

/* 获取debug Level等级 */
static int polarssl_get_debug_level()
{
    int debug_level = 0;

    FILE *fp=fopen("/var/ssl_debug_level", "r");
    if (!fp) {
        return debug_level;
    }
    fscanf(fp, "%d", &debug_level);
    fclose(fp);
    
    return debug_level;
}

/* SSL accept函数 */
int polarssl_accept(mbedtls_ssl_adapt *ssl)
{
    if (ssl == NULL) {
        return -1;
    }
#if defined(VENDOR_POLARSSL_API_DEBUG_C)
    mbedtls_debug_set_threshold(polarssl_get_debug_level());
#endif
    return mbedtls_ssl_handshake(ssl->ssl);
}

/* SSL accept函数 */
int polarssl_connect(mbedtls_ssl_adapt *ssl)
{
    if (ssl == NULL) {
        return -1;
    }
#if defined(VENDOR_POLARSSL_API_DEBUG_C)
    mbedtls_debug_set_threshold(polarssl_get_debug_level());
#endif

    /* 如果使能的缓存恢复缓存 */
    if (ssl->saved_session != NULL) {
        mbedtls_ssl_set_session(ssl->ssl, ssl->saved_session);
    }

    int32_t ret = mbedtls_ssl_handshake(ssl->ssl);
    if (ret == 0) {
        /* 成功则记录下缓存 */
        if (ssl->saved_session != NULL) {
            mbedtls_ssl_get_session(ssl->ssl, ssl->saved_session);
        }
    }

    return ret;
}

/* SSL free函数 */
void polarssl_free(mbedtls_ssl_adapt *ssl)
{
    if (ssl == NULL) {
        return;
    }

    if (ssl->saved_session != NULL) {
        mbedtls_ssl_session_free(ssl->saved_session);
        mbedtls_free(ssl->saved_session);
        ssl->saved_session = NULL;
    }


    mbedtls_ssl_free(ssl->ssl);
    mbedtls_free(ssl->ssl);
    ssl->ssl = NULL;
    mbedtls_free(ssl);
    ssl = NULL;
}

/* SSL 配置设置 */
static int polarssl_ssl_config_defaults(mbedtls_ssl_config *conf, int type)
{
  return  mbedtls_ssl_config_defaults(conf, type, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
}

/* 使能服务端的缓存,失败也不影响功能 */
void polarssl_enable_server_cache(mbedtls_ssl_config_adapt *conf, uint32_t cache_num, uint32_t timeout)
{
    if (conf == NULL || conf->config == NULL) {
        return;
    }

    if (conf->cache == NULL) {
        conf->cache = (mbedtls_ssl_cache_context *)mbedtls_calloc(1, sizeof(mbedtls_ssl_cache_context));
        if (conf->cache != NULL) {
            mbedtls_ssl_cache_init(conf->cache);
            mbedtls_ssl_cache_set_max_entries(conf->cache, cache_num);
            mbedtls_ssl_cache_set_timeout(conf->cache, timeout);
            mbedtls_ssl_conf_session_cache(conf->config, conf->cache, mbedtls_ssl_cache_get, mbedtls_ssl_cache_set);
        }
    }

}

/* 使能客户端缓存 */
void polarssl_enable_client_cache(mbedtls_ssl_adapt *ssl)
{
    if (ssl->saved_session == NULL) {
        /* calloc会memset 0 */
        ssl->saved_session = mbedtls_calloc(1, sizeof(mbedtls_ssl_session));
    }

}

/* SSL配置变量释放 */
void polarssl_free_config_adapt(mbedtls_ssl_config_adapt *conf)
{  
    if (conf == NULL) {
        return;
    }

    if (conf->config != NULL) {
        if (conf->config->MBEDTLS_PRIVATE(ca_crl) != NULL) {
            mbedtls_x509_crl_free(conf->config->MBEDTLS_PRIVATE(ca_crl));
            mbedtls_free(conf->config->MBEDTLS_PRIVATE(ca_crl));
            conf->config->MBEDTLS_PRIVATE(ca_crl) = NULL;
        }
        mbedtls_ssl_config_free(conf->config);
        mbedtls_free(conf->config);
        conf->config = NULL;
    }

    if (conf->entropy != NULL) {
        mbedtls_entropy_free(conf->entropy);
        mbedtls_free(conf->entropy);
        conf->entropy = NULL;
    }

    if (conf->ctr_drbg != NULL) {
        mbedtls_ctr_drbg_free(conf->ctr_drbg);
        mbedtls_free(conf->ctr_drbg);
        conf->ctr_drbg = NULL;
    }

    if (conf->prv != NULL) {
        mbedtls_pk_free(conf->prv);
        mbedtls_free(conf->prv);
        conf->prv = NULL;
    }

    if (conf->pubcert != NULL) {
        mbedtls_x509_crt_free(conf->pubcert);
        mbedtls_free(conf->pubcert);
        conf->pubcert = NULL;
    }

    if (conf->chainHead != NULL) {
        mbedtls_x509_crt_free(conf->chainHead);
        mbedtls_free(conf->chainHead);
        conf->chainHead = NULL;
    }

    if (conf->cache != NULL) {
        mbedtls_ssl_cache_free(conf->cache);
        mbedtls_free(conf->cache);
        conf->cache = NULL;
    }

    mbedtls_free(conf);
    conf = NULL;
}

/* 设置CN域名，后面会继续校验,同时会发送SNI */
int polarssl_ssl_set_hostname(mbedtls_ssl_adapt *ssl, const char *hostname)
{
   if (ssl == NULL) {
       return -1;
   }
   return mbedtls_ssl_set_hostname(ssl->ssl,hostname);
}

/* 设置认证模式 */
void polarssl_conf_authmode(mbedtls_ssl_config_adapt *conf, int authmode)
{
    if (conf == NULL) {
        return ;
    }    
    mbedtls_ssl_conf_authmode(conf->config, authmode);
}

/* hmac包装，摘要方法为sha1 */
int polarssl_hmac_with_sha1(const unsigned char *key, size_t keylen,
                            const unsigned char *input, size_t ilen,
                            unsigned char *output,size_t * outlen)
{
    *outlen = 20;
    return mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), key, keylen, input, ilen, output);
}

/* hmac包装，摘要方法为sha256 */
int polarssl_hmac_with_sha256(const unsigned char *key, size_t keylen,
                              const unsigned char *input, size_t ilen,
                              unsigned char *output,size_t outlen)
{
    outlen = outlen;
    return mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), key, keylen, input, ilen, output);
}

/* 根据文件解析crt/der格式的证书,chain是个链表，如果已存在放在则放在其next里面 */
mbedtls_x509_crt *polarssl_x509_crt_parse_file(const char *path)
{
    mbedtls_x509_crt *crt = mbedtls_calloc(1, sizeof(mbedtls_x509_crt));
    if (crt == NULL) {
        return NULL;
    }

    memset(crt, 0, sizeof(mbedtls_x509_crt));
    int ret = mbedtls_x509_crt_parse_file(crt, path);
    if (ret != 0) {
        mbedtls_free(crt);
        crt = NULL;
    }

    return crt;
}

void polarssl_pem_free(mbedtls_pem_context *ctx)
{
    mbedtls_pem_free(ctx);  
}

/* 解析PUBLIC KEY证书 */
int polarssl_pem_read_buffer(mbedtls_pem_context *pem, unsigned char *pub)
{
    size_t use_len = 0;
    return mbedtls_pem_read_buffer(pem, "-----BEGIN PUBLIC KEY-----", "-----END PUBLIC KEY-----",
        pub, NULL, 0, &use_len);
}


/* 从der格式的公钥中解析出N/E rsa key，要求der格式简单，只有最内一层 */
int polarssl_pk_parse_subpubkey(unsigned char *buf, int len, mbedtls_pk_context *pk)
{
    return mbedtls_pk_parse_subpubkey(&buf, buf + len, pk);
}

/* 将一套pub/priv key设置到其中 */
int polarssl_set_pub_prv_to_conf(mbedtls_ssl_config_adapt *conf, const char *prvPath,const char *pubPath,const char *prvPassword)
{
    if (conf == NULL) {
        return -1;
    }

    conf->pubcert = mbedtls_calloc(1, sizeof(mbedtls_x509_crt));
    if (conf->pubcert == NULL) {
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }

    conf->prv = mbedtls_calloc(1, sizeof(mbedtls_pk_context));
    if(conf->prv == NULL) {
        mbedtls_free(conf->pubcert);
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }

    int ret  = mbedtls_x509_crt_parse_file(conf->pubcert, pubPath);
    ret |= mbedtls_pk_parse_keyfile(conf->prv, prvPath, prvPassword, mbedtls_ctr_drbg_random, conf->ctr_drbg);
    ret |= mbedtls_ssl_conf_own_cert(conf->config, conf->pubcert, conf->prv);

    return ret;
}

/* SSL的read函数 */
int polarssl_ssl_read(mbedtls_ssl_adapt *ssl, unsigned char *buf, size_t len)
{
    if (ssl == NULL) {
        return -1;
    }
    return mbedtls_ssl_read(ssl->ssl, buf, len);
}

/* SSL的write函数 */
int polarssl_ssl_write(mbedtls_ssl_adapt *ssl, const unsigned char *buf, size_t len)
{
    if (ssl == NULL) {
        return -1;
    }
    return mbedtls_ssl_write(ssl->ssl, buf, len);
}

/* SSL获取version */
const char *polarssl_ssl_get_version(const mbedtls_ssl_adapt *ssl)
{
    if (ssl == NULL) {
        return NULL;
    }
    return mbedtls_ssl_get_version(ssl->ssl);
}

/* hash256 摘要下 x509 rsa证书校验判断 */
int polarssl_rsassl_vertify(unsigned char *sig, unsigned int siglen, unsigned char *data, unsigned int datalen,
                            mbedtls_rsa_context *rsa)
{
    unsigned char hash[1024] = {0}; /* Enough for any hash */
    siglen = siglen;
    datalen= datalen;

    int ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), data, datalen, hash);
    if (ret != 0) {
        return ret;
    }

    return mbedtls_rsa_pkcs1_verify(rsa, MBEDTLS_MD_SHA256, 32, hash, sig);
}

/* 将cafile和cadir目录下的所有文件设置到conf中去,没有有效证书则失败 */
int polarssl_conf_setCaList(mbedtls_ssl_config_adapt *conf, char *cafile, char *cadir, char *crlFile)
{
    if (conf == NULL) {
        return -1;
    }

    conf->chainHead = mbedtls_calloc(1, sizeof(mbedtls_x509_crt));
    if (conf->chainHead == NULL) {
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }

    if (cafile != NULL) {
        mbedtls_x509_crt_parse_file(conf->chainHead, cafile);
    }

    if (cadir != NULL) {
        void *dir = opendir(cadir);
        if (dir == NULL) {
            return 1;
        }

        char path[512] = {0};
        struct dirent *filename = NULL;
        while ((filename = readdir(dir)) != NULL) {
            memset(path, 0, sizeof(path));
            if(!strcmp(filename->d_name,".") || !strcmp(filename->d_name,".."))
                continue;
            (void)sprintf(path,"%s/%s",cadir,filename->d_name);
            mbedtls_x509_crt_parse_file(conf->chainHead, path);
        }

        closedir(dir);
    }

    mbedtls_x509_crl *crl = NULL;
    if ((crlFile != NULL) && (access(crlFile, F_OK) == 0)) { /* crl 文件存在 */
        crl = mbedtls_calloc(1, sizeof(mbedtls_x509_crl));
        if (crl == NULL) {
            return MBEDTLS_ERR_SSL_ALLOC_FAILED;
        }

        mbedtls_x509_crl_init(crl);
        int32_t rc = mbedtls_x509_crl_parse_file(crl, crlFile);
        if (rc != 0) {
            mbedtls_free(crl);
            return rc;
        }
    }

    mbedtls_ssl_conf_ca_chain(conf->config, conf->chainHead, crl);

    return 0;
}

/* 获取ssl连接中使用的加密算法 */
const char *polarssl_ssl_get_ciphersuite(const mbedtls_ssl_adapt *ssl)
{
    if (ssl == NULL) {
        return NULL;
    }

    return mbedtls_ssl_get_ciphersuite(ssl->ssl);
}

/* scp中的特殊业务流程 */
void polarssl_enable_ca_update(int enable)
{
     g_enable_caupdate = enable;
}

int polarssl_get_caupdate(void)
{
    return g_enable_caupdate;
}

/* 将socket设置到ssl连接中 */
void polarssl_ssl_set_bio(mbedtls_ssl_adapt *ssl, int fd)
{
     if (ssl == NULL) {
         return ;
     }
     ssl->bio.fd = fd;
     mbedtls_ssl_set_bio(ssl->ssl,&ssl->bio, mbedtls_net_send, mbedtls_net_recv, NULL);
}

/* debug函数 */
static void mbedtls_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    ((void)level);

    mbedtls_fprintf((FILE *) ctx, "%s:%04d: %s", file, line, str);
    fflush((FILE *)ctx);
}

/* 证书校验适配放开,不然证书算法太弱会拒绝连接 */
const mbedtls_x509_crt_profile easyConnect = {
    /* 根据需要适当限制，当前放开 */
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA1 ) |

#if defined(MBEDTLS_MD2_C) 
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_MD2 ) |
#endif
#if defined(MBEDTLS_MD4_C) 
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_MD4 ) |
#endif

    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_MD5 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA224 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA256 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA384 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA512 ),
    0xFFFFFFF, /* Any PK alg    */
    0xFFFFFFF, /* Any curve     */
    2048, /* 3.1.0版本，rsa最小支持2048bit */ 
};

/* 默认 */
const int ssl_ciphers[] = {
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
    MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA,
    MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA,    
    MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA,
    MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA256,
    MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA256 ,
    MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256,
    MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA,
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384,
    0
};

/* 符合公司安全默认用加密套件 */
const int g_sslSafeCiphers[] = {
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
    MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA,
    MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA,
    MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256,
    MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA,
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384,
    0
};

const int g_sslUdmCiphers[] = {
    MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA,
    MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA,
    MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256,
    MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256,
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA,
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384,
    0
};

/* TLS1.3 加密套件 */
const int g_tls13Ciphers[] = {
    MBEDTLS_TLS1_3_AES_128_GCM_SHA256,
    MBEDTLS_TLS1_3_AES_256_GCM_SHA384,
    0
};

/* 按照公司要求，新单板删除CBC加密套， 保留GCM加密套 */
const int g_sslGcmCiphers[] = {
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
    0
};

/* 埃及VDF要求支持安全等级高且算法长度大于128的算法 */
const int g_sslHighCiphers[] = {
    MBEDTLS_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384,
    MBEDTLS_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
    0
};

void mbedtls_set_udm_ssl_conf(mbedtls_ssl_config_adapt *config)
{
    if (config == NULL) {
        return;
    }
 
    mbedtls_ssl_conf_ciphersuites(config->config, g_sslUdmCiphers);
    return;
}

/* 配合cyassl连接模型 适配函数 */
mbedtls_ssl_config_adapt *polarssl_new_ctx_ssl_conf(int connect_type)
{
    const char *pers = (connect_type == MBEDTLS_SSL_IS_CLIENT) ? "ssl_client":"ssl_server";

    mbedtls_ssl_config_adapt *config = mbedtls_calloc(1, sizeof(mbedtls_ssl_config_adapt));
    if (config == NULL) {
        return NULL;
    }

    config->entropy = mbedtls_calloc(1, sizeof(mbedtls_entropy_context));
    if (config->entropy == NULL) {
        mbedtls_free(config);
        return NULL;
    }

    config->ctr_drbg = mbedtls_calloc(1, sizeof(mbedtls_ctr_drbg_context));
    if (config->ctr_drbg == NULL) {
        mbedtls_free(config->entropy);
        mbedtls_free(config);
        return NULL;
    }

    config->config = mbedtls_calloc(1, sizeof(mbedtls_ssl_config));
    if (config->config == NULL) {
        mbedtls_free(config->ctr_drbg);
        mbedtls_free(config->entropy);
        mbedtls_free(config);
        return NULL;
    }

    /* 初始化协议 */
    ssl_init_version();
    /* 清理sni */
    polarssl_ssl_clear_sni();

    mbedtls_entropy_init(config->entropy);
    mbedtls_ctr_drbg_init(config->ctr_drbg);
    mbedtls_ssl_config_init(config->config);
    (void)polarssl_ssl_config_defaults(config->config,connect_type);
    (void)mbedtls_ssl_conf_cert_profile(config->config, &easyConnect);
    (void)mbedtls_ctr_drbg_seed(config->ctr_drbg, mbedtls_entropy_func, config->entropy,
        (const unsigned char *)pers, strlen(pers));

    if (polarss_feather_support("FT_SSL_CIPHERSUITE_HIGH")) {
        mbedtls_ssl_conf_ciphersuites(config->config, g_sslHighCiphers);
    } else {
        if (ssl_deprecated_newboard_support()) {
            mbedtls_ssl_conf_ciphersuites(config->config, g_sslGcmCiphers);
        } else {
            /* 设置默认算法合集 */
            mbedtls_ssl_conf_ciphersuites(config->config, g_sslSafeCiphers);

            /* 特性开关打开则打开,rc4 */
            if (ssl_deprecated_cipher_support()) {
                /* 特性开关控制设置兼容过时算法合集 */
                mbedtls_ssl_conf_ciphersuites(config->config, ssl_ciphers);
            }
        }
    }

    /* dh算法长度校验 */
    if (ssl_deprecated_kexsize_support() == 1) {
        mbedtls_ssl_conf_dhm_min_bitlen(config->config, 0);
    }

    /* mbedtls 只能使用TLS1.2或者TLS1.3  服务端暂不支持1.3， 客户端支持 */
    mbedtls_ssl_conf_max_version(config->config, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
    mbedtls_ssl_conf_min_version(config->config, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
    if ((access("/var/force_tls13", F_OK) == 0) && (connect_type == MBEDTLS_SSL_IS_CLIENT)) {
        mbedtls_ssl_conf_ciphersuites(config->config, g_tls13Ciphers);
        mbedtls_ssl_conf_max_version(config->config, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_4);
        mbedtls_ssl_conf_min_version(config->config, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_4);
    }

    /* 设置随机函数 */
    mbedtls_ssl_conf_rng( config->config, mbedtls_ctr_drbg_random, config->ctr_drbg );

    /* 调试手段 */
    mbedtls_ssl_conf_dbg( config->config, mbedtls_debug, stdout );

    /* 设置随机dm */
    mbedtls_dhm_context dhm;
    mbedtls_dhm_init(&dhm);
    int ret = mbedtls_dhm_parse_dhmfile(&dhm, "/mnt/jffs2/dhparams.pem");
    if (ret == 0) {
        mbedtls_ssl_conf_dh_param_ctx(config->config, &dhm);
    }
    mbedtls_dhm_free(&dhm);

    return config;
}

/* 配合cyassl模型创建ssl连接 */
mbedtls_ssl_adapt *polarssl_new_ssl(mbedtls_ssl_config_adapt * config)
{
    if (config == NULL) {
        return NULL;
    }

    mbedtls_ssl_adapt *ssl = mbedtls_calloc(1, sizeof(mbedtls_ssl_adapt));
    if (ssl == NULL) {
        return NULL;
    }

    ssl->ssl = mbedtls_calloc(1, sizeof(mbedtls_ssl_context));
    if (ssl->ssl == NULL) {
        mbedtls_free(ssl);
        return NULL;
    }

    mbedtls_ssl_init(ssl->ssl);
    (void)mbedtls_ssl_setup(ssl->ssl, config->config);

    return ssl;
}

/* 摘要算法合集 */
void polarssl_dgst(int type, unsigned char *input, size_t ilen, unsigned char * output,size_t * olen)
{
    switch (type) {
        case MBEDTLS_MD_MD5:
             mbedtls_md5(input, ilen, output);
             *olen = 16;
             break;
        case MBEDTLS_MD_SHA1:
             mbedtls_sha1(input, ilen, output);
             *olen = 20;
             break;
        default:
            break;
    }

    return ;
}

/* 适配init/update/final摘要算法格式合集 */
int polarssl_dgst_init(mbedtls_md_context_t *ctx, int type)
{
    mbedtls_md_init(ctx);

    int ret = mbedtls_md_setup(ctx, mbedtls_md_info_from_type(type), 0);
    if (ret != 0) {
        return ret;
    }
    mbedtls_md_starts(ctx);

    return 0;
}

/* 适配init/update/final摘要算法格式合集 */
void polarssl_dgst_update(mbedtls_md_context_t *ctx, unsigned char *input, size_t ilen)
{
    mbedtls_md_update(ctx, input, ilen);
}

/* 适配init/update/final摘要算法格式合集 */
void polarssl_dgst_final(mbedtls_md_context_t *ctx, unsigned char *output)
{
    mbedtls_md_finish(ctx, output);
    mbedtls_md_free(ctx);
}

#if defined(VENDOR_POLARSSL_API_FT_C)
#define FT_MAX_LEN 32
/* FEATURE ioctl数据结构 */
typedef struct {
    uint32_t Ret;
    uint8_t EnableFlag; /* 使能和初始化标志 */
    char CustomDefine;        /* 是否支持运营商定制 */
    uint16_t FtID;      /* 最大支持65535个特性开关 */
    char FtName[FT_MAX_LEN];
} HW_OPEN_FT_IOCTL_INFO;

#define HW_FT_CMD_MAGIC 'X'
#define HW_OPEN_FT_IOCTL_CMD_BY_NAME _IOWR(HW_FT_CMD_MAGIC, 4, HW_OPEN_FT_IOCTL_INFO)

/* 特性开关判断 */
int polarss_feather_support(char *featureName)
{
    int fd = open("/proc/wap_proc/feature", O_RDONLY, 0);
    if (fd < 0) {
        return 0;
    }

    HW_OPEN_FT_IOCTL_INFO ft = {0};
    strncpy(ft.FtName, featureName, sizeof(ft.FtName) - 1);

    int ret = ioctl(fd, HW_OPEN_FT_IOCTL_CMD_BY_NAME, &ft);
    close(fd);
    if (ret < 0 || ft.Ret != 0) {
        return 0;
    }

    return ft.EnableFlag;
}

#else
int polarss_feather_support(char *featureName){
    (void)featureName;
    return 0;
}
#endif

/*摘要算法校验 md2,md4算法是否支持 */
int polarssl_dgst_check(mbedtls_md_type_t md_alg)
{
    (void)md_alg;

    /* 不同市场算法不一 特性开关开启的条件下 做校验 -1表示禁止*/
#if defined(MBEDTLS_MD2_C) 
    if (md_alg == MBEDTLS_MD_MD2 || md_alg == MBEDTLS_MD_MD4) {
        if (polarss_feather_support("FT_OLD_MD_DDGST_FORBIDDEN")) {
            return -1;
        }
    }    
#endif

#if defined(MBEDTLS_MD4_C) 
    if (md_alg == MBEDTLS_MD_MD4) {
        if (polarss_feather_support("FT_OLD_MD_DDGST_FORBIDDEN")) {
            return -1;
        }
    }
#endif

    return 0;
}

/* 配置dh参数 */
int  polarssl_ssl_conf_dh_param_ctx(mbedtls_ssl_config_adapt *conf, mbedtls_dhm_context *ctx)
{
    if (conf == NULL || ctx == NULL) {
        return -1;
    }

    return mbedtls_ssl_conf_dh_param_ctx(conf->config, ctx);
}

/* 计算共享密钥 */
int polarssl_ssl_calc_key(const unsigned char *pw, unsigned int pwLen, const unsigned char *salt, unsigned int saltLen,
    unsigned char *key, unsigned int keyLen)
{
    mbedtls_md_context_t shaCtx;
    const mbedtls_md_info_t *shaInfo = NULL;

    mbedtls_md_init(&shaCtx);
    shaInfo = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (shaInfo == NULL) {
        return -1;
    }

    int ret = mbedtls_md_setup(&shaCtx, shaInfo, 1);
    if (ret != 0) {
        mbedtls_md_free(&shaCtx);
        return ret;
    }

    /* pks count至少10000，与服务端保持一致 */
    ret = mbedtls_pkcs5_pbkdf2_hmac(&shaCtx, pw, pwLen, salt, saltLen, 10000, keyLen, key);
    if (ret != 0) {
        mbedtls_md_free(&shaCtx);
        return ret;
    }

    mbedtls_md_free(&shaCtx);

    return (ret);
}

/* 配置共享密钥 */
void polarssl_ssl_conf_psk(mbedtls_ssl_config_adapt *ctx, unsigned char *key, unsigned int keyLen,
    const unsigned char *ident, unsigned int identLen)
{
    (void)mbedtls_ssl_conf_psk(ctx->config, key, keyLen, ident, identLen);
}

int polarssl_pk_parse_keyfile(mbedtls_pk_context *ctx, const char *path, const char *password)
{
    return mbedtls_pk_parse_keyfile(ctx, path, password, myrand, NULL);
}

int polarssl_pk_parse_key(mbedtls_pk_context *ctx, const unsigned char *key, size_t keylen,
    const unsigned char *pwd, size_t pwdlen)
{
    return mbedtls_pk_parse_key(ctx, key, keylen, pwd, pwdlen, myrand, NULL);
}

int polarssl_pk_check_pair(const mbedtls_pk_context *pub, const mbedtls_pk_context *prv)
{
    return mbedtls_pk_check_pair(pub, prv, myrand, NULL);
}


static int pem_pbkdf1( unsigned char *key, size_t keylen,
                       unsigned char *iv,
                       const unsigned char *pwd, size_t pwdlen )
{
    mbedtls_md5_context md5_ctx;
    unsigned char md5sum[16];
    size_t use_len;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    mbedtls_md5_init( &md5_ctx );

    /*
     * key[ 0..15] = MD5(pwd || IV)
     */
    if( ( ret = mbedtls_md5_starts( &md5_ctx ) ) != 0 )
        goto exit;
    if( ( ret = mbedtls_md5_update( &md5_ctx, pwd, pwdlen ) ) != 0 )
        goto exit;
    if( ( ret = mbedtls_md5_update( &md5_ctx, iv,  8 ) ) != 0 )
        goto exit;
    if( ( ret = mbedtls_md5_finish( &md5_ctx, md5sum ) ) != 0 )
        goto exit;

    if( keylen <= 16 )
    {
        memcpy( key, md5sum, keylen );
        goto exit;
    }

    memcpy( key, md5sum, 16 );

    /*
     * key[16..23] = MD5(key[ 0..15] || pwd || IV])
     */
    if( ( ret = mbedtls_md5_starts( &md5_ctx ) ) != 0 )
        goto exit;
    if( ( ret = mbedtls_md5_update( &md5_ctx, md5sum, 16 ) ) != 0 )
        goto exit;
    if( ( ret = mbedtls_md5_update( &md5_ctx, pwd, pwdlen ) ) != 0 )
        goto exit;
    if( ( ret = mbedtls_md5_update( &md5_ctx, iv, 8 ) ) != 0 )
        goto exit;
    if( ( ret = mbedtls_md5_finish( &md5_ctx, md5sum ) ) != 0 )
        goto exit;

    use_len = 16;
    if( keylen < 32 )
        use_len = keylen - 16;

    memcpy( key + 16, md5sum, use_len );

exit:
    mbedtls_md5_free( &md5_ctx );
    mbedtls_platform_zeroize( md5sum, 16 );

    return( ret );
}


static int pem_aes_encrypt(unsigned char *aes_iv, unsigned int keylen, unsigned char *buf, size_t buflen, 
                           const unsigned char *pwd, size_t pwdlen)
{
    int ret;
    unsigned char aes_key[32];
    mbedtls_aes_context aes_ctx;

    mbedtls_aes_init(&aes_ctx);

    ret = pem_pbkdf1(aes_key, keylen, aes_iv, pwd, pwdlen);
    if (ret != 0 )
        goto exit;

    ret = mbedtls_aes_setkey_enc(&aes_ctx, aes_key, keylen * 8);
    if (ret != 0)
        goto exit;

    ret = mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, buflen, aes_iv, buf, buf);

exit:
    mbedtls_aes_free(&aes_ctx);
    mbedtls_platform_zeroize(aes_key, keylen);

    return ret;
}


static int mbedtls_pem_generate_iv(unsigned char *iv, int len)
{
    int ret;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context drbgCtx;

    mbedtls_ctr_drbg_init(&drbgCtx);
    mbedtls_entropy_init(&entropy);

    ret = mbedtls_ctr_drbg_seed(&drbgCtx, mbedtls_entropy_func, &entropy, NULL, 0);
    if (ret != 0)
        goto cleanup;

    ret = mbedtls_ctr_drbg_random(&drbgCtx, iv, len);
    if (ret != 0)
        goto cleanup;

cleanup:
    mbedtls_ctr_drbg_free(&drbgCtx);
    mbedtls_entropy_free(&entropy);
    return ret;
}

int mbedtls_pem_write_crypt_buffer(const char *header, const char *footer,
                                   unsigned char *der_data, size_t der_len,
                                   const unsigned char * pwd, size_t pwdlen, 
                                   unsigned char *buf, size_t buf_len, size_t *olen)
{
    int i;
    int ret;
    char head[256];
    char iv[33];
    unsigned char aes_iv[17];

    ret = mbedtls_pem_generate_iv(aes_iv, 16);
    if (ret != 0)
        return ret;

    memset(iv, 0x00, sizeof(iv));
    for (i = 0; i < 16; i++)
        sprintf(iv + i * 2, "%02X", aes_iv[i]);

    ret = pem_aes_encrypt(aes_iv, 32, der_data, der_len, pwd, pwdlen);
    if (ret != 0)
        return ret;

    memset(buf, 0, buf_len);
    sprintf(head, "%s%s\n\n", header, iv);
    ret = mbedtls_pem_write_buffer(head, footer, der_data, der_len, buf, buf_len, olen);

    return ret;
}

int mbedtls_x509_crt_check_date(const char* filename)
{
    int ret = 0;
    mbedtls_x509_crt crt ;

    memset( &crt, 0, sizeof( mbedtls_x509_crt ) );
    ret = mbedtls_x509_crt_parse_file( &crt, filename );
    if( ret != 0 )
        return ( ret );

    if(polarssl_ischeck_crttime(&crt) && mbedtls_x509_time_is_past(&crt.valid_to))
    {
        mbedtls_x509_crt_free( &crt );
        return ( MBEDTLS_X509_BADCERT_EXPIRED );
    }

    if(polarssl_ischeck_crttime(&crt) && mbedtls_x509_time_is_future(&crt.valid_from))
    {
        mbedtls_x509_crt_free(&crt);
        return ( MBEDTLS_X509_BADCERT_FUTURE );
    }
    mbedtls_x509_crt_free(&crt);

    return ( 0 );
}


#define PEM_ENCRYPT_BEGIN_PRIVATE_KEY_RSA   "-----BEGIN RSA PRIVATE KEY-----\nProc-Type: 4,ENCRYPTED\nDEK-Info: AES-256-CBC,"
#define PEM_END_PRIVATE_KEY_RSA     "-----END RSA PRIVATE KEY-----\n"
#define PEM_ENCRYPT_BEGIN_PRIVATE_KEY_EC    "-----BEGIN EC PRIVATE KEY-----\nProc-Type: 4,ENCRYPTED\nDEK-Info: AES-256-CBC,"
#define PEM_END_PRIVATE_KEY_EC      "-----END EC PRIVATE KEY-----\n"

#if defined(MBEDTLS_RSA_C)
    #define MPI_MAX_SIZE_2          ( MBEDTLS_MPI_MAX_SIZE / 2 + MBEDTLS_MPI_MAX_SIZE % 2 )
    #define RSA_PRV_DER_MAX_BYTES   ( 47 + 3 * MBEDTLS_MPI_MAX_SIZE + 5 * MPI_MAX_SIZE_2 )
#else /* MBEDTLS_RSA_C */
    #define RSA_PRV_DER_MAX_BYTES   0
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_ECP_C)
    #define ECP_PRV_DER_MAX_BYTES   ( 29 + 3 * MBEDTLS_ECP_MAX_BYTES )
#else /* MBEDTLS_ECP_C */
    #define ECP_PRV_DER_MAX_BYTES   0
#endif /* MBEDTLS_ECP_C */
#define PRV_DER_MAX_BYTES   ( RSA_PRV_DER_MAX_BYTES > ECP_PRV_DER_MAX_BYTES ? RSA_PRV_DER_MAX_BYTES : ECP_PRV_DER_MAX_BYTES )

int mbedtls_pk_write_crypt_key_pem(mbedtls_pk_context *key, const unsigned char *pwd, unsigned char *buf, size_t size )
{
    int ret;
    int i;
    size_t olen = 0;
    int plain_len;
    int pad_len;
    unsigned char output_buf[PRV_DER_MAX_BYTES];
    unsigned char plain[PRV_DER_MAX_BYTES];
    const char *begin, *end;

    if ((key == NULL) || (pwd == NULL) || (buf == NULL) || (size == 0))
        return MBEDTLS_ERR_PK_BAD_INPUT_DATA;

    ret = mbedtls_pk_write_key_der(key, output_buf, sizeof(output_buf));
    if (ret < 0)
        return ret;

    plain_len = (ret % 16) == 0 ? ret : ((ret / 16 + 1) * 16);
    memset(plain, 0x00, sizeof(plain));
    memcpy(plain, output_buf + sizeof(output_buf) - ret, ret);
    pad_len = plain_len - ret;

    for (i = 0; i < pad_len; i++)
        plain[ret + i] = pad_len;

    if ( mbedtls_pk_get_type( key ) == MBEDTLS_PK_RSA )
    {
        begin = PEM_ENCRYPT_BEGIN_PRIVATE_KEY_RSA;
        end = PEM_END_PRIVATE_KEY_RSA;
    } 
    else if ( mbedtls_pk_get_type( key ) == MBEDTLS_PK_ECKEY )
    {
        begin = PEM_ENCRYPT_BEGIN_PRIVATE_KEY_EC;
        end = PEM_END_PRIVATE_KEY_EC;
    }
    else {
        return( MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE );
    }

    return mbedtls_pem_write_crypt_buffer(begin, end, plain, plain_len,
                                          pwd, strlen((const char*)pwd), buf, size, &olen );
}

int x509_info_subject_alt_name( char **buf, size_t *size,
                                const mbedtls_x509_sequence *subject_alt_name,
                                const char *prefix )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t n = *size;
    char *p = *buf;
    const mbedtls_x509_sequence *cur = subject_alt_name;
    mbedtls_x509_subject_alternative_name san;
    int parse_ret;

    while( cur != NULL )
    {
        memset( &san, 0, sizeof( san ) );
        parse_ret = mbedtls_x509_parse_subject_alt_name( &cur->buf, &san );
        if( parse_ret != 0 )
        {
            if( parse_ret == MBEDTLS_ERR_X509_FEATURE_UNAVAILABLE )
            {
                ret = mbedtls_snprintf( p, n, "\n%s    <unsupported>", prefix );
                MBEDTLS_X509_SAFE_SNPRINTF;
            }
            else
            {
                ret = mbedtls_snprintf( p, n, "\n%s    <malformed>", prefix );
                MBEDTLS_X509_SAFE_SNPRINTF;
            }
            cur = cur->next;
            continue;
        }

        switch( san.type )
        {
            /*
             * otherName
             */
            case MBEDTLS_X509_SAN_OTHER_NAME:
            {
                mbedtls_x509_san_other_name *other_name = &san.san.other_name;

                ret = mbedtls_snprintf( p, n, "\n%s    otherName :", prefix );
                MBEDTLS_X509_SAFE_SNPRINTF;

                if( MBEDTLS_OID_CMP( MBEDTLS_OID_ON_HW_MODULE_NAME,
                                     &other_name->value.hardware_module_name.oid ) != 0 )
                {
                    ret = mbedtls_snprintf( p, n, "\n%s        hardware module name :", prefix );
                    MBEDTLS_X509_SAFE_SNPRINTF;
                    ret = mbedtls_snprintf( p, n, "\n%s            hardware type          : ", prefix );
                    MBEDTLS_X509_SAFE_SNPRINTF;

                    ret = mbedtls_oid_get_numeric_string( p, n, &other_name->value.hardware_module_name.oid );
                    MBEDTLS_X509_SAFE_SNPRINTF;

                    ret = mbedtls_snprintf( p, n, "\n%s            hardware serial number : ", prefix );
                    MBEDTLS_X509_SAFE_SNPRINTF;

                    if( other_name->value.hardware_module_name.val.len >= n )
                    {
                        *p = '\0';
                        return( MBEDTLS_ERR_X509_BUFFER_TOO_SMALL );
                    }

                    memcpy( p, other_name->value.hardware_module_name.val.p,
                            other_name->value.hardware_module_name.val.len );
                    p += other_name->value.hardware_module_name.val.len;

                    n -= other_name->value.hardware_module_name.val.len;

                }/* MBEDTLS_OID_ON_HW_MODULE_NAME */
            }
            break;

            /*
             * dNSName
             */
            case MBEDTLS_X509_SAN_DNS_NAME:
            {
                ret = mbedtls_snprintf( p, n, "\n%s    dNSName : ", prefix );
                MBEDTLS_X509_SAFE_SNPRINTF;
                if( san.san.unstructured_name.len >= n )
                {
                    *p = '\0';
                    return( MBEDTLS_ERR_X509_BUFFER_TOO_SMALL );
                }

                memcpy( p, san.san.unstructured_name.p, san.san.unstructured_name.len );
                p += san.san.unstructured_name.len;
                n -= san.san.unstructured_name.len;
            }
            break;

            /*
             * Type not supported, skip item.
             */
            default:
                ret = mbedtls_snprintf( p, n, "\n%s    <unsupported>", prefix );
                MBEDTLS_X509_SAFE_SNPRINTF;
                break;
        }

        cur = cur->next;
    }

    *p = '\0';

    *size = n;
    *buf = p;

    return( 0 );
}
#endif

