/**
 * @file
 * Interface Identification APIs from:
 *              RFC 3493: Basic Socket Interface Extensions for IPv6
 *                  Section 4: Interface Identification
 */

/*
 * Copyright (c) 2017 Joel Cunningham, Garmin International, Inc. <joel.cunningham@garmin.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Joel Cunningham <joel.cunningham@me.com>
 *
 */
 /*
* This file contains the implementation for RFC 2553/3493 section Section 4: Interface Identification
* The following functions are implemented
*  lwip_if_indextoname
*  lwip_if_nametoindex
*  lwip_if_nameindex
*  lwip_if_freenameindex
*  @note: when passing the ifname pointer to the function lwip_if_indextoname:
*  The ifname argument must point to a buffer of at least IF_NAMESIZE   bytes into which the interface
*  name corresponding to the specified
*  index is returned.
*  IF_NAMESIZE is same as IFNAMSIZ and is of 16 bytes including null
*
*/

#ifndef LWIP_HDR_IF_H
#define LWIP_HDR_IF_H

#include "lwip/opt.h"

#if LWIP_SOCKET /* don't build if not configured for use in lwipopts.h */

#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !LWIP_LITEOS_COMPAT && !LWIP_FREERTOS_COMPAT

#ifndef IF_NAMESIZE
#define IF_NAMESIZE NETIF_NAMESIZE
#endif

/**
 * Structure to store the if_index and if_name for each interface.
 */
struct if_nameindex {
  unsigned int if_index;
  char *if_name;
};

#endif

/*
Func Name:  lwip_if_indextoname
*/
/**
* @ingroup if_api
* @brief
* This function maps an interface index into its corresponding name.
* The ifname argument must point to a buffer of at least IF_NAMESIZE bytes which will contain the
* interface name.
* @param[in]    ifindex  Specifies an interface index for which the name is to be retrieved.
* @param[out]   ifname   Specifies the output interface name which is retrieved based on ifindex
*
* @return
*  char * ifname: On success \n
*  NULL  : On failure. The errno is set to indicate the error. \n
*
* @par Errors
*   @li The if_indextoname() function fails in the following conditions:
*     - <b> [ENXIO] </b>: \n The ifindex is more than the max range /There is no name as per the index specified
*     - <b> [EFAULT] </b>: \n The ifname is a null pointer
* @par POSIX Conformance:
* Implementation deviates from RFC 3493, POSIX.1-2001.The following are the exceptions to conformance:
*   Posix expects errno for scenario If there was a system error (such as running out of memory),
*   if_indextoname() returns NULL and errno would be set to the proper value (e.g., ENOMEM).
*   Stack do not allocate memory in this function , so ENOMEM is not returned
* @note
* The caller of this function should ensure that the ifname point to a buffer of at least IF_NAMESIZE bytes.
* If there is no interface corresponding to the specified index, NULL is returned, and errno is set to ENXIO.
* Stack only supports ifindex to be max 255 and the index should start from 1 to get a valid name.
*/

char * lwip_if_indextoname(unsigned int ifindex, char *ifname);

/*
Func Name:  lwip_if_nametoindex
*/
/**
* @ingroup if_api
* @brief
* This function maps an interface name into its corresponding index.
* The ifname argument must point to a buffer of at least IF_NAMESIZE bytes which will contain the
* interface name.
*
* @param[in]    ifname   Specifies the interface name for which corresponding interface
*                        index is required.
*
* @return
* unsigned int ifindex: On success.The interface index for the interface name mentioned in ifname \n
*  0  : On failure. The errno is set to indicate the error. \n
* @par Errors
*   @li The lwip_if_nametoindex() function fails in the following conditions:
*     - <b> [ENODEV] </b>: \n Could not find the index corresponding to the interface name
*     - <b> [EFAULT] </b>: \n The ifname is a null pointer.
* @par POSIX Conformance:
* Implementation deviates from RFC 3493, POSIX.1-2001.The following are the exceptions to conformance:
*   POSIX expects errno for scenario If there was a system error (such as running out of memory),
*   if_nametoindex() returns 0 and errno would be set to the proper value (e.g., ENOMEM).
*   Stack does not allocate memory in this function, so ENOMEM is not returned.
* @note
* The caller of this function should ensure that the ifname point to a buffer of at least
* IF_NAMESIZE bytes. If there is no interface index corresponding to the specified name,
* 0 is returned, and errno is set to ENODEV.
* 0 is an invalid index.
*/
unsigned int lwip_if_nametoindex(const char *ifname);

/*
Func Name:  lwip_if_nameindex
*/
/**
* @ingroup if_api
* @brief
* This function returns an array of if_nameindex structures, one structure per interface.
* The if_nameindex structure holds the information about a single interface and is defined as a result of
* including the <if_api.h> header.
* The end of the array of structures is indicated by a structure with an if_index of 0 and an if_name of NULL.
* @param[in]    void     The function does not take any input
*
* @return
*  struct if_nameindex * : On success. The function returns an array of if_nameindex structures, one structure
per interface. \n
*  NULL  : On failure. The errno is set to indicate the error.\n
* @par Errors
*   @li The if_nameindex() function fails in the following conditions:
*     - <b> [ENOBUFS] </b>: \n Insufficient resources available. For this, the function returns NULL.
* @par POSIX Conformance:
* Implementation conforms to  POSIX.1-2001, POSIX.1-2008, RFC 3493.
* @note
* The end of the array of structures is indicated by a structure with an if_index of 0 and an if_name of NULL.\n
* The memory used for this array of structures along with the interface names pointed to by the if_name members
* is obtained dynamically, and must be freed by calling lwip_if_freenameindex().
*/
struct if_nameindex  *lwip_if_nameindex(void);

/*
Func Name:  lwip_if_freenameindex
*/
/**
* @ingroup if_api
* @brief
* This function frees the dynamic memory that was allocated by lwip_if_nameindex().
* @param[in]    ptr      The argument to this function must be a pointer that was returned by lwip_if_nameindex()
*
* @return
*  void  : On success.\n
*  void  : On failure. The errno is not set.\n
* @par POSIX Conformance:
* Implementation conforms to  POSIX.1-2001, POSIX.1-2008, RFC 3493.
*/
void  lwip_if_freenameindex(struct if_nameindex *ptr);

#ifndef __DOXYGEN__
#ifdef LWIP_COMPAT_SOCKETS
#if LWIP_COMPAT_SOCKETS != 2
#define if_indextoname(ifindex, ifname)  lwip_if_indextoname(ifindex, ifname)
#define if_nametoindex(ifname)          lwip_if_nametoindex(ifname)
#define if_nameindex(void)              lwip_if_nameindex(void)
#define if_freenameindex(ptr)           lwip_if_freenameindex(ptr)
#endif /* LWIP_COMPAT_SOCKETS!=2 */
#endif /* LWIP_COMPAT_SOCKETS */

#ifdef LWIP_COMPAT_SOCKETS
#if LWIP_COMPAT_SOCKETS == 2
#define lwip_if_indextoname(ifindex, ifname)  if_indextoname(ifindex, ifname)
#define lwip_if_nametoindex(ifname)          if_nametoindex(ifname)
#define lwip_if_nameindex(void)              if_nameindex(void)
#define lwip_if_freenameindex(ptr)           if_freenameindex(ptr)
#endif /* LWIP_COMPAT_SOCKETS == 2 */
#endif /* LWIP_COMPAT_SOCKETS */
#endif /* __DOXYGEN__ */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_SOCKET */

#endif /* LWIP_HDR_IF_H */
