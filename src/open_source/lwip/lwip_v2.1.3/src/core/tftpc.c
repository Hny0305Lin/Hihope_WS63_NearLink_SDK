/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: implementation for tftp client
 * Author: none
 * Create: 2020
 */


#include "lwip/opt.h"

#if LWIP_TFTP /* don't build if not configured for use in lwipopts.h */

#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/mem.h"
#include "lwip/err.h"
#include "lwip/inet.h"
#include "netif/etharp.h"
#include "lwip/ip_addr.h"
#include "lwip/sockets.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/def.h"
#include "lwip/tftpc.h"
#include "fcntl.h"
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include <sys/stat.h>

/* Function Declarations */
#ifdef LOSCFG_NET_LWIP_SACK_TFTP
static u32_t lwip_tftp_create_bind_socket(s32_t *pi_socketid);

static s32_t lwip_tftp_make_tftp_packet(u16_t us_opcode, const s8_t *sz_filename,
                                        u32_t ul_mode, tftpc_packet_s *pst_packet);

u32_t lwip_tftp_recv_from_server(s32_t i_socknum,
                                 u32_t *pul_size,
                                 tftpc_packet_s *pst_recvbuf,
                                 u32_t *pul_ignorepkt,
                                 struct sockaddr_in *pst_serveraddr,
                                 tftpc_packet_s *pst_sendbuf);

u32_t lwip_tftp_send_to_server(s32_t i_socknum, u32_t ul_size,
                               tftpc_packet_s *pst_sendbuf,
                               struct sockaddr_in *pst_serveraddr);

u32_t lwip_tftp_validate_data_pkt(s32_t i_socknum,
                                  u32_t *pul_size,
                                  tftpc_packet_s *pst_recvbuf,
                                  u16_t us_currblk, u32_t *pul_resendpkt,
                                  struct sockaddr_in  *pst_serveraddr);

u32_t lwip_tftp_inner_put_file(s32_t i_socknum, tftpc_packet_s *pst_sendbuf,
                               u32_t ulLength, u16_t us_currblk,
                               struct sockaddr_in *pst_serveraddr);

static void lwip_tftp_send_error(s32_t i_socknum, u32_t ul_error, const char *sz_errmsg,
                                 struct sockaddr_in *pst_serveraddr, tftpc_packet_s *pst_sendbuf);


/* Create and bind a UDP socket. */
static u32_t
lwip_tftp_create_bind_socket(s32_t *pi_socketid)
{
  int retval;
  struct sockaddr_in st_client_addr;
  u32_t ul_temp_clientip;
  u32_t set_non_block_socket = 1;

  /* create a socket */
  *pi_socketid = lwip_socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (*pi_socketid == -1) {
    LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_create_bind_socket : lwip_socket create socket failed\n"));
    return TFTPC_SOCKET_FAILURE;
  }

  /* Make the socket as NON-BLOCKING. */
  retval = lwip_ioctl(*pi_socketid, (long)FIONBIO, &set_non_block_socket);
  if (retval != 0) {
    (void)lwip_close(*pi_socketid);
    *pi_socketid = TFTP_NULL_INT32;
    return TFTPC_IOCTLSOCKET_FAILURE;
  }

  ul_temp_clientip = INADDR_ANY;

  /* specify a local address for this socket */
  (void)memset_s(&st_client_addr, sizeof(st_client_addr), 0, sizeof(st_client_addr));
  st_client_addr.sin_family = AF_INET;
  st_client_addr.sin_port = 0;
  st_client_addr.sin_addr.s_addr = htonl(ul_temp_clientip);

  retval = lwip_bind(*pi_socketid, (struct sockaddr *)&st_client_addr, sizeof(st_client_addr));
  if (retval != 0) {
    (void)lwip_close(*pi_socketid);
    *pi_socketid = TFTP_NULL_INT32;
    return TFTPC_BIND_FAILURE;
  }

  return ERR_OK;
}

/*
 * Function to create TFTP packet.
 * us_opcode - indiacting the nature of the operation
 * sz_filename -filename on which the operation needs to done
 * ul_mode -mode in which the operation needs to done
 * pst_packet - packet generated
 * Returns packet address on success
 */
static s32_t
lwip_tftp_make_tftp_packet(u16_t us_opcode, const s8_t *sz_filename, u32_t ul_mode, tftpc_packet_s *pst_packet)
{
  s8_t *pc_cp = NULL;

  pst_packet->us_opcode = htons(us_opcode);
  pc_cp = pst_packet->u.uc_name_mode;

  /*
   * Request packet format is:
   * | Opcode |  Filename  |   0  |    Mode    |   0  |
   */
  (void)strncpy_s((char *)pc_cp, TFTP_MAX_PATH_LENGTH, (char *)sz_filename, (TFTP_MAX_PATH_LENGTH - 1));
  pc_cp[(TFTP_MAX_PATH_LENGTH - 1)] = '\0';

  pc_cp += (strlen((char *)sz_filename) + 1);
  if (ul_mode == TRANSFER_MODE_BINARY) {
    (void)strncpy_s((char *)pc_cp, TFTP_MAX_MODE_SIZE, "octet", (TFTP_MAX_MODE_SIZE - 1));
    pc_cp[(TFTP_MAX_MODE_SIZE - 1)] = '\0';
  } else if (ul_mode == TRANSFER_MODE_ASCII) {
    (void)strncpy_s((char *)pc_cp, TFTP_MAX_MODE_SIZE, "netascii", (TFTP_MAX_MODE_SIZE - 1));
    pc_cp[(TFTP_MAX_MODE_SIZE - 1)] = '\0';
  }

  pc_cp += (strlen((char *)pc_cp) + 1);

  return (pc_cp - (s8_t *)pst_packet);
}

/*
 * Function to recv a packet from server
 * i_socknum - Socket Number
 * pst_serveraddr - Server address
 * pul_ignorepkt - Ignore packet flag
 * pst_recvbuf - received packet
 * pul_size - Size of the packet
 */
u32_t
lwip_tftp_recv_from_server(s32_t i_socknum, u32_t *pul_size,
                           tftpc_packet_s *pst_recvbuf, u32_t *pul_ignorepkt,
                           struct sockaddr_in *pst_serveraddr, tftpc_packet_s *pst_sendbuf)
{
  u32_t ulerror;
  socklen_t sl_fromaddrlen;
  struct sockaddr_in st_fromaddr;
  fd_set st_readfds;
  struct timeval st_timeout;
  u16_t us_opcode; /* Opcode value */
  s32_t iret;

  sl_fromaddrlen = sizeof(st_fromaddr);
  st_timeout.tv_sec = TFTPC_TIMEOUT_PERIOD;
  st_timeout.tv_usec = 0;
  if ((pul_size == NULL) || (pst_recvbuf == NULL) || (pst_serveraddr == NULL) || (pst_sendbuf == NULL)) {
    return ERR_ARG;
  }
  /* wait for DATA packet */
  FD_ZERO(&st_readfds);
  FD_SET(i_socknum, &st_readfds);

  iret = lwip_select((s32_t)(i_socknum + 1), &st_readfds, 0, 0, &st_timeout);
  if (iret == -1) {
    return TFTPC_SELECT_ERROR;
  } else if (iret == 0) {
    return TFTPC_TIMEOUT_ERROR; /* Select timeout occured */
  }

  if (!FD_ISSET(i_socknum, &st_readfds)) {
    return TFTPC_TIMEOUT_ERROR; /* FD not set */
  }

  /* receive a packet from server */
  iret = lwip_recvfrom(i_socknum, (s8_t *)pst_recvbuf, TFTP_PKTSIZE, 0,
                       (struct sockaddr *)&st_fromaddr, &sl_fromaddrlen);
  if (iret <= 0) {
    return TFTPC_RECVFROM_ERROR;
  }

  /* If received packet size < minimum packet size */
  if (iret < TFTPC_FOUR) {
    /* Send Error packet to server */
    lwip_tftp_send_error(i_socknum,
                         TFTPC_PROTOCOL_PROTO_ERROR,
                         "Packet size < min size",
                         pst_serveraddr, pst_sendbuf);

    return TFTPC_PKT_SIZE_ERROR;
  }

