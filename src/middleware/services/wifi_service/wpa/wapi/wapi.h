/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: WAPI Implementation
 */
#ifndef WAPI_H
#define WAPI_H

#include "wpa_supplicant_i.h"
#include "utils/eloop.h"

#define ETH_TYPE_WAI 0x88B4

#define WAI_VERSION 1
#define WAI_TYPE    1

#define WAI_FLAG_BK_UPDATE  BIT(0)
#define WAI_FLAG_PRE_AUTH   BIT(1)
#define WAI_FLAG_CERT_REQ   BIT(2)
#define WAI_FLAG_OPT_FIELD  BIT(3)
#define WAI_FLAG_USK_UPDATE BIT(4)
#define WAI_FLAG_STAKEY_NEG BIT(5)
#define WAI_FLAG_STAKEY_DEL BIT(6)
#define WAI_FLAG_RESERVED   BIT(7)

#define MAX_KEYDATA_SIZE       256
#define WAI_AUTH_ID_SIZE       32
#define WAI_FLAG_SIZE          1
#define WAI_CHALLENGE_SIZE     32
#define WAI_KEY_LENGTH_SIZE    1
#define WAI_BKID_SIZE          16
#define WAI_BK_SIZE            16
#define WAI_ADDID_SIZE         12
#define WAI_USKID_SIZE         1
#define WAI_MSKID_SIZE         1
#define WAI_USK_PN_IV_SIZE     16
#define WAI_MSK_ANNO_IV_SIZE   16
#define WAI_DATA_SERIAL_NUMBER 16
#define WAI_MIC_SIZE           20
#define WAI_UEK_UCK_SIZE       32
#define WAI_MAK_SIZE           16
#define WAI_KEK_SIZE           16
#define WAI_MSK_SIZE           32
#define WAI_NMK_SIZE           16
#define WAI_MAX_TX_COUNT       3
#define WAI_USKSA_CNT          2
#define WAI_PRE_PSK_SIZE       128 /* HEX key type will be 64 * 2 */

#define WAPI_IE_ID              0x44
#define WAPI_IE_ID_SIZE         1
#define WAPI_IE_LENGTH_SIZE     1
#define WAPI_IE_VERSION_SIZE    2
#define WAPI_IE_OUI_SIZE        3
#define WAPI_IE_AKM_CNT_LEN     2
#define WAPI_IE_AKM_SUIT_LEN    1
#define WAPI_IE_AKM_SUIT_PSK    0x00147202
#define WAPI_IE_CIPHER_CNT_LEN  2
#define WAPI_IE_CIPHER_SUIT_LEN 1
#define WAPI_IE_MIN_SIZE        16
#define WAPI_IE_MAX_SIZE        255
#define WAI_USK_DERIVATION_SIZE 96

#define WAI_AUTH_TIMEOUT        10 /* unit: s */

#define WAPI_SUCCESS    0
#define WAPI_FAILED     (-1)

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

typedef enum {
    KEYSLOT_ENGINE_AES = 0,
    KEYSLOT_ENGINE_SM4,
    KEYSLOT_ENGINE_HMAC_SHA1,
    KEYSLOT_ENGINE_HMAC_SHA256,
    KEYSLOT_ENGINE_HMAC_SHA384,
    KEYSLOT_ENGINE_HMAC_SHA512,
    KEYSLOT_ENGINE_HMAC_SM3,

    /* content key. */
    KEYSLOT_ENGINE_AES_ABRK1_REE,
} keyslot_engine;

/** Type of operation. */
typedef enum {
    WAI_OPERATION_NONE = -1,
    WAI_DECRYPT = 0,
    WAI_ENCRYPT,
} wai_operation_t;

typedef enum {
    WAISM_INIT = 0,
    WAISM_ALREADY_ASSOC,
    WAISM_USKNEG_RES,
    WAISM_USKNEG_CONFIRM,
    WAISM_FINSHED
} wai_state_enum;

typedef enum _wai_frame_enum {
    WAI_PREAUTH_START = 1,
    WAI_STAKEY_REQUEST = 2,
    WAI_AUTH_ACTIVE = 3,
    WAI_ACCESS_AUTH_REQUEST = 4,
    WAI_ACCESS_AUTH_RESPONSE = 5,
    WAI_CERT_AUTH_REQUEST = 6,
    WAI_CERT_AUTH_RESPONSE = 7,
    WAI_USK_NEGOTIATION_REQUEST = 8,
    WAI_USK_NEGOTIATION_RESPONSE = 9,
    WAI_USK_NEGOTIATION_CONFIRM = 10,
    WAI_MSK_ANNOUNCEMENT = 11,
    WAI_MSK_ANNOUNCEMENT_RESPONSE = 12,
    WAI_SUBTYPE_MAX = 13
} wai_frame_enum;

