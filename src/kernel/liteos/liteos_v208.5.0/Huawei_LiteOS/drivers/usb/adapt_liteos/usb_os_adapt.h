/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Usb LiteOS Adapt Macro Definition
 * Author: Huawei LiteOS Team
 * Create: 2021-12-24
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

#ifndef _USB_OS_ADAPT_H
#define _USB_OS_ADAPT_H

/* Memory address handler */
/* DMA Operation */

#define USB_DMA_CACHE_INV	dma_cache_inv
#define USB_DMA_CACHE_CLR	dma_cache_clean

#define USB_DMA_PA_TO_VA(daddr)	(daddr)
#define USB_DMA_VA_TO_PA(vaddr)	(vaddr)

/* VM ZONE */

#define IO_DEVICE_ADDR(addr)	(addr)

/* Mutex */

#define USB_MUTEX_INIT_VAL	{ MTX_NOT_INIT }
#define INVALID_MUX 0xFFFFFFFF

/* Queue */

#define USB_QUEUE_INVALID	KERNEL_QUEUE_LIMIT

/* Basic module dependency adapt */
/* Task init */

#define USB_TASK_PARAM_INIT_ARG	LOS_TASK_PARAM_INIT_ARG

/* Return value define */

#ifndef BUS_PROBE_GENERIC
#define	BUS_PROBE_GENERIC	0
#endif

/* Dev attaching flag */

#define IS_USB_DEVICE_ATTACHING(dev)	((dev)->dev_attaching)
#define USBD_SET_ATTACHING(dev)		((dev)->dev_attaching = 1)

/* Header file path  */

#define USB3_HEADER_PATH	"soc/usb3.h"
#define USB_HEADER_PATH		"soc/usb.h"
#define UART_HEADER_PATH	"soc/uart.h"

#endif /* _USB_OS_ADAPT_H */