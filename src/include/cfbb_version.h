/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: CFBB version define. \n
 *
 */

#ifndef CFBB_VERSION_H
#define CFBB_VERSION_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/* Major version number (X.y.z[.p]) */
#define CFBB_VER_MAJOR      0

/* Minor version number (x.Y.z[.p]) */
#define CFBB_VER_MINOR      9

/* Release version nubmer (x.y.Z[.p]) */
#define CFBB_VER_RELEASE    0

/* Patch version number (x.y.z[.P]) */
#define CFBB_VER_PATCH      5


/**
 * MACRO to convert CFBB version nubmer into an integer
 * To be used in comparision, such as CFBB_VERION >= CFBB_VERSION_VALUE(1, 0, 0, 0)
 */
#define CFBB_VERSION_VALUE(x, y, z, p)      (((x) << 24) | ((y) << 16) | ((z) << 8) | (p))

/**
 * Current CFBB version, as an integer
 * To be used in comparision, such as CFBB_VERION >= CFBB_VERSION_VALUE(1, 0, 0, 0)
 */
#define CFBB_VERSION    CFBB_VERSION_VALUE(CFBB_VER_MAJOR, CFBB_VER_MINOR, CFBB_VER_RELEASE, CFBB_VER_PATCH)

/**
 * Current CFBB version, as a string
 */
#if CFBB_VER_PATCH == 0
#define MAKE_VERSION_STR_IMPL(x, y, z, p)   "CFBB "#x"."#y"."#z
#else
#define MAKE_VERSION_STR_IMPL(x, y, z, p)   "CFBB "#x"."#y"."#z"."#p
#endif

#define MAKE_VERSION_STR(x, y, z, p)        MAKE_VERSION_STR_IMPL(x, y, z, p)

#define CFBB_VERSION_STR    MAKE_VERSION_STR(CFBB_VER_MAJOR, CFBB_VER_MINOR, CFBB_VER_RELEASE, CFBB_VER_PATCH)

/**
 * @if Eng
 * @brief   Current CFBB version, as a string.
 * @retval  Version     CFBB version.
 * @else
 * @brief   CFBB版本号，字符串格式。
 * @retval  Version     CFBB版本号.
 * @endif
 */
static inline const char *uapi_get_cfbb_version(void)
{
    return CFBB_VERSION_STR;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* CFBB_VERSION_H */