  /* convert network opcode to host format after receive. */
  us_opcode = ntohs(pst_recvbuf->us_opcode);
  /* if this packet is ERROR packet */
  if (us_opcode == TFTPC_OP_ERROR) {
    ulerror = ntohs(pst_recvbuf->u.sttftp_err.us_errnum);

    /*
     * If the error is according to RFC,then convert to lwip error codes.
     * Constant values are used in the cases as these error codes are as per
     * the RFC, these are constant values returned by many standard TFTP
     * serevrs
     */
    switch (ulerror) {
      case TFTPC_PROTOCOL_USER_DEFINED:
        ulerror = TFTPC_ERROR_NOT_DEFINED;
        break;
      case TFTPC_PROTOCOL_FILE_NOT_FOUND:
        ulerror = TFTPC_FILE_NOT_FOUND;
        break;
      case TFTPC_PROTOCOL_ACCESS_ERROR:
        ulerror = TFTPC_ACCESS_ERROR;
        break;
      case TFTPC_PROTOCOL_DISK_FULL:
        ulerror = TFTPC_DISK_FULL;
        break;
      case TFTPC_PROTOCOL_PROTO_ERROR:
        ulerror = TFTPC_PROTO_ERROR;
        break;
      case TFTPC_PROTOCOL_UNKNOWN_TRANSFER_ID:
        ulerror = TFTPC_UNKNOWN_TRANSFER_ID;
        break;
      case TFTPC_PROTOCOL_FILE_EXISTS:
        ulerror = TFTPC_FILE_EXISTS;
        break;
      case TFTPC_PROTOCOL_CANNOT_RESOLVE_HOSTNAME:
        ulerror = TFTPC_CANNOT_RESOLVE_HOSTNAME;
        break;
      default:
        ulerror = TFTPC_ERROR_NOT_DEFINED;
        break;
    }

    /* If length of error msg > 100 chars */
    pst_recvbuf->u.sttftp_err.uc_errmesg[TFTP_MAXERRSTRSIZE - 1] = '\0';

    LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_recv_from_server : ERROR pkt received: %s\n",
                             pst_recvbuf->u.sttftp_err.uc_errmesg));

    /* Now we get error block, so return. */
    return ulerror;
  }

  /* Store the size of received block */
  *pul_size = (u32_t)iret;

  /*
   * If received packet is first block of data(for get operation) or if
   * received packet is acknowledgement for write request (put operation)
   * store the received port number
   */
  if (((us_opcode == TFTPC_OP_DATA) && (ntohs(pst_recvbuf->u.sttftp_data.us_blknum) == 1)) ||
      ((us_opcode == TFTPC_OP_ACK) && (ntohs(pst_recvbuf->u.us_blknum) == 0))) {
    /* If received packet from correct server */
    if (st_fromaddr.sin_addr.s_addr == pst_serveraddr->sin_addr.s_addr) {
      /* set the server port to received port */
      pst_serveraddr->sin_port = st_fromaddr.sin_port;
    } else {
      /* Received packet form wrong server. */
      LWIP_DEBUGF(TFTP_DEBUG,
                  ("lwip_tftp_recv_from_server : Received 1st packet from wrong Server or unknown server\n"));

      /* Set ignore packet flag */
      *pul_ignorepkt = 1;
    }
  } else {
    /* If not first packet, check if the received packet is from correct server and from correct port */
    if ((st_fromaddr.sin_addr.s_addr != pst_serveraddr->sin_addr.s_addr) ||
        (pst_serveraddr->sin_port != st_fromaddr.sin_port)) {
      /* Received packet form wrong server or wrong port.Ignore packet. */
      LWIP_DEBUGF(TFTP_DEBUG,
                  ("lwip_tftp_recv_from_server : Received a packet from wrong Server or unknown server\n"));

      /* Set ignore packet flag */
      *pul_ignorepkt = 1;
    }
  }

  return ERR_OK;
}

/*
 * Function to send a packet to server
 * i_socknum: Socket Number
 * ul_size: Size of the packet
 * pst_sendbuf: Packet to send
 * pst_serveraddr: Server address
 */
u32_t
lwip_tftp_send_to_server(s32_t i_socknum,
                         u32_t ul_size,
                         tftpc_packet_s *pst_sendbuf,
                         struct sockaddr_in *pst_serveraddr)
{
  s32_t iret;
  if ((pst_sendbuf == NULL) || (pst_serveraddr == NULL)) {
    return ERR_ARG;
  }
  /* Send packet to server */
  iret = lwip_sendto(i_socknum, (s8_t *)pst_sendbuf,
                     (size_t)ul_size, 0,
                     (struct sockaddr *)pst_serveraddr,
                     sizeof(struct sockaddr_in));
  /* Size of data sent not equal to size of packet */
  if ((iret == TFTP_NULL_INT32) || ((u32_t)iret != ul_size)) {
    return TFTPC_SENDTO_ERROR;
  }

  return ERR_OK;
}

/*
 * lwip_tftp_validate_data_pkt
 * Get the data block from the received packet
 * @param Input i_socknum Socket Number
 *              pul_size: Size of received packet,
 *              pst_recvbuf - received packet
 *              us_currblk - Current block number
 * @param Output pul_resendpkt - Resend packet flag
 * @return VOS_OK on success.else error code
 */
u32_t
lwip_tftp_validate_data_pkt(s32_t i_socknum,
                            u32_t *pul_size,
                            tftpc_packet_s *pst_recvbuf,
                            u16_t us_currblk,
                            u32_t *pul_resendpkt,
                            struct sockaddr_in *pst_serveraddr)
{
  fd_set st_readfds;
  struct timeval st_timeout;
  struct sockaddr_in st_fromaddr;
  socklen_t ul_fromaddrlen;
  s32_t ierror;
  u16_t usblknum;
  u32_t ul_loopcnt = 0;
  if ((pul_size == NULL) || (pst_recvbuf == NULL) ||
      (pul_resendpkt == NULL) || (pst_serveraddr == NULL)) {
    return ERR_ARG;
  }
  s32_t irecvlen = (s32_t)*pul_size;
  ul_fromaddrlen = sizeof(st_fromaddr);

  /* Initialize from address to the server address at first */
  if (memcpy_s((void *)&st_fromaddr, sizeof(struct sockaddr_in), (void *)pst_serveraddr, sizeof(st_fromaddr)) != EOK) {
    LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_validate_data_pkt : memcpy_s error\n"));
    return TFTPC_MEMCPY_FAILURE;
  }

  /* Get Block Number */
  usblknum = ntohs(pst_recvbuf->u.sttftp_data.us_blknum);
  /* Now data blocks are not in sync. */
  if (usblknum != us_currblk) {
    /* Set timeout value */
    st_timeout.tv_sec = 1;
    st_timeout.tv_usec = 0;

    /* Reset any stored packets. */
    FD_ZERO(&st_readfds);
    FD_SET(i_socknum, &st_readfds);

    ierror = lwip_select((s32_t)(i_socknum + 1),
                         &st_readfds, 0, 0, &st_timeout);

    /* Loop to get the last data packet from the receive buffer */
    while ((ierror != TFTP_NULL_INT32) && (ierror != 0)) {
      ul_loopcnt++;

      /*
       * MAX file size in TFTP is 32 MB.
       * Reason for keeping 75 here , is ((75*512=38400bytes)/1024) =  37MB. So the recv/Send
       * Loop can receive the complete MAX message from the network.
       */
      if (ul_loopcnt > TFTPC_MAX_WAIT_IN_LOOP) {
        LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_validate_data_pkt : unexpected packets are received repeatedly\n"));
        *pul_size = TFTP_NULL_UINT32;
        return TFTPC_PKT_SIZE_ERROR;
      }

      FD_ZERO(&st_readfds);
      FD_SET(i_socknum, &st_readfds);

      irecvlen = lwip_recvfrom(i_socknum,
                               (s8_t *)pst_recvbuf,
                               TFTP_PKTSIZE, 0,
                               (struct sockaddr *)&st_fromaddr,
                               &ul_fromaddrlen);
      if (irecvlen == -1) {
        *pul_size = TFTP_NULL_UINT32;

        /* return from the function, recvfrom operation failed */
        return TFTPC_RECVFROM_ERROR;
      }

      st_timeout.tv_sec = 1;
      st_timeout.tv_usec = 0;
      ierror = lwip_select((s32_t)(i_socknum + 1), &st_readfds, 0, 0, &st_timeout);
    }

    /* If received packet size < minimum packet size */
    if (irecvlen < TFTPC_FOUR) {
      return TFTPC_PKT_SIZE_ERROR;
    }

    /*
     * Check if the received packet is from correct server and from
     * correct port
     */
    if ((st_fromaddr.sin_addr.s_addr != pst_serveraddr->sin_addr.s_addr) ||
        (pst_serveraddr->sin_port != st_fromaddr.sin_port)) {
      /* resend ack packet */
      *pul_resendpkt = 1;
      LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_validate_data_pkt : Received pkt from unknown server\n"));
      return ERR_OK;
    }

    /* if this packet is not DATA packet */
    if (TFTPC_OP_DATA != ntohs(pst_recvbuf->us_opcode)) {
      LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_validate_data_pkt : Received pkt not a DATA pkt\n"));

     /*
      * return from the function, incorrect packet received,
      * expected packet is data packet
      */
      return TFTPC_PROTO_ERROR;
    }

    usblknum = ntohs(pst_recvbuf->u.sttftp_data.us_blknum);
    /*
     * if we now have the earlier data packet, then the host probably
     * never got our acknowledge packet, now we will send it again.
     */
    if (usblknum == (us_currblk - 1)) {
      /* resend ack packet */
      *pul_resendpkt = 1;
      LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_validate_data_pkt : Received previous DATA pkt\n"));

      return ERR_OK;
    }

    /* If the block of data received is not current block or also previous block, then it is abnormal case. */
    if (usblknum != us_currblk) {
      LWIP_DEBUGF(TFTP_DEBUG,
                  ("lwip_tftp_validate_data_pkt : Received DATA pkt no. %"S32_F" instead of pkt no.%"S32_F"\n",
                   usblknum, us_currblk));

      return TFTPC_SYNC_FAILURE;
    }
  }

