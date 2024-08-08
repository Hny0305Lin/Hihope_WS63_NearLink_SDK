/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  common logging producer interface - need to change name of log.h in all the protocol core files
 */

#ifndef _LOG_DEF_H_
#define _LOG_DEF_H_

#include "core.h"

#ifdef BTH_SYS_PART
#include "log_def_bth.h"
#else
#include "log_def_bt.h"
#endif

#include "log_def_pf.h"
#include "log_def_dsp.h"
#ifdef GNSS_SYS_PART
#include "log_def_gnss.h"
#endif

#endif
