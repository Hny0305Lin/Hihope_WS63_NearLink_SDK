#ifndef _ASM_LINKAGE_H
#define _ASM_LINKAGE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define __ALIGN .align 0
#define __ALIGN_STR ".align 0"

#define ENDPROC(name)      \
    .type name, %function; \
    END(name)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