  *pul_size = (u32_t)irecvlen;
  return ERR_OK;
}

/*
 * Send an error packet to the server
 * i_socknum : Socket Number
 * ul_error: Error code
 * sz_errmsg - Error message
 * pst_serveraddr - Server address
 */
static void
lwip_tftp_send_error(s32_t i_socknum, u32_t ul_error, const char *sz_errmsg,
                     struct sockaddr_in *pst_serveraddr, tftpc_packet_s *pst_sendbuf)
{
  u16_t us_opcode = TFTPC_OP_ERROR;

  (void)memset_s((void *)pst_sendbuf, sizeof(tftpc_packet_s), 0, sizeof(tftpc_packet_s));

  /* Set up the send buffer */
  pst_sendbuf->us_opcode = htons(us_opcode);
  pst_sendbuf->u.sttftp_err.us_errnum = htons((u16_t)ul_error);

  if (strncpy_s((char *)(pst_sendbuf->u.sttftp_err.uc_errmesg), TFTP_MAXERRSTRSIZE,
                (char *)sz_errmsg, (TFTP_MAXERRSTRSIZE - 1)) != EOK) {
    LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_send_error : strncpy_s error\n"));
    return;
  }
  pst_sendbuf->u.sttftp_err.uc_errmesg[(TFTP_MAXERRSTRSIZE - 1)] = '\0';

  /* Send to server */
  if (lwip_tftp_send_to_server(i_socknum,
                               sizeof(tftpc_packet_s),
                               pst_sendbuf,
                               pst_serveraddr) != ERR_OK) {
    LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_send_to_server error."));
    return;
  }
}

/*
 * INTEFACE to get a file using filename
 * ul_hostaddr - IP address of Host
 * sz_srcfilename - Source file
 * sz_destdirpath - Destination file path
 */
