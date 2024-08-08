#ifndef _LINUX_IO_H
#define _LINUX_IO_H

#include "asm/io.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define IO_SPACE_LIMIT 0xffffffff

#define __io(a)        __typesafe_io(a)
#define IO_ADDRESS(x)          (x)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_IO_H */

