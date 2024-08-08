/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: header file for diag api.
 */

#ifndef SOC_ZDIAG_H
#define SOC_ZDIAG_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "diag_config.h"
#include "common_def.h"

/**
 * @defgroup include_middleware_utils_dfx_zdiag_api Diag
 * @ingroup  include_middleware_utils
 * @{
 */

/**
 * @if Eng
 * @brief  Diag option.
 * @else
 * @brief  option选项，当前存放对端地址
 * @endif
 */
typedef struct {
    diag_addr peer_addr;     /*!< @if Eng peer addr.
                                   @else   地址 @endif */
    uint8_t pad[3];             /*!< @if Eng pad.
                                     @else   预留字段 @endif */
} diag_option_t;

/**
 * @if Eng
 * @brief  Init value of diag_option_t.
 * @else
 * @brief  diag_option的初始值
 * @endif
 */
#define DIAG_OPTION_INIT_VAL {0, {0, 0, 0}}

/**
 * @if Eng
 * @brief  Pointer to zdiag cmd function.
 * @else
 * @brief  diag命令行函数指针
 * @endif
 */
typedef errcode_t (*zdiag_cmd_f)(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);

/**
 * @if Eng
 * @brief  Structure for registration command.
 * @else
 * @brief  diag命令注册结构体
 * @endif
 */
typedef struct {
    uint16_t min_id;               /*!< @if Eng Minimum DIAG ID.
                                        @else   Diag最小命令ID @endif */
    uint16_t max_id;               /*!< @if Eng Maximum DIAG ID.
                                        @else   Diag最大命令ID @endif */
    zdiag_cmd_f fn_input_cmd;      /*!< @if Eng This Handler is used to process the HSO command.
                                        @else   Diag命令处理函数 @endif */
} zdiag_cmd_reg_obj_t;

/**
 * @if Eng
 * @brief  Structure for statistics obj.
 * @else
 * @brief  统计量注册结构体
 * @endif
 */
typedef struct {
    uint16_t id;                 /*!< @if Eng Statistics ID.
                                      @else   统计量ID @endif */
    uint16_t array_cnt;          /*!< @if Eng Number of statistic structures.
                                      @else   统计量数量 @endif */
    uint32_t stat_packet_size;   /*!< @if Eng Size of a single statistic structure (unit: byte).
                                      @else   每个统计量的大小 @endif */
    void *stat_packet;           /*!< @if Eng Pointer to the statistic structure.
                                      @else   指向统计量的指针 @endif */
} zdiag_sys_stat_obj_t;

/**
 * @if Eng
 * @brief  Structure for diag ack.
 * @else
 * @brief  diag应答结构体
 * @endif
 */
typedef struct {
    uint16_t sn;               /*!< @if Eng cmd sn.
                                    @else   命令sn @endif */
    uint8_t ctrl;              /*!< @if Eng ack mode.
                                    @else   应答模式 @endif */
    uint8_t pad;
    uint16_t cmd_id;           /*!< @if Eng cmd id.
                                    @else   应答id @endif */
    uint16_t param_size;       /*!< @if Eng param size.
                                    @else   应答内容大小 @endif */
    uint8_t *param;            /*!< @if Eng data.
                                    @else   应答内容 @endif */
} msp_diag_ack_param_t;

/**
 * @if Eng
 * @brief  Registers the command handling function.
 * @par Description: The DIAG subsystem supports a function to register diag command. You can run
 *                   uapi_zdiag_register_cmd to register new commands.
 * @param  [in]  cmd_tbl Command table, which must be declared as a constant array
 *                        and transmitted to this parameter.
 * @param  [in]  cmd_num Number of commands. The value cannot be 0.
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li soc_zdiag.h
 * @else
 * @brief  diag命令注册接口
 * @par 说明: DIAG模块提供了diag命令行注册的功能. 用户可以调用uapi_zdiag_register_cmd函数注册新的命令
 * @param  [in]  cmd_tbl diag命令注册表
 * @param  [in]  cmd_num 命令条数
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li soc_zdiag.h
 * @endif
 */
errcode_t uapi_zdiag_register_cmd(const zdiag_cmd_reg_obj_t *cmd_tbl, uint16_t cmd_num);

/**
 * @if Eng
 * @brief  Unregisters the command handling function.
 * @par Description: The DIAG subsystem supports a function to unregister diag command. You can run
 *                   uapi_zdiag_unregister_cmd to unregister new commands.
 * @param  [in]  cmd_tbl Command table, which must be declared as a constant array
 *                        and transmitted to this parameter.
 * @param  [in]  cmd_num Number of commands. The value cannot be 0.
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li soc_zdiag.h
 * @else
 * @brief  diag命令解注册接口
 * @par 说明: DIAG模块提供了diag命令行解注册的功能. 用户可以调用uapi_zdiag_unregister_cmd函数可将已注册得命令行表清除
 * @param  [in]  cmd_tbl diag命令注册表
 * @param  [in]  cmd_num 命令条数
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li soc_zdiag.h
 * @endif
 */