u32_t
lwip_tftp_get_file_by_filename(u32_t ul_hostaddr,
                               u16_t us_tftpservport,
                               u8_t uc_tftptransmode,
                               s8_t *sz_srcfilename,
                               s8_t *sz_destdirpath)
{
  s32_t i_socknum = TFTP_NULL_INT32;
  u32_t ul_srcstrlen;
  u32_t ul_deststrlen;
  u32_t ul_size;
  u32_t ul_recvsize = TFTP_NULL_UINT32;
  s32_t i_errcode;
  u32_t ul_errcode;
  u16_t us_readreq;
  u16_t us_tempservport;
  s8_t *psz_tempdestname = NULL;
  s8_t *sz_tempsrcname = NULL;
  u32_t ul_currblk = 1;
  u32_t ul_resendpkt = 0; /* Resend the previous packet */
  u32_t ul_ignorepkt = 0; /* Ignore received packet */
  u32_t ul_totaltime = 0;
  u32_t is_localfileopened = false;

  tftpc_packet_s *pst_sendbuf = NULL;
  tftpc_packet_s *pst_recvbuf = NULL;
  struct sockaddr_in st_serveraddr;
  struct stat sb;
  u32_t is_direxist = 0;
  s32_t fp = -1;

  /* Validate the parameters */
  if ((sz_srcfilename == NULL) || (sz_destdirpath == NULL)) {
    return TFTPC_INVALID_PARAVALUE;
  }

  if ((uc_tftptransmode != TRANSFER_MODE_BINARY) && (uc_tftptransmode != TRANSFER_MODE_ASCII)) {
    return TFTPC_INVALID_PARAVALUE;
  }

  /* check IP address not within ( 1.0.0.0 - 126.255.255.255 ) and ( 128.0.0.0 - 223.255.255.255 ) range. */
  if (!(((ul_hostaddr >= TFTPC_IP_ADDR_MIN) && (ul_hostaddr <= TFTPC_IP_ADDR_EX_RESV)) ||
      ((ul_hostaddr >= TFTPC_IP_ADDR_CLASS_B) && (ul_hostaddr <= TFTPC_IP_ADDR_EX_CLASS_DE)))) {
    return TFTPC_IP_NOT_WITHIN_RANGE;
  }

  /* Check validity of source filename */
  ul_srcstrlen = strlen((char *)sz_srcfilename);
  if ((ul_srcstrlen == 0) || (ul_srcstrlen >= TFTP_MAX_PATH_LENGTH)) {
    return TFTPC_SRC_FILENAME_LENGTH_ERROR;
  }

  /* Check validity of destination path */
  ul_deststrlen = strlen((char *)sz_destdirpath);
  if ((ul_deststrlen >= TFTP_MAX_PATH_LENGTH) || (ul_deststrlen == 0)) {
    return TFTPC_DEST_PATH_LENGTH_ERROR;
  }

  pst_sendbuf = (tftpc_packet_s *)mem_malloc(sizeof(tftpc_packet_s));
  if (pst_sendbuf == NULL) {
    return TFTPC_MEMALLOC_ERROR;
  }

  pst_recvbuf = (tftpc_packet_s *)mem_malloc(sizeof(tftpc_packet_s));
  if (pst_recvbuf == NULL) {
    mem_free(pst_sendbuf);
    return TFTPC_MEMALLOC_ERROR;
  }

  psz_tempdestname = (s8_t *)mem_malloc(TFTP_MAX_PATH_LENGTH);
  if (psz_tempdestname == NULL) {
    mem_free(pst_sendbuf);
    mem_free(pst_recvbuf);
    return TFTPC_MEMALLOC_ERROR;
  }

  /* First time initialize the buffers */
  (void)memset_s((void *)pst_sendbuf, sizeof(tftpc_packet_s), 0, sizeof(tftpc_packet_s));
  (void)memset_s((void *)pst_recvbuf, sizeof(tftpc_packet_s), 0, sizeof(tftpc_packet_s));

  /* If given src filename is a relative path extrac the file name from the path */
  if ((strchr((char *)sz_srcfilename, '/') != 0) || (strchr((char *)sz_srcfilename, '\\') != 0)) {
    /* Move to the end of the src file path */
    sz_tempsrcname = sz_srcfilename + (ul_srcstrlen - 1);

    while (((*(sz_tempsrcname - 1) != '/') &&
            (*(sz_tempsrcname - 1) != '\\')) &&
           (sz_tempsrcname != sz_srcfilename)) {
      sz_tempsrcname--;
    }

    /* Get length of the extracted src filename */
    ul_srcstrlen = strlen((char *)sz_tempsrcname);
  } else {
    /* If not a relative src path use the given src filename */
    sz_tempsrcname = sz_srcfilename;
  }

  (void)memset_s(psz_tempdestname, TFTP_MAX_PATH_LENGTH, 0, TFTP_MAX_PATH_LENGTH);
  if (strncpy_s((char *)psz_tempdestname, TFTP_MAX_PATH_LENGTH,
                (char *)sz_destdirpath, TFTP_MAX_PATH_LENGTH - 1) != EOK) {
    ul_errcode = TFTPC_MEMCPY_FAILURE;
    goto err_handler;
  }
  psz_tempdestname[TFTP_MAX_PATH_LENGTH - 1] = '\0';

  if ((stat((char *)psz_tempdestname, &sb) == 0) && (S_ISDIR(sb.st_mode))) {
    is_direxist = 1;
  }

  if (is_direxist == 1) {
    /* The filename is not present concat source filename and try */
    if ((ul_deststrlen + ul_srcstrlen) >= TFTP_MAX_PATH_LENGTH) {
      /* If concatenating src filename exceeds 256 bytes */
      ul_errcode = TFTPC_DEST_PATH_LENGTH_ERROR;
      goto err_handler;
    }

    /* Check if / present at end of string */
    if ((psz_tempdestname[ul_deststrlen - 1] != '/') &&
        (psz_tempdestname[ul_deststrlen - 1] != '\\')) {
      if ((ul_deststrlen + ul_srcstrlen + 1) >= TFTP_MAX_PATH_LENGTH) {
        /* If concatenating src filename exceeds 256 bytes */
        ul_errcode = TFTPC_DEST_PATH_LENGTH_ERROR;
        goto err_handler;
      }

      /* If not present concat / to the path */
      if (strncat_s((char *)psz_tempdestname, (TFTP_MAX_PATH_LENGTH - strlen((char *)psz_tempdestname)),
                    "/", TFTP_MAX_PATH_LENGTH - strlen((char *)psz_tempdestname) - 1) != EOK) {
        ul_errcode = TFTPC_ERROR_NOT_DEFINED;
        goto err_handler;
      }
    }

    /* Concatenate src filename to destination path */
    if (strncat_s((char *)psz_tempdestname, (TFTP_MAX_PATH_LENGTH - strlen((char *)psz_tempdestname)),
                  (char *)sz_tempsrcname, TFTP_MAX_PATH_LENGTH - strlen((char *)psz_tempdestname) - 1) != EOK) {
      ul_errcode = TFTPC_ERROR_NOT_DEFINED;
      goto err_handler;
    }
  }

  ul_errcode = lwip_tftp_create_bind_socket(&i_socknum);
  if (ul_errcode != ERR_OK) {
    goto err_handler;
  }

  if (us_tftpservport == 0) {
    us_tftpservport = TFTPC_SERVER_PORT;
  }

  us_tempservport = us_tftpservport;

  /* set server IP address */
  (void)memset_s(&st_serveraddr, sizeof(st_serveraddr), 0, sizeof(st_serveraddr));
  st_serveraddr.sin_family = AF_INET;
  st_serveraddr.sin_port = htons(us_tempservport);
  st_serveraddr.sin_addr.s_addr = htonl(ul_hostaddr);

  /* Make a request packet - TFTPC_OP_RRQ */
  ul_size = (u32_t)lwip_tftp_make_tftp_packet(TFTPC_OP_RRQ, sz_srcfilename,
                                              (u32_t)uc_tftptransmode,
                                              pst_sendbuf);
  ul_errcode = lwip_tftp_send_to_server(i_socknum, ul_size,
                                        pst_sendbuf, &st_serveraddr);
  if (ul_errcode != ERR_OK) {
    /* send to server failed */
    (void)lwip_close(i_socknum);
    goto err_handler;
  }

  for (;;) {
    if (ul_ignorepkt > 0) {
      ul_ignorepkt = 0;
    }

    ul_errcode = lwip_tftp_recv_from_server(i_socknum, &ul_recvsize, pst_recvbuf,
                                            &ul_ignorepkt, &st_serveraddr, pst_sendbuf);
    /* If select timeout occured */
    if (ul_errcode == TFTPC_TIMEOUT_ERROR) {
      ul_totaltime++;
      if (ul_totaltime < TFTPC_MAX_SEND_REQ_ATTEMPTS) {
        /* Max attempts not reached. Resend packet */
        ul_errcode = lwip_tftp_send_to_server(i_socknum, ul_size,
                                              pst_sendbuf, &st_serveraddr);
        if (ul_errcode != ERR_OK) {
          (void)lwip_close(i_socknum);
          if (is_localfileopened == true) {
            close(fp);
          }
          goto err_handler;
        }

        continue;
      } else {
        /* return from the function, max attempts limit reached */
        (void)lwip_close(i_socknum);
        if (is_localfileopened == true) {
          close(fp);
        }
        ul_errcode = TFTPC_TIMEOUT_ERROR;
        goto err_handler;
      }
    } else if (ul_errcode != ERR_OK) {
      (void)lwip_close(i_socknum);
      if (is_localfileopened == true) {
        close(fp);
      }
      goto err_handler;
    }

    /* Now we have receive block from different server. */
    if (ul_ignorepkt > 0) {
      /* Continue without processing this block. */
      continue;
    }

    /* if this packet is unkonwn or incorrect packet */
    if (ntohs(pst_recvbuf->us_opcode) != TFTPC_OP_DATA) {
      /* Send error packet to server */
      lwip_tftp_send_error(i_socknum,
                           TFTPC_PROTOCOL_PROTO_ERROR,
                           "Protocol error.",
                           &st_serveraddr, pst_sendbuf);

      (void)lwip_close(i_socknum);
      if (is_localfileopened == true) {
        close(fp);
      }

      LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_get_file_by_filename : Received pkt not DATA pkt\n"));

      ul_errcode = TFTPC_PROTO_ERROR;
      goto err_handler;
    }

    /* Now the number of tries will be reset. */
    ul_totaltime = 0;

    /* Validate received  DATA packet. */
    ul_errcode = lwip_tftp_validate_data_pkt(i_socknum, &ul_recvsize,
                                             pst_recvbuf, (u16_t)ul_currblk,
                                             &ul_resendpkt,
                                             &st_serveraddr);
    if (ul_errcode != ERR_OK) {
      /* Send Error packet to server */
      if (ul_errcode != TFTPC_RECVFROM_ERROR) {
        lwip_tftp_send_error(i_socknum,
                             TFTPC_PROTOCOL_PROTO_ERROR,
                             "Received unexpected packet",
                             &st_serveraddr, pst_sendbuf);
      }

      (void)lwip_close(i_socknum);
      if (is_localfileopened == true) {
        close(fp);
      }

      goto err_handler;
    }

    /* Received previous data block again. Resend last packet */
    if (ul_resendpkt > 0) {
      /* Now set ul_resendpkt to 0 to send the last packet. */
      ul_resendpkt = 0;
      ul_errcode = lwip_tftp_send_to_server(i_socknum, ul_size,
                                            pst_sendbuf, &st_serveraddr);
      if (ul_errcode != ERR_OK) {
        (void)lwip_close(i_socknum);
        if (is_localfileopened == true) {
          close(fp);
        }

        goto err_handler;
      }

      /* Continue in loop to send last packet again. */
      continue;
    }

    /* Get the size of the data block received */
    ul_recvsize -= TFTP_HDRSIZE;

    /* Check if the size of the received data block > max size */
    if (ul_recvsize > TFTP_BLKSIZE) {
      /* Send Error packet to server */
      lwip_tftp_send_error(i_socknum,
                           TFTPC_PROTOCOL_PROTO_ERROR,
                           "Packet size > max size",
                           &st_serveraddr, pst_sendbuf);

      (void)lwip_close(i_socknum);
      if (is_localfileopened == true) {
        close(fp);
      }

      LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_get_file_by_filename : Packet size > max size\n"));

      ul_errcode = TFTPC_PKT_SIZE_ERROR;
      goto err_handler;
    }

    us_readreq = (u16_t)TFTPC_OP_ACK;
    pst_sendbuf->us_opcode = htons(us_readreq);
    pst_sendbuf->u.us_blknum = htons((u16_t)ul_currblk);
    ul_size = TFTP_HDRSIZE;

    if (is_localfileopened == false) {
      fp = open((char *)psz_tempdestname, (O_WRONLY | O_CREAT | O_TRUNC), DEFFILEMODE);
      if (fp == TFTP_NULL_INT32) {
        ul_errcode = TFTPC_FILECREATE_ERROR;
        goto err_handler;
      }
      is_localfileopened = true;
    }

    if (ul_recvsize != TFTP_BLKSIZE) {
      (void)lwip_tftp_send_to_server(i_socknum, ul_size, pst_sendbuf, &st_serveraddr);

      if (ul_recvsize == 0) {
        /* Now free allocated resourdes and return, data block receiving is already completed */
        (void)lwip_close(i_socknum);
        close(fp);
        ul_errcode = ERR_OK;
        goto err_handler;
      }
      /* Write the last packet to the file */
      i_errcode = write(fp, (void *)pst_recvbuf->u.sttftp_data.uc_databuf, (size_t)ul_recvsize);
      if (ul_recvsize != (u32_t)i_errcode) {
        /* Write to file failed. */
        lwip_tftp_send_error(i_socknum,
                             TFTPC_PROTOCOL_USER_DEFINED,
                             "Write to file failed",
                             &st_serveraddr, pst_sendbuf);

        (void)lwip_close(i_socknum);
        close(fp);

        /* return from the function, file write failed */
        ul_errcode = TFTPC_FILEWRITE_ERROR;
        goto err_handler;
      }

      /* Now free allocated resourdes and return, data block receiving is already completed */
      (void)lwip_close(i_socknum);
      close(fp);
      ul_errcode = ERR_OK;
      goto err_handler;
    }

    ul_errcode = lwip_tftp_send_to_server(i_socknum, ul_size,
                                          pst_sendbuf, &st_serveraddr);
    if (ul_errcode != ERR_OK) {
      (void)lwip_close(i_socknum);
      close(fp);
      goto err_handler;
    }

    i_errcode = write(fp, (void *)pst_recvbuf->u.sttftp_data.uc_databuf, (size_t)ul_recvsize);
    if (ul_recvsize != (u32_t)i_errcode) {
      /* Write to file failed. */
      lwip_tftp_send_error(i_socknum,
                           TFTPC_PROTOCOL_USER_DEFINED,
                           "Write to file failed",
                           &st_serveraddr, pst_sendbuf);

      (void)lwip_close(i_socknum);
      close(fp);

      /* return from the function, file write failed */
      ul_errcode = TFTPC_FILEWRITE_ERROR;
      goto err_handler;
    }

    /* form the ACK packet for the DATA packet received */
    /* Go to the next packet no. */
    ul_currblk++;

    /* if the file is too big, exit */
    if (ul_currblk > TFTP_MAX_BLK_NUM) {
      /* Send error packet to server */
      lwip_tftp_send_error(i_socknum,
                           TFTPC_PROTOCOL_USER_DEFINED,
                           "File is too big.",
                           &st_serveraddr, pst_sendbuf);

      (void)lwip_close(i_socknum);
      close(fp);
      LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_get_file_by_filename : Data block number exceeded max value\n"));

      ul_errcode = TFTPC_FILE_TOO_BIG;
      goto err_handler;
    }
  }

err_handler:
  mem_free(pst_sendbuf);
  mem_free(pst_recvbuf);
  mem_free(psz_tempdestname);
  fp = -1;
  return ul_errcode;
}


