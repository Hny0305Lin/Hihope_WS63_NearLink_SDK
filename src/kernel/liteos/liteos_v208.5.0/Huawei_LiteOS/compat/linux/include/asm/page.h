#ifndef _ASM_PAGE_H
#define _ASM_PAGE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef PAGE_SIZE
#define PAGE_SIZE    (0x1000U)
#endif
#define PAGE_MASK    (~(PAGE_SIZE - 1))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASM_PAGE_H */
