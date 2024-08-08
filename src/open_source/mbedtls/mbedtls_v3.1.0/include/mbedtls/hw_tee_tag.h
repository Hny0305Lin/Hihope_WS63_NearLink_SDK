#ifndef HUWAEI_TEE_TAG_H
#define HUWAEI_TEE_TAG_H

#if defined(HUWAEI_TEE_TAG_C)
#include "tag.h"
#define VMP_TAG     VMP_TAG
#define STRLITE_TAG STRLITE_TAG
#define FLA_TAG     FLA_TAG
#define STROBF_TAG  STROBF_TAG
#define FORCEINLINE FORCEINLINE
#else
#define VMP_TAG
#define STRLITE_TAG
#define FLA_TAG
#define STROBF_TAG
#define FORCEINLINE
#endif

#endif