/*
 * INTERFACE Function to put a file using filename
 * ul_hostaddr: IP address of Host
 * sz_srcfilename: Source file
 * sz_destdirpath: Destination file path
 */
u32_t
lwip_tftp_put_file_by_filename(u32_t ul_hostaddr, u16_t us_tftpservport, u8_t uc_tftptransmode,
                               s8_t *sz_srcfilename, s8_t *sz_destdirpath)
{
  u32_t ul_srcstrlen;
  u32_t ul_deststrlen;
  s32_t i_socknum = TFTP_NULL_INT32;
  s32_t i_errcode;
  u32_t ul_errcode;
  u16_t us_tempservport;
  tftpc_packet_s *pst_sendbuf = NULL;
  u16_t us_readreq;
  u32_t ul_size;
  s8_t *puc_buffer = 0;
  s8_t *sz_tempdestname = NULL;

  /* Initialize the block number */
  u16_t us_currblk = 0;
  struct sockaddr_in st_serveraddr;
  struct stat buffer;
  s32_t fp = -1;

  /* Validate parameters */
  if ((sz_srcfilename == NULL) || (sz_destdirpath == NULL)) {
    return TFTPC_INVALID_PARAVALUE;
  }

  if ((uc_tftptransmode != TRANSFER_MODE_BINARY) && (uc_tftptransmode != TRANSFER_MODE_ASCII)) {
    return TFTPC_INVALID_PARAVALUE;
  }

  /* check IP address not within ( 1.0.0.0 - 126.255.255.255 ) and ( 128.0.0.0 - 223.255.255.255 ) range. */
  if (!(((ul_hostaddr >= TFTPC_IP_ADDR_MIN) && (ul_hostaddr <= TFTPC_IP_ADDR_EX_RESV)) ||
      ((ul_hostaddr >= TFTPC_IP_ADDR_CLASS_B) && (ul_hostaddr <= TFTPC_IP_ADDR_EX_CLASS_DE)))) {
    return TFTPC_IP_NOT_WITHIN_RANGE;
  }

  /* If Src filename is empty or exceeded max length */
  ul_srcstrlen = strlen((char *)sz_srcfilename);
  if ((ul_srcstrlen == 0) || (ul_srcstrlen >= TFTP_MAX_PATH_LENGTH)) {
    return TFTPC_SRC_FILENAME_LENGTH_ERROR;
  }

  /* Check if source file exists */
  if (stat((char *)sz_srcfilename, &buffer) != 0) {
    return TFTPC_FILE_NOT_EXIST;
  }

  /* Check if the file is too big */
  if (buffer.st_size >= (off_t)(TFTP_MAX_BLK_NUM * TFTP_BLKSIZE)) {
    return TFTPC_FILE_TOO_BIG;
  }

  /* Check validity of destination path */
  ul_deststrlen = strlen((char *)sz_destdirpath);
  /* If dest path length exceeded max value */
  if (ul_deststrlen >= TFTP_MAX_PATH_LENGTH) {
    return TFTPC_DEST_PATH_LENGTH_ERROR;
  }

  pst_sendbuf = (tftpc_packet_s *)mem_malloc(sizeof(tftpc_packet_s));
  if (pst_sendbuf == NULL) {
    return TFTPC_MEMALLOC_ERROR;
  }

  /* First time intialize the buffer */
  (void)memset_s((void *)pst_sendbuf, sizeof(tftpc_packet_s), 0, sizeof(tftpc_packet_s));

  /*
   * The destination path can only be one of the following:
   * 1. Only filename
   * 2. Relative path WITH filename
   * 3. Empty string
   */
  if (ul_deststrlen != 0) {
    /* If not empty string use the Destination path name */
    sz_tempdestname = sz_destdirpath;
  } else {
    /*
     * If destination directory is empty string use source filename
     * If given src filename is a relative path extract the file name
     * from the path
     */
    if ((strchr((char *)sz_srcfilename, '/') != 0) ||
        (strchr((char *)sz_srcfilename, '\\') != 0)) {
      /* Move to the end of the src file path */
      sz_tempdestname = sz_srcfilename + (ul_srcstrlen - 1);

      while (((*(sz_tempdestname - 1) != '/') && (*(sz_tempdestname - 1) != '\\')) &&
             (sz_tempdestname != sz_srcfilename)) {
        sz_tempdestname--;
      }
    } else {
      /* If not a relative src path use the given src filename */
      sz_tempdestname = sz_srcfilename;
    }
  }

  /* Create a socket and bind it to an available port number */
  ul_errcode = lwip_tftp_create_bind_socket(&i_socknum);
  if (ul_errcode != ERR_OK) {
    /* Create and Bind socket failed */
    goto err_handler;
  }

  if (us_tftpservport == 0) {
    us_tftpservport = TFTPC_SERVER_PORT;
  }

  us_tempservport = us_tftpservport;

  /* set server internet address */
  (void)memset_s(&st_serveraddr, sizeof(st_serveraddr), 0, sizeof(st_serveraddr));
  st_serveraddr.sin_family = AF_INET;
  st_serveraddr.sin_port = htons(us_tempservport);
  st_serveraddr.sin_addr.s_addr = htonl(ul_hostaddr);

  /* Make request packet - TFTPC_OP_WRQ */
  ul_size = (u32_t)lwip_tftp_make_tftp_packet(TFTPC_OP_WRQ,
                                              sz_tempdestname,
                                              uc_tftptransmode,
                                              pst_sendbuf);

  ul_errcode = lwip_tftp_send_to_server(i_socknum, ul_size,
                                        pst_sendbuf, &st_serveraddr);
  if (ul_errcode != ERR_OK) {
    /* Send to server error */
    (void)lwip_close(i_socknum);

    goto err_handler;
  }

  /* Send the request packet */
  ul_errcode = lwip_tftp_inner_put_file(i_socknum, pst_sendbuf, ul_size,
                                        us_currblk, &st_serveraddr);
  if (ul_errcode != ERR_OK) {
    /* Send request packet failed */
    (void)lwip_close(i_socknum);

    LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_put_file_by_filename : Failed to send request packet\n"));

    goto err_handler;
  }

  /* Create buffer block size */
  puc_buffer = mem_malloc(TFTP_BLKSIZE);
  if (puc_buffer == NULL) {
    /* Memory allocation failed */
    lwip_tftp_send_error(i_socknum,
                         TFTPC_PROTOCOL_USER_DEFINED,
                         "Memory allocation failed.",
                         &st_serveraddr, pst_sendbuf);

    (void)lwip_close(i_socknum);
    ul_errcode = TFTPC_MEMALLOC_ERROR;
    goto err_handler;
  }

  (void)memset_s((void *)puc_buffer, TFTP_BLKSIZE, 0, TFTP_BLKSIZE);

  fp = open((char *)sz_srcfilename, O_RDONLY);
  if (fp == TFTP_NULL_INT32) {
    /* If file could not be opened send error to server */
    lwip_tftp_send_error(i_socknum,
                         TFTPC_PROTOCOL_USER_DEFINED,
                         "File open error.",
                         &st_serveraddr, pst_sendbuf);

    (void)lwip_close(i_socknum);
    close(fp);
    mem_free(puc_buffer);

    ul_errcode = TFTPC_FILEOPEN_ERROR;
    goto err_handler;
  }

  i_errcode = read(fp, puc_buffer, TFTP_BLKSIZE);
  if (i_errcode < 0) {
    /* If failed to read from file */
    lwip_tftp_send_error(i_socknum,
                         TFTPC_PROTOCOL_USER_DEFINED,
                         "File read error.",
                         &st_serveraddr, pst_sendbuf);

    (void)lwip_close(i_socknum);
    close(fp);
    mem_free(puc_buffer);

    ul_errcode = TFTPC_FILEREAD_ERROR;
    goto err_handler;
  }

  /*
   * Read from source file and send to server
   * To send empty packet to server when file is a 0 byte file
   */
  do {
    if (((u32_t)us_currblk + 1) > TFTP_MAX_BLK_NUM) {
      lwip_tftp_send_error(i_socknum,
                           TFTPC_PROTOCOL_USER_DEFINED,
                           "File is too big.",
                           &st_serveraddr, pst_sendbuf);

      (void)lwip_close(i_socknum);
      close(fp);
      mem_free(puc_buffer);
      LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_put_file_by_filename : Data block number exceeded max value\n"));

      ul_errcode = TFTPC_FILE_TOO_BIG;
      goto err_handler;
    }

    /* Increment block number */
    us_currblk++;

    ul_size = (u32_t)i_errcode + TFTP_HDRSIZE;

    /* Form the DATA packet */
    us_readreq = (u16_t)TFTPC_OP_DATA;
    pst_sendbuf->us_opcode = htons(us_readreq);
    pst_sendbuf->u.sttftp_data.us_blknum = htons(us_currblk);
    if (memcpy_s((void *)pst_sendbuf->u.sttftp_data.uc_databuf, TFTP_BLKSIZE,
                 (void *)puc_buffer, (u32_t)i_errcode) != EOK) {
      (void)lwip_close(i_socknum);
      close(fp);
      mem_free(puc_buffer);
      goto err_handler;
    }

    ul_errcode = lwip_tftp_send_to_server(i_socknum, ul_size,
                                          pst_sendbuf, &st_serveraddr);
    if ((ul_errcode != ERR_OK) || (memset_s((void *)puc_buffer, TFTP_BLKSIZE, 0, TFTP_BLKSIZE) != EOK)) {
      (void)lwip_close(i_socknum);
      close(fp);
      mem_free(puc_buffer);
      goto err_handler;
    }

    /* Read a block from the file to buffer */
    i_errcode = read(fp, puc_buffer, TFTP_BLKSIZE);
    if (i_errcode < 0) {
      /* If failed to read from file */
      lwip_tftp_send_error(i_socknum, TFTPC_PROTOCOL_USER_DEFINED, "File read error.",
                           &st_serveraddr, pst_sendbuf);

      (void)lwip_close(i_socknum);
      close(fp);
      mem_free(puc_buffer);
      ul_errcode = TFTPC_FILEREAD_ERROR;
      goto err_handler;
    }

    /* Send the request packet */
    ul_errcode = lwip_tftp_inner_put_file(i_socknum, pst_sendbuf, ul_size,
                                          us_currblk, &st_serveraddr);
    if (ul_errcode != ERR_OK) {
      /* Sending buffer contents failed */
      (void)lwip_close(i_socknum);
      close(fp);
      mem_free(puc_buffer);
      LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_put_file_by_filename : Sending file to server failed\n"));
      goto err_handler;
    }
  } while (ul_size == (TFTP_BLKSIZE + TFTP_HDRSIZE));

  /* Transfer of data is finished */
  (void)lwip_close(i_socknum);
  close(fp);
  mem_free(puc_buffer);

  ul_errcode = ERR_OK;
err_handler:
  mem_free(pst_sendbuf);
  fp = -1;
  return ul_errcode;
}

