/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides at common service api header for customer \n
 *
 * History: \n
 * 2022-09-02, Create file. \n
 */

#ifndef AT_H
#define AT_H

#include <stdint.h>
#include <stdbool.h>
#include "at_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_utils_at_custom Custom
 * @ingroup  middleware_utils_at
 * @{
 */

#define AT_RESPONSE_OK "OK\r\n"
#define AT_RESPONSE_ERROR "ERROR\r\n"
#define AT_RESPONSE_BUSY "BUSY\r\n"
#define AT_RESPONSE_ABORTING "ABORTING\r\n"
 
/* AT FLAG indicates the AT command feature. */
#define AT_FLAG_NONE                    0x0
#define AT_FLAG_ABORTABLE               0x10
#define AT_FLAG_NOT_BLOCK_URC           0x200

/**
 * @if Eng
 * @brief  Definition of AT error code.
 * @else
 * @brief  定义AT错误码。
 * @endif
 */
typedef enum {
    AT_RET_OK = 0,
    AT_RET_SYNTAX_ERROR,
    AT_RET_MALLOC_ERROR,
    AT_RET_MEM_API_ERROR,
    AT_RET_CHANNEL_PARA_ERROR,
    AT_RET_CHANNEL_NOT_INIT,
    AT_RET_CHANNEL_DATA_NULL,
    AT_RET_CMD_PARA_ERROR,
    AT_RET_CMD_FORMAT_ERROR,
    AT_RET_CMD_NO_MATCH,
    AT_RET_CMD_TYPE_ERROR,
    AT_RET_CMD_IN_PROGRESS_BLOCK,
    AT_RET_CMD_ATTR_NOT_ALLOW,
    AT_RET_PROC_CMD_FUNC_MISSING,
    AT_RET_PROC_READ_FUNC_MISSING,
    AT_RET_PROC_TEST_FUNC_MISSING,
    AT_RET_PROC_SET_FUNC_MISSING,
    AT_RET_PROC_WAIT_INTERACTIVITY,
    AT_RET_PROC_ABORT_CURRENT_COMMAND,
    AT_RET_PARSE_PARA_ERROR,
    AT_RET_PARSE_PARA_MISSING_ERROR,
    AT_RET_PROGRESS_BLOCK,
    AT_RET_TIMER_ERROR,
    AT_RET_ABORT_DELAY
} at_ret_t;

/**
 * @if Eng
 * @brief  Definition of AT command type.
 * @else
 * @brief  定义AT命令类型。
 * @endif
 */
typedef enum {
    AT_CMD_TYPE_CMD = 0x00,    /*!< AT execute command. "AT+TEST", for example. */
    AT_CMD_TYPE_SET,           /*!< AT set command. "AT+TEST=520", for example. */
    AT_CMD_TYPE_READ,          /*!< AT read command. "AT+TEST?", for example. */
    AT_CMD_TYPE_TEST,          /*!< AT test command. "AT+TEST=?", for example. */
#ifdef CONFIG_AT_SUPPORT_QUERY
    AT_CMD_TYPE_QUERY,         /*!< AT query command. "AT+TEST?=", for example. */
#endif
    AT_CMD_TYPE_ERROR
} at_cmd_type_t;

/**
 * @if Eng
 * @brief  Declare AT abort function type.
 * @param  [in]  arg AT command abort func input parameter.
 * @else
 * @brief  声明AT命令打断函数类型。
 * @param  [in]  arg AT命令打断函数入参。
 * @endif
 */
typedef at_ret_t(*at_abort_func_t)(void *arg);

/**
 * @if Eng
 * @brief  Declare AT interactivity function type.
 * @param  [in]  data AT command interactivity func input parameter. This parameter must be a string.
 * @param  [in]  len The length of at command interactivity func input parameter.
 * @else
 * @brief  声明AT命令交互函数类型。
 * @param  [in]  data AT命令交互函数上报参数。这个参数必须为字符串。
 * @param  [in]  len AT命令交互函数上报参数长度。
 * @endif
 */
typedef at_ret_t(*at_interactivity_func_t)(const char *data, uint32_t len);

/**
 * @if Eng
 * @brief  Declare AT executing function type.
 * @else
 * @brief  声明AT命令执行函数类型。
 * @endif
 */
typedef at_ret_t(*at_cmd_func_t)(void);

/**
 * @if Eng
 * @brief  Declare AT setting function type.
 * @param  [in]  arg AT command set func input parameter.
 * @else
 * @brief  声明AT命令设置函数类型。
 * @param  [in]  arg AT命令设置函数入参。
 * @endif
 */
typedef at_ret_t(*at_set_func_t)(const void *arg);

/**
 * @if Eng
 * @brief  Declare AT reading function type.
 * @else
 * @brief  声明AT命令读函数类型。
 * @endif
 */
typedef at_ret_t(*at_read_func_t)(void);

/**
 * @if Eng
 * @brief  Declare AT test function type.
 * @else
 * @brief  声明AT命令测试函数类型。
 * @endif
 */
typedef at_ret_t(*at_test_func_t)(void);

#ifdef CONFIG_AT_SUPPORT_QUERY
/**
 * @if Eng
 * @brief  Declare AT query function type.
 * @param  [in]  arg AT command query func input parameter.
 * @else
 * @brief  声明AT命令查询函数类型。
 * @param  [in]  arg AT命令查询函数入参。
 * @endif
 */
typedef at_ret_t(*at_query_func_t)(const void *arg);
#endif

/**
 * @if Eng
 * @brief  Range checking syntax definition of integer.
 * @else
 * @brief  定义基于取值范围的整型校验语法。
 * @endif
 */
typedef struct {
    int32_t min_val;
    int32_t max_val;
} at_token_int_range_t;

/**
 * @if Eng
 * @brief  White list checking syntax definition of integer.
 * @else
 * @brief  定义基于白名单的整型校验语法。
 * @endif
 */
typedef struct {
    uint32_t num;
    const int32_t *values;
} at_token_int_list_t;

/**
 * @if Eng
 * @brief  Length checking syntax definition of string.
 * @else
 * @brief  定义基于长度的字符串校验语法。
 * @endif
 */
typedef struct {
    uint32_t max_length;
} at_token_string_t;

/**
 * @if Eng
 * @brief  White list checking syntax definition of string.
 * @else
 * @brief  定义基于白名单的字符串校验语法。
 * @endif
 */
typedef struct {
    uint32_t num;
    const uint8_t * const *values;
} at_token_string_values_t;

/**
 * @if Eng
 * @brief  Range checking syntax definition of binary string.
 * @else
 * @brief  定义基于范围值的二进制字符串校验语法。
 * @endif
 */
typedef struct {
    uint32_t max_value;
} at_token_bit_string_range_t;

/**
 * @if Eng
 * @brief  White list checking syntax definition of binary string.
 * @else
 * @brief  定义基于白名单二进制字符串校验语法。
 * @endif
 */
typedef struct {
    uint32_t num;
    const uint32_t *values;
} at_token_bit_string_list_t;

/**
 * @if Eng
 * @brief  Length checking syntax definition of hexadecimal string.
 * @else
 * @brief  定义基于长度的十六进制字符串校验语法。
 * @endif
 */
typedef struct {
    /** @if Eng  Record offset of a Identifies identification length field which record \n
     *           the data length after hexadecimal string conversion. \n
     *           The identification length field can be preset in advance or \n
     *           automatically generated through the AT_SYNTAX_ATTR_ADD_LENGTH attribute. \n
     *  @else    记录标识长度字段的偏移，标识长度字段用来存储十六进制字符串转换后的数据长度。该字段 \n
     *           可以预设也可以由 AT_SYNTAX_ATTR_ADD_LENGTH 属性新增。 \n
     *  @endif */
    uint32_t length_field_offset;
    uint32_t max_length;
} at_token_hex_string_t;

/**
 * @if Eng
 * @brief  Definition of AT command parameter type.
 * @else
 * @brief  定义AT命令参数类型。
 * @endif
 */
typedef enum {
    AT_SYNTAX_TYPE_INT,
    AT_SYNTAX_TYPE_STRING,
    AT_SYNTAX_TYPE_BIT_STRING,
    AT_SYNTAX_TYPE_OCTET_STRING,
    AT_SYNTAX_TYPE_NUM
} at_syntax_type_t;

/**
 * @if Eng
 * @brief  Definition of AT command parameter verification attribute.
 * @else
 * @brief  定义AT命令参数校验属性。
 * @endif
 */
typedef enum {
    /** @if Eng  Identify that the parameter is invalid \n
     *  @else    标识该参数当前不支持。 \n
     *  @endif */
    AT_SYNTAX_ATTR_NOT_SUPPORTED    = 0x0001,
    /** @if Eng  Identify that the parameter is defaultable \n
     *  @else    标识该参数可缺省，为保证参数顺序，缺省时‘，’不可缺省。 \n
     *  @endif */
    AT_SYNTAX_ATTR_OPTIONAL         = 0x0002,
    /** @if Eng  The attribute identifies the verification method is minimum value verification \n
     *  @else    该属性标识校验方式为最小值校验。 \n
     *  @endif */
    AT_SYNTAX_ATTR_AT_MIN_VALUE     = 0x0004,
    /** @if Eng  The attribute identifies the verification method is maximum value verification \n
     *  @else    该属性标识校验方式为最大值校验。 \n
     *  @endif */
    AT_SYNTAX_ATTR_AT_MAX_VALUE     = 0x0008,
    /** @if Eng  The attribute identifies the verification method is white list \n
     *  @else    该属性标识校验方式为白名单校验。 \n
     *  @endif */
    AT_SYNTAX_ATTR_LIST_VALUE       = 0x0010,
    /** @if Eng  The attribute identifies the verification method is length verification \n
     *  @else    该属性标识校验方式为长度校验。 \n
     *  @endif */
    AT_SYNTAX_ATTR_MAX_LENGTH       = 0x0020,
    /** @if Eng  This attribute indicates that the length field of this parameter is added for this parameter \n
     *  @else    该属性标识为该参数新增此参数的长度字段。 \n
     *  @endif */
    AT_SYNTAX_ATTR_ADD_LENGTH       = 0x0040,
    /** @if Eng  This attribute is used for strings. Identification string parameter is of mixed case type \n
     *  @else    该参数用于字符串，标识字符串支持大小写混合。 \n
     *  @endif */
    AT_SYNTAX_ATTR_FIX_CASE         = 0x0080,
    /** @if Eng  Identifies that the parameter has a preset length field \n
     *  @else    该属性标识该参数已预设长度字段。 \n
     *  @endif */
    AT_SYNTAX_ATTR_LENGTH_FIELD     = 0x0100
} at_syntax_attribute_t;

/**
 * @if Eng
 * @brief  Define a parameter verification structure of AT command.
 * @else
 * @brief  定义AT命令某个参数校验结构。
 * @endif
 */
typedef struct {
    uint32_t type : 4;    /*!< Parameter type(at_syntax_type_t). */
    uint32_t last : 1;    /*!< Identify whether it is the last parameter. */
    uint32_t attribute : 12;    /*!< Parameter type(at_syntax_attribute_t). */
    uint32_t offset : 15;    /*!< Parameter offset of para blob. */
    union {
        at_token_int_range_t int_range;
        at_token_int_list_t  int_list;
        at_token_string_t string;
        at_token_string_values_t string_list;
        at_token_bit_string_range_t bit_string_range;
        at_token_bit_string_list_t bit_string_list;
        at_token_hex_string_t octet_string;
    } entry;
} at_para_parse_syntax_t;

/**
 * @if Eng
 * @brief  Definition of AT command entry.
 * @else
 * @brief  定义AT命令实体。
 * @endif
 */
typedef struct {
    const char *name;    /*!< The name cannot be duplicate. */
    const uint16_t cmd_id;    /*!< The cmd_id cannot be duplicate. */
    const uint16_t attribute;
    const at_para_parse_syntax_t *syntax;
    at_cmd_func_t cmd;
    at_set_func_t set;
    at_read_func_t read;
    at_test_func_t test;
#ifdef CONFIG_AT_SUPPORT_QUERY
    at_query_func_t query;
#endif
} at_cmd_entry_t;

/**
 * @if Eng
 * @brief  Register AT Command List.
 * @param  [in]  table The address of AT command list. see @ref at_cmd_entry_t
 * @param  [in]  len The length of AT command list.
 * @param  [in]  struct_max_size The maximum size of the setting function input parameters struct.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册AT命令列表。
 * @param  [in]  table AT命令列表起始地址。参考 @ref at_cmd_entry_t
 * @param  [in]  len AT命令列表长度。
 * @param  [in]  struct_max_size AT命令列表中设置函数输入参数结构体的最大大小。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_at_cmd_table_register(const at_cmd_entry_t *table, uint32_t len,
                                     uint32_t struct_max_size);

#ifdef CONFIG_AT_SUPPORT_ASYNCHRONOUS
/**
 * @if Eng
 * @brief  Register AT command abort func.
 * @param  [in]  func AT command abort func. see @ref at_abort_func_t
 * @param  [in]  arg AT command abort func input parameter.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册AT命令打断函数。
 * @param  [in]  func AT命令打断函数。参考 @ref at_abort_func_t
 * @param  [in]  arg AT命令打断函数入参。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_at_cmd_abort_register(at_abort_func_t func, void *arg);

/**
 * @if Eng
 * @brief  This interface is used to send AT command results for Asynchronous blocking AT command.
 * @param  [in]  err AT command execution result. If the result is successful, enter 0. Other values indicate failure.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t
 * @else
 * @brief  异步阻塞式AT命令结果发送接口。
 * @param  [in]  err 执行结果。成功则输入0，其他值表示失败。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_at_send_async_result(uint16_t err);

/**
 * @if Eng
 * @brief  Register AT interactivity func.
 * @param  [in]  func AT interactivity func. see @ref at_interactivity_func_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册AT交互命令处理函数。
 * @param  [in]  func AT命令交互命令处理函数。参考 @ref at_interactivity_func_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other 失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_at_interactivity_func_register(at_interactivity_func_t func);
#endif

/**
 * @if Eng
 * @brief  Output AT print information to the default channel.
 * @param  [in]  str AT print information.This parameter must have a string terminator.
 * @else
 * @brief  向默认通道输出AT打印信息。
 * @param  [in]  str AT打印信息。这个参数必须携带字符串结束符。
 * @endif
 */
void uapi_at_report(const char *str);

/**
 * @if Eng
 * @brief  Output AT print information to the specified channel.
 * @param  [in]  channel_id AT channel number. It is defined in the at_config.h file which is defined by product.
 * @param  [in]  str AT print information.This parameter must have a string terminator.
 * @else
 * @brief  向指定通道输出AT打印信息。
 * @param  [in]  channel_id AT通道号，在at_config.h文件中定义，at_config.h由产品定义。
 * @param  [in]  str AT打印信息。这个参数必须携带字符串结束符。
 * @endif
 */
void uapi_at_report_to_single_channel(at_channel_id_t channel_id, const char *str);

#ifdef CONFIG_AT_SUPPORT_NOTIFY_REPORT
/**
 * @if Eng
 * @brief  Sends URC(Unsolicited result code) information to a specified channel.
 * @param  [in]  channel_id Channel id. It is defined in the at_config.h file which is defined by product.
 * @param  [in]  msg Unsolicitedly report messages.
 * @param  [in]  msg_len Indicates the length of the unsolicited report message.
 * @else
 * @brief  向指定通道发送主动上报信息。
 * @param  [in]  channel_id AT通道号，在at_config.h文件中定义，at_config.h由产品定义。
 * @param  [in]  msg 主动上报消息。
 * @param  [in]  msg_len 主动上报消息长度。
 * @endif
 */
errcode_t uapi_at_urc_to_channel(at_channel_id_t channel_id, const char *msg, uint32_t msg_len);
#endif

/**
 * @}
 */
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
