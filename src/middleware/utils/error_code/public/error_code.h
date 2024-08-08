/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  error_code_e module interface
 * Author:
 * Create: 2018-10-15
 */
#ifndef LIB_ERROR_CODE_H
#define LIB_ERROR_CODE_H

#include "std_def.h"
#include "stdint.h"

/**
 * @defgroup connectivity_libs_error_code Reporting
 * @ingroup  connectivity_libs
 * @{
 */
/**
 * @brief  Definitions of the error code
 */
typedef enum error_code_e {
    ERROR_CODE_NONE_ERROR = 0,
    ERROR_CODE_FLASH_INIT_ERROR = 1,
    ERROR_CODE_FLASH_LENGTH_ERROR = 2,
    ERROR_CODE_FLASH_BACKUP_LENGTH_ERROR = 3,
    ERROR_CODE_FLASH_VALIDATE_ERROR = 4,
    ERROR_CODE_FLASH_BACKUP_VALIDATE_ERROR = 5,
    ERROR_CODE_FLASH_READ_ERROR = 6,
    ERROR_CODE_CODELOADER_INIT_ERROR = 7,
    ERROR_CODE_CODELOADER_READ_VERSION_ERROR = 8,
    ERROR_CODE_CODELOADER_WRITE_ERROR = 9,
    ERROR_CODE_CODELOADER_RECEIVE_SSB_SHA_DATA_ERROR = 10,
    ERROR_CODE_CODELOADER_CHECK_SSB_SHA_DATA_ERROR = 11,
    ERROR_CODE_CODELOADER_WRITE_SSB_SHA_DATA_ERROR = 12,
    ERROR_CODE_CODELOADER_EXIT_ERROR = 13,
    ERROR_CODE_SSB_SHA_CHECK_ERROR = 14,
    ERROR_CODE_ROOT_PUBKEY_SHA_CHECK_ERROR = 15,
    ERROR_CODE_SUBKEY_RSA_CHECK_ERROR = 16,
    ERROR_CODE_SUBKEY_PARAM_CHECK_ERROR = 17,
    ERROR_CODE_SSB_SIGN_CHECK_ERROR = 18,
    ERROR_CODE_CODELOADER_SET_NON_EFLASH_MODE_ERROR = 19,
    ERROR_CODE_SSB_VERSION_ERROR = 20,
    ERROR_CODE_CFCT_CHIP_ID_ERROR = 21,
    ERROR_CODE_CFCT_CFG_PARA_ERROR = 22,
    ERROR_CODE_CFCT_SIGN_CHECK_ERROR = 23,
    ERROR_CODE_CFCT_FLASH_ERROR = 24,
    ERROR_CODE_CFCT_FLASH_READ_ERROR = 25,
} error_code_e;

/**
 * @brief  Reset the error module
 * @return NULL.
 */
void error_code_reset(void);

/**
 * @brief  Set the error bit
 * @param  id the relevant bit in error code to be set
 */
void error_code_set(error_code_e id);

/**
 * @brief  Clear the error bit
 * @param  id the relevant bit in error code to be clear
 */
void error_code_clear(error_code_e id);

/**
 * @brief  Get the error result
 * @return the result that represents the errors
 */
uint32_t error_code_get(void);

/**
 * @}
 */
#endif