/*
 * Put file function
 * i_socknum: Socket ID
 * pst_sendbuf: Packet to send to server
 * ul_sendsize: Packet length
 * us_currblk: Current block number
 * pst_serveraddr: Server address
 */
u32_t
lwip_tftp_inner_put_file(s32_t i_socknum,
                         tftpc_packet_s *pst_sendbuf,
                         u32_t ul_sendsize,
                         u16_t us_currblk,
                         struct sockaddr_in *pst_serveraddr)
{
  u32_t ul_pktsize;
  u32_t ul_error;
  s32_t i_error;
  int i_recvlen = 0;
  socklen_t i_fromaddrlen;
  u32_t ul_totaltime = 0;
  fd_set st_readfds;
  struct sockaddr_in st_fromaddr;
  struct timeval st_timeout;
  tftpc_packet_s *pst_recvbuf = NULL;
  u32_t ul_ignorepkt = 0;
  u16_t us_blknum;
  u32_t ul_loopcnt = 0;
  if ((pst_sendbuf == NULL) || (pst_serveraddr == NULL)) {
    return ERR_ARG;
  }
  i_fromaddrlen = sizeof(st_fromaddr);

  pst_recvbuf = (tftpc_packet_s *)mem_malloc(sizeof(tftpc_packet_s));
  if (pst_recvbuf == NULL) {
    return TFTPC_MEMALLOC_ERROR;
  }

  /* First time intialize the buffer */
  (void)memset_s((void *)pst_recvbuf, sizeof(tftpc_packet_s), 0, sizeof(tftpc_packet_s));

  /* Initialize from address to the server address at first */
  if (memcpy_s((void *)&st_fromaddr, sizeof(struct sockaddr_in),
               (void *)pst_serveraddr, sizeof(st_fromaddr)) != EOK) {
    ul_error = TFTPC_MEMCPY_FAILURE;
    goto err_handler;
  }

