/* ---------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Driver Framework Header File
 * Author: Huawei LiteOS Team
 * Create: 2020-08-15
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
 * @defgroup drivers Drivers
 * @defgroup los_driverbase Driverbase
 * @ingroup drivers
 *
 */

#ifndef _LOS_DRIVER_BASE_H
#define _LOS_DRIVER_BASE_H

#include "los_mux.h"

struct LosDriver;
extern LOS_DL_LIST g_deviceList;

/**
 * @ingroup los_driverbase
 * LosDevice register config structure.
 *
 */
struct LosDeviceRegs {
    UINTPTR base; /**< Register base address of device */
    UINTPTR size; /**< Register size of device */
};

/**
 * @ingroup los_driverbase
 * LosDevice config structure.
 *
 */
struct LosDeviceConfig {
    UINTPTR irqNum;             /**< Interrupt number of device */
    UINT32 numRegs;             /**< Number of device registers */
    struct LosDeviceRegs *regs; /**< Device registers */
};

/**
 * @ingroup los_driverbase
 * LosDevice information structure.
 *
 */
struct LosDevice {
    const CHAR *name;           /**< Device name */
    INT32 id;                   /**< Device id, generally like 0,1... */
    LOS_DL_LIST driverNode;     /**< Node for linking to the attached driver */
    LOS_DL_LIST deviceItem;     /**< Node for linking to the global device list */
    LOS_DL_LIST pmNode;         /**< Node for linking to the global power manager list */
    UINT32 mutex;               /**< Mutex */
    BOOL isRegistered;          /**< If device registered to the system */
    struct LosDriver *driver;   /**< Attached driver */
    struct LosDeviceConfig cfg; /**< Standard device configurations */
    VOID *data;                 /**< Device private data */
};

/**
 * @ingroup los_driverbase
 * LosDevice power manager operation structure.
 *
 */
struct LosPowerMgrOps {
    INT32 (*suspend)(struct LosDevice *device);  /**< Suspend function of power manager ops */
    INT32 (*resume)(struct LosDevice *device);   /**< Resume function of power manager ops */
    INT32 (*prepare)(struct LosDevice *device);  /**< Prepare function of power manager ops */
    INT32 (*complete)(struct LosDevice *device); /**< Complete function of power manager ops */
};

/**
 * @ingroup los_driverbase
 * LosDriver operation structure.
 *
 */
struct LosDriverOps {
    INT32 (*probe)(struct LosDevice *device);   /**< Probe function of driver ops */
    INT32 (*remove)(struct LosDevice *device);  /**< Remove function of driver ops */
    VOID (*shutdown)(struct LosDevice *device); /**< Shutdown function of driver ops */
};

/**
 * @ingroup los_driverbase
 * LosDriver information structure.
 *
 */
struct LosDriver {
    const CHAR *name;            /**< Driver name */
    LOS_DL_LIST deviceList;      /**< List header for putting attached devices */
    LOS_DL_LIST driverItem;      /**< Node for linking to the global driver list */
    UINT32 mutex;                /**< Mutex */
    BOOL isRegistered;           /**< If driver registered to the system */
    struct LosDriverOps ops;     /**< Driver operations */
    struct LosPowerMgrOps pmOps; /**< Driver power manager operations */
};

/* errno */
/**
 * @ingroup los_driverbase
 * Task error code: Invalid input.
 *
 * Value: 0x02004110
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_INPUT_INVALID                        LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x10)

/**
 * @ingroup los_driverbase
 * Task error code: Mutex failed.
 *
 * Value: 0x02004111
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_MUX_FAIL                             LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x11)

/**
 * @ingroup los_driverbase
 * Task error code: Do match function failed.
 *
 * Value: 0x02004120
 *
 * Solution: This error code is not in use temporarily.
 */
#define LOS_ERRNO_DRIVER_DRIVER_MATCH_FAIL                    LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x20)

/**
 * @ingroup los_driverbase
 * Task error code: Do probe function failed.
 *
 * Value: 0x02004121
 *
 * Solution:  This error code is not in use temporarily.
 */
#define LOS_ERRNO_DRIVER_DRIVER_PROBE_FAIL                    LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x21)

/**
 * @ingroup los_driverbase
 * Task error code: driver register twice.
 *
 * Value: 0x02004122
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_DRIVER_REGISTERED                    LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x22)

/**
 * @ingroup los_driverbase
 * Task error code: Invalid input.
 *
 * Value: 0x02004123
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_DRIVER_NOTFOUND                      LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x23)

/**
 * @ingroup los_driverbase
 * Task error code: Device already attach to driver.
 *
 * Value: 0x02004130
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_DEVICE_BOUNDED                       LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x30)

/**
 * @ingroup los_driverbase
 * Task error code: Invalid input.
 *
 * Value: 0x02004131
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_DEVICE_INITIALFAIL                   LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x31)

/**
 * @ingroup los_driverbase
 * Task error code: Device register twice.
 *
 * Value: 0x02004132
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_DEVICE_REGISTERED                    LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x32)

/**
 * @ingroup los_driverbase
 * Task error code: Device busy.
 *
 * Value: 0x02004133
 *
 * Solution: Check the Input.
 */
#define LOS_ERRNO_DRIVER_DEVICE_BUSY                          LOS_ERRNO_OS_ERROR(LOS_MOD_DRIVER, 0x33)

/* driver api */
/**
 * @ingroup los_driverbase
 * @brief register a new driver to the system.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to register a driver to the system.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The param probe function is applied after attach the device to the driver.</li>
 * <li>The param remove function is used when device or driver is deleted.</li>
 * </ul>
 *
 * @param  drv      [IN]A pointer to LosDriver.
 *
 * @retval #LOS_ERRNO_DRIVER_INPUT_INVALID        Invalid input.drv/drv->name can not be NULL.
 * @retval #LOS_ERRNO_DRIVER_DRIVER_REGISTERED    Invalid input.drv node register twice.
 * @retval #LOS_ERRNO_DRIVER_MUX_FAIL             Mux create failed.
 * @retval #LOS_OK                                The driver register success.
 * @par Dependency:
 * <ul><li>los_driverbase.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R05C10
 */
UINT32 LOS_DriverRegister(struct LosDriver *drv);

/**
 * @ingroup los_driverbase
 * @brief unregister a driver from the system.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to unregister a driver from the system.</li>
 * </ul>
 *
 * @param  drv      [IN]A pointer to device_driver.drv/drv->name can not be NULL.
 *
 * @retval #LOS_ERRNO_DRIVER_INPUT_INVALID        Invalid input.drv and drv.driver->name can not be NULL.
 * @retval #LOS_ERRNO_DRIVER_DRIVER_NOTFOUND      Driver not found.
 * @retval #LOS_ERRNO_DRIVER_DEVICE_BUSY          Device busy.
 * @retval #LOS_OK                                The driver unregister success.
 *
 * @par Dependency:
 * <ul><li>los_driverbase.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R05C10
 */
UINT32 LOS_DriverUnregister(struct LosDriver *drv);

/* device api */
/**
 * @ingroup los_driverbase
 * @brief register a new device to the system.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to register a device to the system.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The same dev node can not be registered twice.</li>
 * </ul>
 *
 * @param  dev      [IN]A pointer to device.
 *
 * @retval #LOS_ERRNO_DRIVER_INPUT_INVALID        Invalid input.dev/dev->name can not be NULL.
 * @retval #LOS_ERRNO_DRIVER_DEVICE_REGISTERED    Device register twice.
 * @retval #LOS_OK                                The device register success.
 * @par Dependency:
 * <ul><li>los_driverbase.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R05C10
 */
UINT32 LOS_DeviceRegister(struct LosDevice *dev);

/**
 * @ingroup los_driverbase
 * @brief unregister a device from the system.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to unregister a device from the system.</li>
 * </ul>
 * @param  dev      [IN]A pointer to device.dev/dev->name can not be NULL.
 *
 * @par Dependency:
 * <ul><li>los_driverbase.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R05C10
 */
void LOS_DeviceUnregister(struct LosDevice *dev);

/**
 * @ingroup los_driverbase
 * @brief get data form a device.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to get data form a device.</li>
 * </ul>
 * @param dev      [IN]A pointer to device.dev/dev->name can not be NULL.
 *
 * @retval #VOID*  the data of the device.
 * @par Dependency:
 * <ul><li>los_driverbase.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R05C10
 */
VOID *LOS_DeviceDataGet(const struct LosDevice *dev);

/**
 * @ingroup los_driverbase
 * @brief get register base address form a device.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to get register base address form a device. The device may have multiple sets of registers,
 * the index must be specified. If The device has only one set of register, the index should be 0.</li>
 * </ul>
 * @param dev        [IN]A pointer to device.dev/dev->name can not be NULL.
 * @param index      [IN]The number of which reg base to get.
 *
 * @retval #UINTPTR  the register base address of the device.
 * @par Dependency:
 * <ul><li>los_driverbase.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R05C10
 */
UINTPTR LOS_DeviceRegBaseGet(const struct LosDevice *dev, UINT32 index);

/**
 * @ingroup los_driverbase
 * @brief get register size form a device.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to get register size form a device. The device may have multiple sets of registers,
 * the index must be specified. If The device has only one set of register, the index should be 0.</li>
 * </ul>
 * @param dev       [IN]A pointer to device.dev/dev->name can not be NULL.
 * @param index     [IN]The number of which reg size to get.
 *
 * @retval #UINTPTR  the register size of the device.
 * @par Dependency:
 * <ul><li>los_driverbase.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R05C10
 */
UINTPTR LOS_DeviceRegSizeGet(const struct LosDevice *dev, UINT32 index);

/**
 * @ingroup los_driverbase
 * @brief get interrupt number form a device.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to get register base address form a device.</li>
 * </ul>
 * @param dev        [IN]A pointer to device.dev/dev->name can not be NULL.
 *
 * @retval #UINTPTR  the interrupt number of the device.
 * @par Dependency:
 * <ul><li>los_driverbase.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R05C10
 */
UINTPTR LOS_DeviceIrqNumGet(const struct LosDevice *dev);

/**
 * @ingroup los_driverbase
 * @brief suspend all devices in preparedList.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to suspend all prepared list device.</li>
 * </ul>
 * @param #VOID.
 *
 * @retval #VOID.
 * @par Dependency:
 * <ul><li>los_driverbase.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R05C10
 */
INT32 LOS_PmSuspend(VOID);

/**
 * @ingroup los_driverbase
 * @brief resume all devices in suspendedList.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to resume all prepared list device.</li>
 * </ul>
 * @param #VOID.
 *
 * @retval #VOID.
 * @par Dependency:
 * <ul><li>los_driverbase.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R05C10
 */
VOID LOS_PmResume(VOID);

/**
 * @ingroup los_driverbase
 * @brief iterate over a device list and execute hook function.
 *
 * @par Description:
 * This API is used to iterate over a device list and execute hook function..
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param device         [IN] Pointer to the structure that contains the doubly linked list that is to be traversed.
 * @param list           [IN] Pointer to the doubly linked list to be traversed.
 * @param hook           [IN] Hook function.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_driverbase.h: the header file that contains the API declaration.</li></ul>
 * @see
 * @since Huawei LiteOS V200R05C10
 */
#define LOS_FOREACH_DEVICE_LIST_HOOK(device, list, hook) \
    LOS_DL_LIST_FOR_EACH_ENTRY_HOOK(device, list, struct LosDevice, driverNode, hook)

/**
 * @ingroup los_driverbase
 * @brief iterate over g_deviceList and execute hook function.
 *
 * @par Description:
 * This API is used to iterate over g_deviceList and execute hook function.
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param device         [IN] Pointer to the structure that contains the doubly linked list that is to be traversed.
 * @param hook           [IN] Hook function.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_driverbase.h: the header file that contains the API declaration.</li></ul>
 * @see
 * @since Huawei LiteOS V200R05C10
 */
#define LOS_FOREACH_DEVICE_HOOK(device, hook) \
    LOS_DL_LIST_FOR_EACH_ENTRY_HOOK(device, &g_deviceList, struct LosDevice, deviceItem, hook)

/**
 * @ingroup los_driverbase
 * @brief iterate over a driver's deviceList.
 *
 * @par Description:
 * This API is used to iterate over a driver's deviceList.
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param device         [IN] Pointer to the structure that contains the doubly linked list that is to be traversed.
 * @param driver         [IN] Pointer to the structure that contains deviceList to be traversed.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_driverbase.h: the header file that contains the API declaration.</li></ul>
 * @see
 * @since Huawei LiteOS V200R05C10
 */
#define LOS_FOREACH_DEVICE_BY_DRIVER(device, driver) \
    LOS_FOREACH_DEVICE_LIST_HOOK(device, &(driver)->deviceList, NULL)

/**
 * @ingroup los_driverbase
 * @brief iterate over a device list.
 *
 * @par Description:
 * This API is used to iterate over a device list.
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param device         [IN] Pointer to the structure that contains the doubly linked list that is to be traversed.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_driverbase.h: the header file that contains the API declaration.</li></ul>
 * @see
 * @since Huawei LiteOS V200R05C10
 */
#define LOS_FOREACH_DEVICE(device) \
    LOS_FOREACH_DEVICE_HOOK(device, NULL)

#endif
