#ifndef VENDOR_REDEFINE_TEE_API_H
#define VENDOR_REDEFINE_TEE_API_H

#if defined(VENDOR_REDEFINE_TEE_API_C)
#include "libc_wrapper.h"

#define strcmp tee_strcmp
#define memcmp tee_memcmp

#endif /* VENDOR_REDEFINE_TEE_API_C */
#endif /* VENDOR_REDEFINE_TEE_API_H */
