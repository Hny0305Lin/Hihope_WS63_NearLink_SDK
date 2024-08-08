#ifndef _LINUX_STRINGIFY_H
#define _LINUX_STRINGIFY_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Indirect stringification.  Doing two levels allows the parameter to be a
 * macro itself.  For example, compile with -DFOO=bar, __stringify(FOO)
 * converts to "bar".
 */
#define __stringify_1(x...)    #x
#define __stringify(x...)      __stringify_1(x)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_STRINGIFY_H */