  for (;;) {
    ul_error = lwip_tftp_recv_from_server(i_socknum, &ul_pktsize,
                                          pst_recvbuf, &ul_ignorepkt,
                                          pst_serveraddr, pst_sendbuf);
    /* If select timeout occured */
    if (ul_error == TFTPC_TIMEOUT_ERROR) {
      ul_totaltime++;
      if (ul_totaltime < TFTPC_MAX_SEND_REQ_ATTEMPTS) {
        /* Max attempts not reached. Resend packet */
        ul_error = lwip_tftp_send_to_server(i_socknum, ul_sendsize,
                                            pst_sendbuf, pst_serveraddr);
        if (ul_error != ERR_OK) {
          goto err_handler;
        }

        continue;
      } else {
        /* return from the function, max attempts limit reached */
        ul_error = TFTPC_TIMEOUT_ERROR;
        goto err_handler;
      }
    } else if (ul_error != ERR_OK) {
      /* return from the function, RecvFromServer failed */
      goto err_handler;
    }

    /* If Received packet from another server */
    if (ul_ignorepkt > 0) {
      /* The packet that is received is to be ignored. So continue without processing it. */
      ul_ignorepkt = 0;
      continue;
    }

    /* if this packet is unknown or incorrect packet */
    if (ntohs(pst_recvbuf->us_opcode) != TFTPC_OP_ACK) {
      lwip_tftp_send_error(i_socknum,
                           TFTPC_PROTOCOL_PROTO_ERROR,
                           "Protocol error.",
                           pst_serveraddr, pst_sendbuf);

      LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_inner_put_file : Received pkt not Ack pkt\n"));

      ul_error = TFTPC_PROTO_ERROR;
      goto err_handler;
    }

    ul_totaltime = 0;

    /* if the packet is acknowledge packet */
    us_blknum = ntohs(pst_recvbuf->u.us_blknum);
    i_recvlen = (int)ul_pktsize;

    /* If not correct block no. */
    if (us_blknum != us_currblk) {
      /* we are not in sync now */
      /* reset any collected packets. */
      st_timeout.tv_sec = 1;
      st_timeout.tv_usec = 0;

      FD_ZERO(&st_readfds);
      FD_SET(i_socknum, &st_readfds);

      /*
       * Need to take care of timeout scenario in Select call.
       * Since the socket used is blocking, if select timeout occurs,
       * the following recvfrom will block indefinitely.
       */
      i_error = lwip_select((s32_t)(i_socknum + 1), &st_readfds, 0, 0, &st_timeout);

       /* Loop to get the last data packet from the receive buffer */
      while ((i_error != -1) && (i_error != 0)) {
        ul_loopcnt++;

        /*
         * MAX file size in TFTP is 32 MB.
         * Reason for keeping 75 here , is ((75*512=38400bytes)/1024) =  37MB. So the recv/Snd
         * Loop can receive the complete MAX message from the network.
         */
        if (ul_loopcnt > TFTPC_MAX_WAIT_IN_LOOP) {
          LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_inner_put_file : unexpected packets are received repeatedly\n"));
          ul_error = TFTPC_PKT_SIZE_ERROR;
          goto err_handler;
        }

        FD_ZERO(&st_readfds);
        FD_SET(i_socknum, &st_readfds);
        i_recvlen = lwip_recvfrom(i_socknum,
                                  (s8_t *)pst_recvbuf,
                                  TFTP_PKTSIZE, 0,
                                  (struct sockaddr *)&st_fromaddr,
                                  &i_fromaddrlen);
        if (TFTP_NULL_INT32 == i_recvlen) {
          ul_error = TFTPC_RECVFROM_ERROR;
          goto err_handler;
        }

        st_timeout.tv_sec = 1;
        st_timeout.tv_usec = 0;
        i_error = lwip_select((s32_t)(i_socknum + 1),
                              &st_readfds, 0, 0, &st_timeout);
      }

      /*
       * If a new packet is not received then donot change the byte order
       * as it has already been done
       */
      /* If received packet size < minimum packet size */
      if (i_recvlen < TFTPC_FOUR) {
        lwip_tftp_send_error(i_socknum,
                             TFTPC_PROTOCOL_PROTO_ERROR,
                             "Packet size < min packet size",
                             pst_serveraddr, pst_sendbuf);

        LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_inner_put_file : Received pkt not Ack pkt\n"));

        ul_error = TFTPC_PKT_SIZE_ERROR;
        goto err_handler;
      }

      /* Check if the received packet is from correct server and from correct port */
      if ((st_fromaddr.sin_addr.s_addr != pst_serveraddr->sin_addr.s_addr) ||
          (pst_serveraddr->sin_port != st_fromaddr.sin_port)) {
        /* This ACK packet is invalid. Just ignore it. */
        LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_inner_put_file : Received pkt from unknown server\n"));
        continue;
      }

      /* if this packet is not ACK packet */
      if (TFTPC_OP_ACK != ntohs(pst_recvbuf->us_opcode)) {
        lwip_tftp_send_error(i_socknum,
                             TFTPC_PROTOCOL_PROTO_ERROR,
                             "Protocol error.",
                             pst_serveraddr, pst_sendbuf);

        LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_inner_put_file : Received pkt not Ack pkt\n"));

        ul_error = TFTPC_PROTO_ERROR;
        goto err_handler;
      }

      us_blknum = ntohs(pst_recvbuf->u.us_blknum);
      /*
       * In this case we have received a duplicate ACK for data block.
       * (ACK for this data block was aready received earlier)
       * In this case we have usRecvBlkNum == (usNextBlkNum - 1).
       * This could mean that:
       * (i) last data packet that was sent was not received at server side
       * (ii) Acknowledgement of peer side is delayed.
       *
       * In this case, this duplicate ACK will be ignored and return to the
       * state machine to initiate a receive of this data packet.
       */
      if ((us_currblk - 1) == us_blknum) {
        /* This ACK packet is invalid. Just ignore it. */
        continue;
      }
      /*
       * Now check the block number with current block
       * If it is not the previous block and the current block,
       * then it is an unexpected packet.
       */
      if (us_blknum != us_currblk) {
        lwip_tftp_send_error(i_socknum,
                             TFTPC_PROTOCOL_PROTO_ERROR,
                             "Received unexpected packet",
                             pst_serveraddr, pst_sendbuf);

        LWIP_DEBUGF(TFTP_DEBUG,
                    ("lwip_tftp_inner_put_file : Received DATA pkt no. %"S32_F"instead of pkt no. %"S32_F"\n",
                     us_blknum, us_currblk));

        ul_error = TFTPC_SYNC_FAILURE;
        goto err_handler;
      }
    }

    ul_error = ERR_OK;
    goto err_handler;
  }

err_handler:
  mem_free(pst_recvbuf);
  return ul_error;
}

/*
 * INTEFACE to get a file using filename
 * ul_hostaddr - IP address of Host
 * sz_srcfilename - Source file
 * sz_destmemaddr - The target memory address in the client
 *
 * Example :
 * ul_hostaddr = ntohl(inet_addr ("192.168.1.3"));
 * lwip_tftp_get_file_by_filename_to_rawmem(ul_hostaddr, "/ramfs/vs_server.bin", memaddr, &filelen);
 */
