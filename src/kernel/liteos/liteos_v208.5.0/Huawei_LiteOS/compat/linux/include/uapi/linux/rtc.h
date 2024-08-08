#ifndef _HISOC_RTC_H
#define _HISOC_RTC_H

#include "asm/platform.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PERI_CRG57 (CRG_REG_BASE + 0xE4)
#define BIT_TEMP_SRST_REQ 2
struct rtc_time {
  int tm_isdst;
  int tm_yday;
  int tm_wday;
  int tm_year;
  int tm_mon;
  int tm_mday;
  int tm_hour;
  int tm_min;
  int tm_sec;
};
struct rtc_wkalrm {
  struct rtc_time time;    /* time the alarm is set to */
  UINT8 pending;           /* 0 = alarm not pending, 1 = alarm pending */
  UINT8 enabled;           /* 0 = alarm disabled, 1 = alarm enabled */
};
struct rtc_pll_info {
  long pll_clock;       /* base PLL frequency */
  INT32 pll_negmult;    /* factor for -ve correction */
  INT32 pll_posmult;    /* factor for +ve correction */
  INT32 pll_min;        /* max -ve (slower) adjustment value */
  INT32 pll_max;        /* max +ve (faster) adjustment value */
  INT32 pll_value;      /* get/set correction value */
  INT32 pll_ctrl;       /* placeholder for fancier control */
};
#define RTC_AIE_ON _IO('p', 0x01)
#define RTC_AIE_OFF _IO('p', 0x02)
#define RTC_UIE_ON _IO('p', 0x03)
#define RTC_UIE_OFF _IO('p', 0x04)
#define RTC_PIE_ON _IO('p', 0x05)
#define RTC_PIE_OFF _IO('p', 0x06)
#define RTC_WIE_ON _IO('p', 0x0f)
#define RTC_WIE_OFF _IO('p', 0x10)
#define RTC_ALM_SET _IOW('p', 0x07, struct rtc_time)
#define RTC_ALM_READ _IOR('p', 0x08, struct rtc_time)
#define RTC_RD_TIME _IOR('p', 0x09, struct rtc_time)
#define RTC_SET_TIME _IOW('p', 0x0a, struct rtc_time)
#define RTC_IRQP_READ _IOR('p', 0x0b, unsigned long)
#define RTC_IRQP_SET _IOW('p', 0x0c, unsigned long)
#define RTC_EPOCH_READ _IOR('p', 0x0d, unsigned long)
#define RTC_EPOCH_SET _IOW('p', 0x0e, unsigned long)
#define RTC_WKALM_SET _IOW('p', 0x0f, struct rtc_wkalrm)
#define RTC_WKALM_RD _IOR('p', 0x10, struct rtc_wkalrm)
#define RTC_PLL_GET _IOR('p', 0x11, struct rtc_pll_info)
#define RTC_PLL_SET _IOW('p', 0x12, struct rtc_pll_info)
#define RTC_VL_READ _IOR('p', 0x13, int)
#define RTC_VL_CLR _IO('p', 0x14)
#define RTC_IRQF 0x80
#define RTC_PF 0x40
#define RTC_AF 0x20
#define RTC_UF 0x10
#define RTC_MAX_FREQ 8192

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