struct wpa_supplicant;
struct l2_packet_data;
struct wpabuf;
struct wpa_ie_data;
struct wpa_bss;
struct wpa_driver_associate_params;

typedef int (*wai_dispose_func)(struct wpa_supplicant *wpa,
    const unsigned char *payload, unsigned int payload_len);

typedef struct wai_sm_hdl {
    unsigned char msgid;
    wai_dispose_func handler;
} wai_sm_hdl_stru;

/* BKSA struct */
typedef struct wapi_bksa {
    unsigned char bkid[WAI_BKID_SIZE];
    unsigned char bk[WAI_BK_SIZE];
    unsigned char ae_mac[ETH_ALEN];
    unsigned char asue_mac[ETH_ALEN];
} wapi_bksa_stru;

typedef struct wapi_usk {
    unsigned char uek_uck[WAI_UEK_UCK_SIZE];
    unsigned char mak[WAI_MAK_SIZE];
    unsigned char kek[WAI_KEK_SIZE];
} wapi_usk_stru;

/* USKSA struct */
typedef struct wapi_usksa {
    unsigned char uskid;
    wapi_usk_stru usk;
} wapi_usksa_stru;

/* MSKSA struct */
typedef struct wapi_msksa {
    unsigned char mskid;
    unsigned char msk_seq_pn[WAI_DATA_SERIAL_NUMBER];
    unsigned char msk_anno_iv[WAI_MSK_ANNO_IV_SIZE];
} wapi_msksa_stru;

typedef struct wapi_cipher_suite {
    int wpa_alg_type;
    int wpa_cipher_flag;
    int wai_cipher_type;
    int (*wai_decrypt_cb)(const unsigned char *iv, unsigned int ivlen,
                          const unsigned char *key, unsigned int keylen,
                          const unsigned char *input, unsigned int inlen,
                          unsigned char *output, unsigned int *outlen);
} wapi_cipher_suite_stru;

typedef enum _auth_type_enum {
    AUTH_TYPE_NONE_WAPI = 0, /* no WAPI */
    AUTH_TYPE_WAPI_CERT,     /* Certificate */
    AUTH_TYPE_WAPI_PSK       /* Pre-PSK */
} auth_type_enum;

typedef enum _cipher_type_enum {
    CIPHER_TYPE_NONE = 0, /* resv */
    CIPHER_TYPE_SM4_OFB,  /* SM4 OFB mode */
    CIPHER_TYPE_MAX,
} cipher_type_enum;

typedef enum {
    CONN_ASSOC = 0,
    CONN_DISASSOC
} conn_status_enum;

struct wapi_asue_struct {
    struct wpa_supplicant *wpa;
    struct l2_packet_data *wapi_l2;
    struct wpabuf *tx_framebuf;
    unsigned int tx_count;

    const wapi_cipher_suite_stru *ucast_cipher_suite;
    const wapi_cipher_suite_stru *mcast_cipher_suite;

    wai_state_enum state;
    auth_type_enum auth_type;

    unsigned short next_frame_seq;
    unsigned short tx_frame_seq;
    unsigned char  wai_flag;

    unsigned char own_mac[ETH_ALEN];
    unsigned char bssid[ETH_ALEN];
    unsigned char addid[WAI_ADDID_SIZE]; /* ADDID( MAC || MAC ) */
    unsigned char asue_nonce[WAI_CHALLENGE_SIZE];
    unsigned char ae_next_nonce[WAI_CHALLENGE_SIZE];
    unsigned char bk[WAI_BK_SIZE];

    wapi_bksa_stru  bksa;
    wapi_usksa_stru usksa;
    wapi_msksa_stru msksa;

    unsigned char wapi_ie[WAPI_IE_MAX_SIZE];
    size_t wapi_ie_len;
    unsigned char assoc_wapi_ie[WAPI_IE_MAX_SIZE]; /* Own WAPI/RSN IE from (Re)AssocReq */
    size_t assoc_wapi_ie_len;
};

void wapi_iface_init(struct wapi_asue_struct *wapi);
void wapi_iface_deinit(struct wapi_asue_struct *wapi);
int wapi_init_ie(struct wpa_supplicant *wpa);
int wapi_parse_ie(const unsigned char *wapi_ie, unsigned int ie_len,
    struct wpa_ie_data *ie_data);
int wapi_generate_addid(struct wapi_asue_struct *wapi, struct wpa_bss *bss);
int wapi_event_process(struct wapi_asue_struct *wapi, conn_status_enum action,
    const unsigned char *assoc_ie, size_t assoc_ie_len);

#endif /* end of WAPI_H */

