#ifndef _LINUX_STAT_H
#define _LINUX_STAT_H

#include "sys/stat.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if !defined(S_IRUGO)
#define COMPAT_S_IRUGO (S_IRUSR | S_IRGRP | S_IROTH)
#define S_IRUGO COMPAT_S_IRUGO
#endif
#if !defined(S_IWUGO)
#define COMPAT_S_IWUGO (S_IWUSR | S_IWGRP | S_IWOTH)
#define S_IWUGO COMPAT_S_IWUGO
#endif
#if !defined(S_IXUGO)
#define COMPAT_S_IXUGO (S_IXUSR | S_IXGRP | S_IXOTH)
#define S_IXUGO COMPAT_S_IXUGO
#endif
#if !defined(S_IRWXUGO)
#define COMPAT_S_IRWXUGO (S_IRWXU | S_IRWXG | S_IRWXO)
#define S_IRWXUGO COMPAT_S_IRWXUGO
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_STAT_H */
