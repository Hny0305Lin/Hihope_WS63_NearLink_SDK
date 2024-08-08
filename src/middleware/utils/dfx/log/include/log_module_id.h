/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  log module id define
 * Author:
 * Create:
 */

#ifndef LOG_MODULE_ID_H
#define LOG_MODULE_ID_H

enum OML_LOG_MODULEID_ENUM {
    LOG_WIFIMODULE      = 0,
    LOG_BTMODULE        = 1,
    LOG_GNSSMODULE      = 2,
    LOG_DSPMODULE       = 3,
    LOG_PFMODULE        = 4,
    LOG_MEDIAMODULE     = 5,
    LOG_NFCMODULE       = 6,
    LOG_APPMODULE       = 7,
    LOG_GPUMODULE       = 8,
    LOG_GUIMODULE       = 9,
    LOG_SLPMODULE       = 10,
    LOG_BTHMODULE       = 11,
    LOG_OHOSMODULE      = 12,
    LOG_EXTMODULE       = 15,
    MODULEID_BUTT       = 16
};

enum OM_MSG_MODULEID {
    OM_WIFI = 0x00,
    OM_BT = 0x10,
    OM_GNSS = 0x20,
    OM_DSP = 0x30,
    OM_PF = 0x40,
    OM_NFC = 0x60,
    OM_IR = 0x70,
    OM_SLP = 0XA0,
    OM_BTH = 0xB0,
    OM_MODULEID_BUTT,
};

#endif /* LOG_MODULE_ID_H */
