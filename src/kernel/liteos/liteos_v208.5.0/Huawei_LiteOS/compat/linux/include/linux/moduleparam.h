#ifndef _LINUX_MODULEPARAM_H
#define _LINUX_MODULEPARAM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef module_param
#define module_param(name, type, perm)
#endif
#ifndef MODULE_LICENSE
#define MODULE_LICENSE(l)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_MODULEPARAM_H */
