/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Usb Api Private File
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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

#ifndef _USB_API_H
#define	_USB_API_H

#include <los_config.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* extern struct module_data bsd_##name##_##busname##_driver_mod */
#ifdef LOSCFG_DRIVERS_USB_HOST_EHCI
/* host or device controller modules */
/* usb controller modules  link xxx-controller bus */
extern struct module_data bsd_usbus_ehci_driver_mod;
#endif

#ifdef LOSCFG_DRIVERS_USB_HOST_OHCI
extern struct module_data bsd_usbus_ohci_driver_mod;
#endif

#ifdef LOSCFG_DRIVERS_USB_HOST_XHCI
extern struct module_data bsd_usbus_xhci_driver_mod;
#endif

#ifdef LOSCFG_DRIVERS_USB_HOST_DRIVER
/* root hub driver modules  link to usb bus */
extern struct module_data bsd_uhub_usbus_driver_mod;
/* xxx class driver moudles  link to usb hub bus */
extern struct module_data bsd_uhub_uhub_driver_mod;
#endif

#ifdef LOSCFG_DRIVERS_USB_4G_MODEM
extern struct module_data bsd_cdce_uhub_driver_mod;
#endif

#if defined (LOSCFG_DRIVERS_USB_SERIAL) || defined (LOSCFG_DRIVERS_USB_4G_MODEM)
extern struct module_data bsd_u3g_uhub_driver_mod;
#endif

#ifdef LOSCFG_DRIVERS_USB_ETHERNET
extern struct module_data bsd_axe_uhub_driver_mod;
extern struct module_data bsd_axge_uhub_driver_mod;
#endif

#ifdef LOSCFG_DRIVERS_USB_RNDIS_HOST
extern struct module_data bsd_urndis_uhub_driver_mod;
#endif

#ifdef LOSCFG_DRIVERS_USB_MASS_STORAGE
extern struct module_data bsd_umass_uhub_driver_mod;
#endif

#ifdef LOSCFG_DRIVERS_USB_HOST_UVC_CLASS
extern struct module_data bsd_uvc_uhub_driver_mod;
#endif

#ifdef LOSCFG_DRIVERS_USB_WIRELESS
extern struct module_data bsd_usb_linux_uhub_driver_mod;
#endif

#ifdef LOSCFG_DRIVERS_USB_HOST_HID
extern struct module_data bsd_uhid_uhub_driver_mod;
#endif

#ifdef LOSCFG_DRIVERS_USB_HOST_OHCI
int ohci_hcd_init(void);
int ohci_hcd_exit(void);
#endif

#ifdef LOSCFG_DRIVERS_USB_HOST_XHCI
int xhci_hcd_init(void);
int xhci_hcd_exit(void);
#endif

#ifdef LOSCFG_DRIVERS_USB_HOST_EHCI
int ehci_hcd_init(void);
int ehci_hcd_exit(void);
#endif

#ifdef LOSCFG_DRIVERS_USB2_DEVICE_CONTROLLER
int udc_init(void);
int udc_exit(void);
#endif

#ifdef LOSCFG_DRIVERS_USB3_DEVICE_CONTROLLER
int udc3_init(void);
int udc3_exit(void);
#endif

#ifdef LOSCFG_USB_DEBUG
void usb_debug_module_regsiter(void);
void usb_debug_module_unregsiter(void);
#endif

const char *fetach_usbversion(void);

unsigned int userial_mask_get(void);
void userial_mask_set(unsigned int val);

#define OHCI_BUS_NAME		"OHCI"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _USB_API_H */

