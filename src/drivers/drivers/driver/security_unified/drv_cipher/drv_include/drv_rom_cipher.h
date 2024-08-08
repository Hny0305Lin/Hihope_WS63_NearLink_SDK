/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: security api for romboot. \n
 *
 */

#ifndef DRV_ROM_CIPHER_H
#define DRV_ROM_CIPHER_H

int drv_rom_cipher_init(void);
int drv_rom_cipher_deinit(void);

/* support data_length not 64 bytes aligned. */
int drv_rom_cipher_sha256(const unsigned char *data, unsigned int data_length, unsigned char *hash,
    unsigned int hash_length);

int drv_rom_cipher_get_randnum(unsigned int *randnum);

/* ecc verify. */
typedef struct {
    unsigned char *x;
    unsigned char *y;
    unsigned int length;
} drv_rom_cipher_ecc_point;

typedef struct {
    unsigned int length;
    unsigned char *data;
} drv_rom_cipher_data;

typedef struct {
    unsigned char  *r;
    unsigned char  *s;
    unsigned int length;
} drv_rom_cipher_ecc_sig;

int drv_rom_cipher_pke_bp256r_verify(const drv_rom_cipher_ecc_point *pub_key, const drv_rom_cipher_data *hash,
    const drv_rom_cipher_ecc_sig *sig);
#endif