errcode_t uapi_zdiag_unregister_cmd(const zdiag_cmd_reg_obj_t *cmd_tbl, uint16_t cmd_num);

/**
 * @if Eng
 * @brief  Reports DIAG packets.
 * @par Description: Reports DIAG channel packets to the DIAG client.
 * @param  [in]  cmd_id DIAG data packet ID. For example, if the packet ID is the same as the command
 *                           request ID, use the cmd_id parameter in the command callback function diag_cmd_f_prv.
 * @param  [in]  option Use the option parameter notify the DIAG subsystem
 *                           whether the packet is a local packet or a remote packet.
 * @param  [in]  packet Buffer address of the data packet.
 * @param  [in]  packet_size Data packet size (unit: byte).
 * @param  [in]  sync Synchronous or asynchronous DIAG packet pushing. TRUE indicates that the
 *                           packets are pushed synchronously and the operation is blocked. FALSE indicates the packets
 *                           are pushed asynchronously (with the memory allocated, the packet is cashed by the OS queue
 *                           before being pushed), and the operation is not blocked.
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li soc_zdiag.h
 * @else
 * @brief  diag报文上报接口
 * @par 说明: 上报报文给DIAG客户端
 * @param  [in]  cmd_id 报文上报ID
 * @param  [in]  option option选项，用来识别报文是本地报文还是远端报文
 * @param  [in]  packet 数据包地址
 * @param  [in]  packet_size 数据包大小
 * @param  [in]  sync 上报方式，同步或者异步
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li soc_zdiag.h
 * @endif
 */
errcode_t uapi_zdiag_report_packet(uint16_t cmd_id, diag_option_t *option, const uint8_t *packet, uint16_t packet_size,
                                   bool sync);

/**
 * @if Eng
 * @brief  Reports multiple DIAG critical packets.
 * @par Description: Reports multiple DIAG channel critical packets to the DIAG client.
 * @param  [in]  cmd_id DIAG data packet ID. For example, if the packet ID is the same as the command
 *                           request ID, use the cmd_id parameter in the command callback function diag_cmd_f_prv.
 * @param  [in]  option Use the option parameter notify the DIAG subsystem
 *                           whether the packet is a local packet or a remote packet.
 * @param  [in]  packet the pointer to the buffer address of the data packet.
 * @param  [in]  packet_size Data packet size (unit: byte).
 * @param  [in]  pkt_cnt the number of packets
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li soc_zdiag.h
 * @else
 * @brief  diag上报多个关键报文
 * @par 说明: 上报多个关键报文给DIAG客户端
 * @param  [in]  cmd_id 报文上报ID
 * @param  [in]  option option选项，用来识别报文是本地报文还是远端报文
 * @param  [in]  packet 指向数据指针数组的指针
 * @param  [in]  packet_size 指向数据包大小数组的指针
 * @param  [in]  pkt_cnt 数据包个数
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li soc_zdiag.h
 * @endif
 */
errcode_t uapi_zdiag_report_packets_critical(uint16_t cmd_id, diag_option_t *option, uint8_t **packet,
                                             uint16_t *packet_size, uint8_t pkt_cnt);

/**
 * @if Eng
 * @brief  Reports multiple DIAG normal packets.
 * @par Description: Reports multiple DIAG channel normal packets to the DIAG client.
 * @param  [in]  cmd_id DIAG data packet ID. For example, if the packet ID is the same as the command
 *                           request ID, use the cmd_id parameter in the command callback function diag_cmd_f_prv.
 * @param  [in]  option Use the option parameter notify the DIAG subsystem
 *                           whether the packet is a local packet or a remote packet.
 * @param  [in]  packet the pointer to the buffer address of the data packet.
 * @param  [in]  packet_size Data packet size (unit: byte).
 * @param  [in]  pkt_cnt the number of packets
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li soc_zdiag.h
 * @else
 * @brief  diag上报多个普通报文
 * @par 说明: 上报多个普通报文给DIAG客户端
 * @param  [in]  cmd_id 报文上报ID
 * @param  [in]  option option选项，用来识别报文是本地报文还是远端报文
 * @param  [in]  packet 指向数据指针数组的指针
 * @param  [in]  packet_size 指向数据包大小数组的指针
 * @param  [in]  pkt_cnt 数据包个数
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li soc_zdiag.h
 * @endif
 */
errcode_t uapi_zdiag_report_packets_normal(uint16_t cmd_id, diag_option_t *option, uint8_t **packet,
                                           uint16_t *packet_size, uint8_t pkt_cnt);

