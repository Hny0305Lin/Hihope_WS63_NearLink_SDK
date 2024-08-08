/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: radar_sensor_fft.h
 */

#ifndef __ALG_RADAR_FFT_H__
#define __ALG_RADAR_FFT_H__

#include "wlan_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PI 3.1415926535

typedef struct {
    float real;
    float imag;
} alg_radar_complex;

osal_void alg_radar_fft_complex(alg_radar_complex *x, osal_u32 fft_size, osal_s16 ifft_flag);
osal_void alg_radar_fftshift(alg_radar_complex *x, osal_u32 fft_size);
osal_void alg_radar_complex_abs(alg_radar_complex *x, float *res, osal_u32 sample_len);
float alg_radar_mean(float *x, osal_u32 sample_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif