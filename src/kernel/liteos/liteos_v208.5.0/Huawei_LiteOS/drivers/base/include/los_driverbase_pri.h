/* ---------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Driver Framework Inner Header File
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

#ifndef _LOS_DRIVER_BASE_PRI_H
#define _LOS_DRIVER_BASE_PRI_H

#include "los_driverbase.h"

extern LOS_DL_LIST g_deviceList;
extern LOS_DL_LIST g_driverList;
extern UINT32 g_driverBaseMutex;
extern UINT32 g_pmListMutex;

#define DEVICE_LOCK(dev)            (VOID)LOS_MuxPend((dev)->mutex, LOS_WAIT_FOREVER)
#define DEVICE_UNLOCK(dev)          (VOID)LOS_MuxPost((dev)->mutex)

#define DRIVER_LOCK(drv)            (VOID)LOS_MuxPend((drv)->mutex, LOS_WAIT_FOREVER)
#define DRIVER_UNLOCK(drv)          (VOID)LOS_MuxPost((drv)->mutex)

#define PM_LOCK(lock)               (VOID)LOS_MuxPend((lock), LOS_WAIT_FOREVER)
#define PM_UNLOCK(lock)             (VOID)LOS_MuxPost((lock))

#define DRIVER_BASE_LOCK(lock)      (VOID)LOS_MuxPend((lock), LOS_WAIT_FOREVER)
#define DRIVER_BASE_UNLOCK(lock)    (VOID)LOS_MuxPost((lock))

UINT32 OsDriverBaseInit(VOID);
UINT32 OsDriverAttachDevice(struct LosDriver *drv, struct LosDevice *dev);
UINT32 OsDriverDetachDevice(struct LosDriver *drv, struct LosDevice *dev);

void OsDevicePmAdd(struct LosDevice *dev);
void OsDevicePmRemove(struct LosDevice *dev);

STATIC INLINE BOOL OsDeviceMatchDriver(struct LosDevice *dev, struct LosDriver *drv)
{
    return (strcmp(dev->name, drv->name) == 0);
}

STATIC INLINE VOID OsDeviceBindDriver(struct LosDevice *dev, struct LosDriver *drv)
{
    DRIVER_LOCK(drv);
    LOS_ListTailInsert(&drv->deviceList, &dev->driverNode);
    DRIVER_UNLOCK(drv);
}

STATIC INLINE VOID OsDeviceUnbindDriver(struct LosDevice *dev, struct LosDriver *drv)
{
    DRIVER_LOCK(drv);
    LOS_ListDelete(&dev->driverNode);
    DRIVER_UNLOCK(drv);
}

STATIC INLINE VOID OsSysAddDevice(struct LosDevice *dev)
{
    DRIVER_BASE_LOCK(g_driverBaseMutex);
    LOS_ListTailInsert(&g_deviceList, &dev->deviceItem);
    DRIVER_BASE_UNLOCK(g_driverBaseMutex);

    dev->isRegistered = TRUE;
}

STATIC INLINE VOID OsSysRemoveDevice(struct LosDevice *dev)
{
    DRIVER_BASE_LOCK(g_driverBaseMutex);
    LOS_ListDelete(&dev->deviceItem);
    DRIVER_BASE_UNLOCK(g_driverBaseMutex);

    dev->isRegistered = FALSE;
}

STATIC INLINE VOID OsSysAddDriver(struct LosDriver *drv)
{
    DRIVER_BASE_LOCK(g_driverBaseMutex);
    LOS_ListTailInsert(&g_driverList, &drv->driverItem);
    DRIVER_BASE_UNLOCK(g_driverBaseMutex);

    drv->isRegistered = TRUE;
}

STATIC INLINE VOID OsSysRemoveDriver(struct LosDriver *drv)
{
    DRIVER_BASE_LOCK(g_driverBaseMutex);
    LOS_ListDelete(&drv->driverItem);
    DRIVER_BASE_UNLOCK(g_driverBaseMutex);

    drv->isRegistered = FALSE;
}

#endif