/**
 * @if Eng
 * @brief  Reports message to the DIAG client
 * @par Description: Reports message to the DIAG client
 * @param  [in]  module_id The source module id of the print log.
 * @param  [in]  msg_id The message id of the print log.
 * @param  [in]  buf The print buffer.
 * @param  [in]  buf_size The buffer size.
 * @param  [in]  level The level of the print log.
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li soc_zdiag.h
 * @else
 * @brief  消息上报接口
 * @par 说明: 上报消息给DIAG客户端
 * @param  [in]  module_id 模块id
 * @param  [in]  msg_id 消息id
 * @param  [in]  buf 上报内容
 * @param  [in]  buf_size 内容大小
 * @param  [in]  level 日志级别
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li soc_zdiag.h
 * @endif
 */
errcode_t uapi_diag_report_sys_msg(uint32_t module_id, uint32_t msg_id, const uint8_t *buf,
                                   uint16_t buf_size, uint8_t level);

/**
 * @if Eng
 * @brief  Registers the ind handling function.
 * @par Description: The DIAG subsystem supports a function to register ind.
 * @param  [in]  cmd_tbl Command table, which must be declared as a constant array
 *                       and transmitted to this parameter.
 * @param  [in]  cmd_num Number of commands.
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li soc_zdiag.h
 * @else
 * @brief  应答注册接口
 * @par 说明: diag模块提供应答注册函数
 * @param  [in]  cmd_tbl 注册应答表
 * @param  [in]  cmd_num 应答个数
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li soc_zdiag.h
 * @endif
 */
errcode_t uapi_zdiag_register_ind(const zdiag_cmd_reg_obj_t *cmd_tbl, uint16_t cmd_num);

/**
 * @if Eng
 * @brief  Run cmd process func with cmd ID.
 * @par Description: Run cmd process func with cmd ID.
 * @param  [in]  cmd_id DIAG command request ID.
 * @param  [in]  data Buffer address of the data packet.
 * @param  [in]  data_size Data packet size (unit: byte).
 * @param  [in]  option Use the option parameter notify the DIAG subsystem
 *                           whether the packet is a local packet or a remote packet.
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li soc_zdiag.h
 * @else
 * @brief  命令执行接口
 * @par 说明: 命令执行接口
 * @param  [in]  cmd_id 命令ID
 * @param  [in]  data 数据内容
 * @param  [in]  data_size 数据大小
 * @param  [in]  option option选项，用来识别报文是本地报文还是远端报文
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li soc_zdiag.h
 * @endif
 */
errcode_t uapi_diag_run_cmd(uint16_t cmd_id, uint8_t *data, uint16_t data_size, diag_option_t *option);

/**
 * @if Eng
 * @brief  Reports ack packets.
 * @par Description: Reports DIAG ack packets to the DIAG client.
 * @param  [in]  ack ack param.
 * @param  [in]  option Use the option parameter notify the DIAG subsystem
 *                        whether the packet is a local packet or a remote packet.
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li soc_zdiag.h
 * @else
 * @brief  ack数据包上报接口
 * @par 说明: 上报ack数据包给DIAG客户端
 * @param  [in]  ack ack内容
 * @param  [in]  option option选项，用来识别报文是本地报文还是远端报文
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li soc_zdiag.h
 * @endif
 */
errcode_t uapi_zdiag_report_ack(msp_diag_ack_param_t *ack, diag_option_t *option);

/**
 * @if Eng
 * @brief  Registers statistic objects handling function.
 * @par Description: The DIAG subsystem supports a function to register statistic objects. You can run
 *         uapi_zdiag_register_stat_obj to register a new stat obj.
 * @param  [in]  stat_obj_tbl Pointer to stat obj table, which must be declared
 *                       as a constant array and transmitted to this parameter.
 * @param  [in]  obj_num Number of objects.
 * @retval ERRCODE_SUCC            Success
 * @retval Others                  ERRCODE_FAIL or other error num.
 * @par Depends:
 * @li soc_zdiag.h
 * @else
 * @brief  统计量注册接口
 * @par 说明: DIAG模块提供了统计量注册函数, 用户可以调用uapi_zdiag_register_stat_obj接口注册新的统计量
 * @param  [in]  stat_obj_tbl 统计量注册表
 * @param  [in]  obj_num 统计量个数
 * @retval ERRCODE_SUCC           成功返回#ERRCODE_SUCC
 * @retval Others                 失败返回#ERRCODE_FAIL或其他返回值
 * @par 依赖:
 * @li soc_zdiag.h
 * @endif
 */
errcode_t uapi_zdiag_register_stat_obj(const zdiag_sys_stat_obj_t *stat_obj_tbl, uint16_t obj_num);

/**
 * @}
 */

#endif /* SOC_ZDIAG_H */
