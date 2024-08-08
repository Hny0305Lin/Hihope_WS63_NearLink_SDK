/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: Low-power Framework.
 * Author: Huawei LiteOS Team
 * Create: 2020-09-19
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

/**
 * @defgroup los_lowpower Lowpower
 * @ingroup kernel
 */

#ifndef _LOS_LOWPOWER_H
#define _LOS_LOWPOWER_H

#include "los_base.h"
#include "los_sys.h"
#include "los_err.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup  los_lowpower
 *
 * Sleep modes
 */
typedef enum LOS_SLEEP_MODE {
    LOS_SLEEP_NORMAL = 0,
    LOS_SLEEP_LIGHT, /**< Light sleep mode */
    LOS_SLEEP_DEEP,  /**< Deep sleep mode */
    LOS_SLEEP_MAX,   /**< Reserved value, which is invalid now */
} LosSleepMode;

#ifdef LOSCFG_KERNEL_DYN_FREQ
/**
 * @ingroup  los_lowpower
 *
 * System main frequency modes
 */
typedef enum LOS_FREQ_MODE {
    LOS_SYS_FREQ_SUPER = 0, /**< Super high freq */
    LOS_SYS_FREQ_HIGH,      /**< High freq */
    LOS_SYS_FREQ_NORMAL,    /**< Normal freq */
    LOS_SYS_FREQ_LOW,       /**< Low freq */
    LOS_SYS_FREQ_MAX,
} LosFreqMode;

/**
 * @ingroup  los_lowpower
 * @brief System main frequency tuning.
 *
 * @par Description:
 * This API is used to tune the system main frequency.
 *
 * @attention The frequency increase is performed immediately.
 * The frequency decrease is performed only after system enters the idle state.
 *
 * @param  freq [IN] The system frequency, corresponding to LosFreqMode.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_lowpower.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V200R005C10
 */
extern VOID LOS_PowerMgrChangeFreq(LosFreqMode freq);
#endif

/**
 * @ingroup  los_lowpower
 *
 * Power manager run options
 */
typedef struct {
#ifdef LOSCFG_KERNEL_DYN_FREQ
    UINT32 (*changeFreq)(LosFreqMode freq); /**< Tune system frequency */
#endif
#if defined (LOSCFG_KERNEL_LIGHTSLEEP) || defined (LOSCFG_KERNEL_DEEPSLEEP)
    LosSleepMode (*getSleepMode)(VOID);     /**< GetSleepMode, provided for special needs before entering sleep */
#endif
#ifdef LOSCFG_KERNEL_LIGHTSLEEP
    VOID (*enterLightSleep)(VOID);    /**< Enter light sleep */
#endif
#ifdef LOSCFG_KERNEL_DEEPSLEEP
#ifdef LOSCFG_LOWPOWER_SLEEP_USERCONFIG
    VOID (*userPreConfig)(VOID);      /**< UserPreconfig, provided for special needs before entering sleep */
    VOID (*userPostConfig)(VOID);     /**< UserPostconfig, provided for special needs after wakeup */
#endif
    VOID (*systemWakeup)(VOID);       /**< System wakeup */
    VOID (*suspendConfig)(VOID);      /**< Supend device before entering deep sleep */
    VOID (*resumeConfig)(VOID);       /**< Resume device after wakeup from deep sleep */
#ifdef LOSCFG_KERNEL_SMP
    VOID (*otherCoreResume)(VOID);    /**< Other core Resume for multi-core scenes */
#endif
    VOID (*enterDeepSleep)(VOID);     /**< Enter deep sleep */
    VOID (*setWakeUpTimer)(UINT32 timeout);   /**< Set wakeup timer */
    UINT32 (*withdrawWakeUpTimer)(VOID);      /**< Withdraw wakeup timer */
#ifdef LOSCFG_KERNEL_RAM_SAVE
    VOID (*contextSave)(VOID);        /**< Context save */
    VOID (*contextRestore)(VOID);     /**< Context restore */
#endif
#endif
#ifdef LOSCFG_LOWPOWER_SLEEP_WFI
    VOID (*enterWFI)(VOID);     /**< Enter WFI mode */
#endif
} PowerMgrRunOps;

/**
 * @ingroup  los_lowpower
 *
 * Power manager parameter
 */
typedef struct {
    PowerMgrRunOps runOps;           /**< power manager run operations */
} PowerMgrParameter;

#ifdef LOSCFG_KERNEL_DYN_FREQ
    #define SET_RUNOPS_CHANGEFREQ(hook) .changeFreq = hook,
#else
    #define SET_RUNOPS_CHANGEFREQ(hook)
#endif
#if defined (LOSCFG_KERNEL_LIGHTSLEEP) || defined (LOSCFG_KERNEL_DEEPSLEEP)
    #define SET_RUNOPS_GETSLEEPMODE(hook) .getSleepMode = hook,
#else
    #define SET_RUNOPS_GETSLEEPMODE(hook)
#endif
#ifdef LOSCFG_KERNEL_LIGHTSLEEP
    #define SET_RUNOPS_ENTERLIGHTSLEEP(hook) .enterLightSleep = hook,
#else
    #define SET_RUNOPS_ENTERLIGHTSLEEP(hook)
#endif
#ifdef LOSCFG_KERNEL_DEEPSLEEP
#ifdef LOSCFG_LOWPOWER_SLEEP_USERCONFIG
    #define SET_RUNOPS_USERPRECONFIG(hook) .userPreConfig = hook,
    #define SET_RUNOPS_USERPOSTCONFIG(hook) .userPostConfig = hook,
#else
    #define SET_RUNOPS_USERPRECONFIG(hook)
    #define SET_RUNOPS_USERPOSTCONFIG(hook)
