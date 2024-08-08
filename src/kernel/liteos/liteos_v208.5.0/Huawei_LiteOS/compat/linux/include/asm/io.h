#ifndef _ASM_IO_H
#define _ASM_IO_H

#include "los_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define writeb(value, address)  WRITE_UINT8(value, address)
#define writew(value, address)  WRITE_UINT16(value, address)
#define writel(value, address)  WRITE_UINT32(value, address)

#define readb(address)          GET_UINT8(address)
#define readw(address)          GET_UINT16(address)
#define readl(address)          GET_UINT32(address)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASM_IO_H */