u32_t
lwip_tftp_get_file_by_filename_to_rawmem(u32_t ul_hostaddr,
                                         u16_t us_tftpservport,
                                         u8_t uc_tftptransmode,
                                         const s8_t *sz_srcfilename,
                                         s8_t *sz_destmemaddr,
                                         u32_t *ul_flelength)
{
  s32_t i_socknum = TFTP_NULL_INT32;
  u32_t ul_srcstrlen;
  u32_t ul_size;
  u32_t ul_recvsize = TFTP_NULL_UINT32;
  u32_t ul_errcode;
  u16_t us_readreq;
  u16_t us_tempservport;
  u32_t ul_currblk = 1;
  u32_t ul_resendpkt = 0; /* Resend the previous packet */
  u32_t ul_ignorepkt = 0; /* Ignore received packet */
  u32_t ul_totaltime = 0;

  tftpc_packet_s *pst_sendbuf = NULL;
  tftpc_packet_s *pst_recvbuf = NULL;
  struct sockaddr_in st_serveraddr;
  u32_t ul_memoffset = 0;

  /* Validate the parameters */
  if ((sz_srcfilename == NULL) || (sz_destmemaddr == NULL) || (*ul_flelength == 0)) {
    return TFTPC_INVALID_PARAVALUE;
  }

  if ((uc_tftptransmode != TRANSFER_MODE_BINARY) && (uc_tftptransmode != TRANSFER_MODE_ASCII)) {
    return TFTPC_INVALID_PARAVALUE;
  }

  /* check IP address not within ( 1.0.0.0 - 126.255.255.255 ) and ( 128.0.0.0 - 223.255.255.255 ) range. */
  if (!(((ul_hostaddr >= TFTPC_IP_ADDR_MIN) && (ul_hostaddr <= TFTPC_IP_ADDR_EX_RESV)) ||
      ((ul_hostaddr >= TFTPC_IP_ADDR_CLASS_B) && (ul_hostaddr <= TFTPC_IP_ADDR_EX_CLASS_DE)))) {
    return TFTPC_IP_NOT_WITHIN_RANGE;
  }

  /* Check validity of source filename */
  ul_srcstrlen = strlen((const char *)sz_srcfilename);
  if ((ul_srcstrlen == 0) || (ul_srcstrlen >= TFTP_MAX_PATH_LENGTH)) {
    return TFTPC_SRC_FILENAME_LENGTH_ERROR;
  }

  pst_sendbuf = (tftpc_packet_s *)mem_malloc(sizeof(tftpc_packet_s));
  if (pst_sendbuf == NULL) {
    return TFTPC_MEMALLOC_ERROR;
  }

  pst_recvbuf = (tftpc_packet_s *)mem_malloc(sizeof(tftpc_packet_s));
  if (pst_recvbuf == NULL) {
    mem_free(pst_sendbuf);
    return TFTPC_MEMALLOC_ERROR;
  }

  /* First time initialize the buffers */
  (void)memset_s((void *)pst_sendbuf, sizeof(tftpc_packet_s), 0, sizeof(tftpc_packet_s));
  (void)memset_s((void *)pst_recvbuf, sizeof(tftpc_packet_s), 0, sizeof(tftpc_packet_s));

  ul_errcode = lwip_tftp_create_bind_socket(&i_socknum);
  if (ul_errcode != ERR_OK) {
    goto err_handler;
  }

  if (us_tftpservport == 0) {
    us_tftpservport = TFTPC_SERVER_PORT;
  }

  us_tempservport = us_tftpservport;

  /* set server IP address */
  (void)memset_s(&st_serveraddr, sizeof(st_serveraddr), 0, sizeof(st_serveraddr));
  st_serveraddr.sin_family = AF_INET;
  st_serveraddr.sin_port = htons(us_tempservport);
  st_serveraddr.sin_addr.s_addr = htonl(ul_hostaddr);

  /* Make a request packet - TFTPC_OP_RRQ */
  ul_size = (u32_t)lwip_tftp_make_tftp_packet(TFTPC_OP_RRQ, sz_srcfilename, (u32_t)uc_tftptransmode, pst_sendbuf);
  ul_errcode = lwip_tftp_send_to_server(i_socknum, ul_size, pst_sendbuf, &st_serveraddr);
  if (ul_errcode != ERR_OK) {
    /* send to server failed */
    (void)lwip_close(i_socknum);
    goto err_handler;
  }

  for (;;) {
    if (ul_ignorepkt > 0) {
      ul_ignorepkt = 0;
    }

    ul_errcode = lwip_tftp_recv_from_server(i_socknum, &ul_recvsize, pst_recvbuf, &ul_ignorepkt,
                                            &st_serveraddr, pst_sendbuf);
    /* If select timeout occured */
    if (ul_errcode == TFTPC_TIMEOUT_ERROR) {
      ul_totaltime++;
      if (ul_totaltime < TFTPC_MAX_SEND_REQ_ATTEMPTS) {
        /* Max attempts not reached. Resend packet */
        ul_errcode = lwip_tftp_send_to_server(i_socknum, ul_size,
                                              pst_sendbuf, &st_serveraddr);
        if (ul_errcode != ERR_OK) {
          (void)lwip_close(i_socknum);
          goto err_handler;
        }

        continue;
      } else {
        /* return from the function, max attempts limit reached */
        (void)lwip_close(i_socknum);
        ul_errcode = TFTPC_TIMEOUT_ERROR;
        goto err_handler;
      }
    } else if (ul_errcode != ERR_OK) {
      (void)lwip_close(i_socknum);
      goto err_handler;
    }

    /* Now we have receive block from different server. */
    if (ul_ignorepkt > 0) {
      /* Continue without processing this block. */
      continue;
    }

    /* if this packet is unkonwn or incorrect packet */
    if (ntohs(pst_recvbuf->us_opcode) != TFTPC_OP_DATA) {
      /* Send error packet to server */
      lwip_tftp_send_error(i_socknum,
                           TFTPC_PROTOCOL_PROTO_ERROR,
                           "Protocol error.",
                           &st_serveraddr, pst_sendbuf);

      (void)lwip_close(i_socknum);

      LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_get_file_by_filename : Received pkt not DATA pkt\n"));

      ul_errcode = TFTPC_PROTO_ERROR;
      goto err_handler;
    }

    /* Now the number of tries will be reset. */
    ul_totaltime = 0;

    /* Validate received  DATA packet. */
    ul_errcode = lwip_tftp_validate_data_pkt(i_socknum, &ul_recvsize,
                                             pst_recvbuf, (u16_t)ul_currblk,
                                             &ul_resendpkt,
                                             &st_serveraddr);
    if (ul_errcode != ERR_OK) {
      /* Send Error packet to server */
      if (ul_errcode != TFTPC_RECVFROM_ERROR) {
        lwip_tftp_send_error(i_socknum,
                             TFTPC_PROTOCOL_PROTO_ERROR,
                             "Received unexpected packet",
                             &st_serveraddr, pst_sendbuf);
      }

      (void)lwip_close(i_socknum);

      goto err_handler;
    }

    /* Received previous data block again. Resend last packet */
    if (ul_resendpkt > 0) {
      /* Now set ul_resendpkt to 0 to send the last packet. */
      ul_resendpkt = 0;
      ul_errcode = lwip_tftp_send_to_server(i_socknum, ul_size,
                                            pst_sendbuf, &st_serveraddr);
      if (ul_errcode != ERR_OK) {
        (void)lwip_close(i_socknum);

        goto err_handler;
      }

      /* Continue in loop to send last packet again. */
      continue;
    }

    /* Get the size of the data block received */
    ul_recvsize -= TFTP_HDRSIZE;

    /* Check if the size of the received data block > max size */
    if (ul_recvsize > TFTP_BLKSIZE) {
      /* Send Error packet to server */
      lwip_tftp_send_error(i_socknum,
                           TFTPC_PROTOCOL_PROTO_ERROR,
                           "Packet size > max size",
                           &st_serveraddr, pst_sendbuf);

      (void)lwip_close(i_socknum);

      LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_get_file_by_filename : Packet size > max size\n"));

      ul_errcode = TFTPC_PKT_SIZE_ERROR;
      goto err_handler;
    }

    us_readreq = (u16_t)TFTPC_OP_ACK;
    pst_sendbuf->us_opcode = htons(us_readreq);
    pst_sendbuf->u.us_blknum = htons((u16_t)ul_currblk);
    ul_size = TFTP_HDRSIZE;

    if (ul_recvsize != TFTP_BLKSIZE) {
      (void)lwip_tftp_send_to_server(i_socknum, ul_size,
                                     pst_sendbuf, &st_serveraddr);

      /* If the received packet has only header and do not have payload, the return failure */
      if (ul_recvsize != 0) {
        /* memcopy filed */
        if (*ul_flelength < (ul_memoffset + ul_recvsize)) {
          ul_errcode = TFTPC_MEMCPY_FAILURE;
          (void)lwip_close(i_socknum);
          *ul_flelength = ul_memoffset;
          goto err_handler;
        }
        /* copy the last packet to the memory */
        if (memcpy_s(sz_destmemaddr + ul_memoffset, TFTP_MAX_BLK_NUM * TFTP_BLKSIZE,
                     (void *)pst_recvbuf->u.sttftp_data.uc_databuf, (size_t)ul_recvsize) != EOK) {
          ul_errcode = TFTPC_MEMCPY_FAILURE;
          (void)lwip_close(i_socknum);
          *ul_flelength = ul_memoffset;
          goto err_handler;
        }
        ul_memoffset += ul_recvsize;
      }

      /* Now free allocated resourdes and return, data block receiving is already completed */
      (void)lwip_close(i_socknum);
      ul_errcode = ERR_OK;
      *ul_flelength = ul_memoffset;
      goto err_handler;
    }

    ul_errcode = lwip_tftp_send_to_server(i_socknum, ul_size,
                                          pst_sendbuf, &st_serveraddr);
    if (ul_errcode != ERR_OK) {
      (void)lwip_close(i_socknum);
      goto err_handler;
    }

    /* memcopy filed */
    if (*ul_flelength < (ul_recvsize * ul_currblk)) {
      ul_errcode = TFTPC_MEMCPY_FAILURE;
      (void)lwip_close(i_socknum);
      *ul_flelength = ul_memoffset;
      goto err_handler;
    }
    if (memcpy_s(sz_destmemaddr + ul_memoffset, TFTP_MAX_BLK_NUM * TFTP_BLKSIZE,
                 (void *)pst_recvbuf->u.sttftp_data.uc_databuf, (size_t)ul_recvsize) != EOK) {
      ul_errcode = TFTPC_MEMCPY_FAILURE;
      (void)lwip_close(i_socknum);
      *ul_flelength = ul_memoffset;
      goto err_handler;
    }

    ul_memoffset += ul_recvsize;
    /* form the ACK packet for the DATA packet received */
    /* Go to the next packet no. */
    ul_currblk++;
    /* if the file is too big, exit */
    if (ul_currblk > TFTP_MAX_BLK_NUM) {
      /* Send error packet to server */
      lwip_tftp_send_error(i_socknum,
                           TFTPC_PROTOCOL_USER_DEFINED,
                           "File is too big.",
                           &st_serveraddr, pst_sendbuf);

      (void)lwip_close(i_socknum);

      LWIP_DEBUGF(TFTP_DEBUG, ("lwip_tftp_get_file_by_filename : Data block number exceeded max value\n"));

      ul_errcode = TFTPC_FILE_TOO_BIG;
      goto err_handler;
    }
  }

err_handler:
  mem_free(pst_sendbuf);
  mem_free(pst_recvbuf);
  return ul_errcode;
}
#endif

#endif /* LWIP_TFTP */