#endif
    #define SET_RUNOPS_SYSTEMWAKEUP(hook) .systemWakeup = hook,
    #define SET_RUNOPS_SUSPENDCONFIG(hook) .suspendConfig = hook,
    #define SET_RUNOPS_RESUMECONFIG(hook) .resumeConfig = hook,
#ifdef LOSCFG_KERNEL_SMP
    #define SET_RUNOPS_OTHERCORERESUME(hook) .otherCoreResume = hook,
#else
    #define SET_RUNOPS_OTHERCORERESUME(hook)
#endif
    #define SET_RUNOPS_ENTERDEEPSLEEP(hook) .enterDeepSleep = hook,
    #define SET_RUNOPS_SETWAKEUPTIMER(hook) .setWakeUpTimer = hook,
    #define SET_RUNOPS_WITHDRAWWAKEUPTIMER(hook) .withdrawWakeUpTimer = hook,
#ifdef LOSCFG_KERNEL_RAM_SAVE
    #define SET_RUNOPS_CONTEXTSAVE(hook) .contextSave = hook,
    #define SET_RUNOPS_CONTEXTRESTORE(hook) .contextRestore = hook,
#else
    #define SET_RUNOPS_CONTEXTSAVE(hook)
    #define SET_RUNOPS_CONTEXTRESTORE(hook)
#endif
#else
    #define SET_RUNOPS_SYSTEMWAKEUP(hook)
    #define SET_RUNOPS_SUSPENDCONFIG(hook)
    #define SET_RUNOPS_RESUMECONFIG(hook)
    #define SET_RUNOPS_ENTERDEEPSLEEP(hook)
    #define SET_RUNOPS_SETWAKEUPTIMER(hook)
    #define SET_RUNOPS_WITHDRAWWAKEUPTIMER(hook)
#endif
#ifdef LOSCFG_LOWPOWER_SLEEP_WFI
    #define SET_RUNOPS_ENTERWFI(hook) .enterWFI = hook,
#else
    #define SET_RUNOPS_ENTERWFI(hook)
#endif

/**
 * @ingroup  los_lowpower
 * @brief Init the power manager framework.
 *
 * @par Description:
 * This API is used to init the power manager framework.
 *
 * @attention None.
 *
 * @param  para [IN] The power manager parameter.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_lowpower_impl.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V200R005C10
 */
extern UINT32 LOS_PowerMgrInit(const PowerMgrParameter *para);

/**
 * @ingroup los_lowpower
 * Lowpower error code: Null parameter.
 *
 * Value: 0x02000201.
 *
 * Solution: Check the parameter.
 */
#define LOS_ERRNO_LPM_INIT_NULL                  LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x01)

#ifdef LOSCFG_LOWPOWER_SLEEP_VETO
/**
 * @ingroup  los_lowpower
 * @brief Veto to enter lighter sleep mode.
 *
 * @par Description:
 * This API is used to mark sleep veto, called when the current state is idle.
 *
 * @attention if request for not sleep, pass none mode.
 *
 * @param  mode [IN] the sleep mode requested.
 * @param  requestId [IN] the sleep mode who request, used to debug, should no more than 32.
 *
 * @retval #LOS_OK Sleep mode locked successfully.
 * @retval #LOS_NOK Failed to lock sleep mode.
 * @par Dependency:
 * <ul><li>los_lowpower.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V200R005C20
 */
extern UINT32 LOS_PowerMgrSleepLock(LosSleepMode mode, UINT32 requestId);

/**
 * @ingroup  los_lowpower
 * @brief Erase the sleep veto.
 *
 * @par Description:
 * This API is used to erase sleep veto, called when the current state is busy.
 *
 * @attention erase veto flag by LOS_PowerMgrSleepRequest
 *
 * @param  mode [IN] the sleep mode requested.
 * @param  requestId [IN] the sleep mode who request, used to debug, should no more than 32.
 *
 * @retval #LOS_OK Sleep mode unlocked successfully.
 * @retval #LOS_NOK Failed to unlock sleep mode.
 * @par Dependency:
 * <ul><li>los_lowpower.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V200R005C20
 */
extern UINT32 LOS_PowerMgrSleepUnLock(LosSleepMode mode, UINT32 requestId);
#endif

#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
/**
 * @ingroup  los_lowpower
 * @brief Enable lowpower feature.
 *
 * @par Description:
 * This API is used to enable lowpower feature.
 *
 * @attention None.
 *
 * @param  None.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_lowpower.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V200R005C10
 */
extern VOID LOS_EnableLowPower(VOID);

/**
 * @ingroup  los_lowpower
 * @brief Disable lowpower feature.
 *
 * @par Description:
 * This API is used to disable lowpower feature.
 *
 * @attention None.
 *
 * @param  None.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_lowpower.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V200R005C10
 */
extern VOID LOS_DisableLowPower(VOID);

/**
 * @ingroup  los_lowpower
 * @brief Get lowpower status.
 *
 * @par Description:
 * This API is used to get lowpower status.
 *
 * @attention None.
 *
 * @param  None.
 *
 * @retval #UINT32 Lowpower status.
 * @par Dependency:
 * <ul><li>los_lowpower.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V200R005C10
 */
extern UINT32 LOS_GetLowPowerStatus(VOID);
#endif

/**
 * @ingroup  los_lowpower
 * @brief Get predicted sleep ticks.
 *
 * @par Description:
 * This API is used to get predicted sleep ticks.
 *
 * @attention None.
 *
 * @param  None.
 *
 * @retval #UINT32 Sleep ticks.
 * @par Dependency:
 * <ul><li>los_lowpower.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V200R005C10
 */
extern UINT32 LOS_SleepTicksGet(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_LOWPOWER_H */
