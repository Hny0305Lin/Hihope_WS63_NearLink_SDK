/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: diag dfx cmd init
 */

#include "diag_dfx_cmd_init.h"
#include "dfx_adapt_layer.h"

#ifdef CONFIG_DIAG_UPDATE_CMD_SUPPORT
#include "diag_update.h"
#endif

#ifdef CONFIG_DIAG_NV_SUPPORT
#include "zdiag_nv.h"
#endif

#ifdef CONFIG_DIAG_CHARGER_SUPPORT
#include "zdiag_charger.h"
#endif

#ifdef CONFIG_DIAG_AUDIO_PROC_SUPPORT
#include "zdiag_audio_proc.h"
#endif

#ifdef CONFIG_DIAG_AUDIO_DUMP_SUPPORT
#include "zdiag_audio_dump.h"
#endif

#ifdef CONFIG_DIAG_AUDIO_PROBE_SUPPORT
#include "zdiag_audio_probe.h"
#endif

#ifdef CONFIG_DIAG_GPU_PROC_SUPPORT
#include "diag_gpu_proc.h"
#endif

#ifdef CONFIG_DIAG_AUDIO_SAMPLE_DATA_SUPPORT
#include "diag_audio_sample_data.h"
#endif

#ifdef CONFIG_DIAG_LOGFILE_SUPPORT
#include "diag_cmd_logflash_test.h"
#endif

static errcode_t diag_audio_dfx_init(void)
{
    errcode_t ret = ERRCODE_SUCC;
#ifdef CONFIG_DIAG_AUDIO_PROC_SUPPORT
    ret = zdiag_audio_proc_init();
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("zdiag_audio_proc_init failed with 0x%x.\r\n", ret);
        return ret;
    }
#endif

#ifdef CONFIG_DIAG_AUDIO_DUMP_SUPPORT
    ret = zdiag_audio_dump_init();
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("zdiag_audio_dump_init failed with 0x%x.\r\n", ret);
        return ret;
    }
#endif

#ifdef CONFIG_DIAG_AUDIO_PROBE_SUPPORT
    ret = zdiag_audio_probe_init();
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("zdiag_audio_probe_init failed with 0x%x.\r\n", ret);
        return ret;
    }
#endif

#ifdef CONFIG_DIAG_AUDIO_SAMPLE_DATA_SUPPORT
    ret = diag_audio_sample_data_init();
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("diag_audio_sample_data_init failed with 0x%x.\r\n", ret);
        return ret;
    }
#endif
    return ret;
}

errcode_t diag_dfx_cmd_init(void)
{
    errcode_t ret = ERRCODE_SUCC;

#ifdef CONFIG_DIAG_UPDATE_CMD_SUPPORT
    ret = zdiag_update_init();
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("zdiag_update_init failed with 0x%x.\r\n", ret);
        return ret;
    }
#endif

#ifdef CONFIG_DIAG_NV_SUPPORT
    ret = zdiag_nv_init();
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("zdiag_nv_init failed with 0x%x.\r\n", ret);
        return ret;
    }
#endif

#ifdef CONFIG_DIAG_CHARGER_SUPPORT
    ret = zdiag_charger_init();
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("zdiag_charger_init failed with 0x%x.\r\n", ret);
        return ret;
    }
#endif

#ifdef CONFIG_DIAG_LOGFILE_SUPPORT
    ret = diag_logfile_init();
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("diag_logfile_init failed with 0x%x.\r\n", ret);
        return ret;
    }
#endif

#ifdef CONFIG_DIAG_GPU_PROC_SUPPORT
    ret = diag_gpu_proc_init();
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("diag_gpu_proc_init failed with 0x%x.\r\n", ret);
        return ret;
    }
#endif

    ret = diag_audio_dfx_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ret;
}

