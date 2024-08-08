/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides AT parse source \n
 */

#include "at_base.h"
#include "at_parse.h"

#define AT_PARSE_STRING_END_FLAG_LEN 1
#define AT_PARSE_STRING_FORMAT_FLAG_LEN 1
#define AT_PARSE_UINT32_STRING_LENGTH 10
#define AT_PARSE_UINT32_BASE 0
#define AT_PARSE_S32_MAX (2147483647)
#define AT_PARSE_S32_MIN (-2147483648)
#define AT_PARSE_OCTET_BYTES 2
#define AT_PARSE_CH_NUMBER_PER_BYTE 2
#define AT_PARSE_BIT_STRING_MAX_LEN 32
#define AT_PARSE_OFFSET_1 1
#define AT_PARSE_OFFSET_2 2
#define AT_PARSE_OFFSET_4 4
#define AT_PARSE_NUM_OF_HEX_A 10

typedef struct {
    const char        *str;
    uint32_t          str_len;
} at_parse_param_t;

typedef struct {
    uint16_t            param_count;
    uint16_t            param_max_num;
    at_parse_param_t    *params_array;
} at_parse_param_table_t;

typedef at_ret_t(*at_parse_assign_fun_t)(const at_parse_param_t*, const void*, const at_para_parse_syntax_t*);
typedef at_ret_t(*at_parse_verify_fun_t)(const void*, const at_para_parse_syntax_t*);

typedef struct {
    at_parse_assign_fun_t    assign_func;
    at_parse_verify_fun_t    verify_func;
} at_parse_args_t;

at_cmd_info_t *g_at_remain_list = NULL;

bool at_parse_has_remain_cmd(void)
{
    return (g_at_remain_list != NULL);
}

at_cmd_info_t* at_parse_get_next_remain_cmd(void)
{
    if (g_at_remain_list == NULL) {
        return NULL;
    }

    at_cmd_info_t *cmd = g_at_remain_list;
    g_at_remain_list = g_at_remain_list->next;
    return cmd;
}

at_format_t at_parse_format_of_cmd(const at_cmd_info_t *cmd_info, uint16_t *offset)
{
    switch (cmd_info->cmd_str[*offset]) {
        case '^':
        case '+':
            *offset = *offset + AT_PARSE_STRING_FORMAT_FLAG_LEN;
            return AT_FORMAT_ADD;
        default:
            return AT_FORMAT_ERROR;
    }
}

void at_parse_del_one_remain_cmd(at_cmd_info_t *cmd_info)
{
    at_cmd_info_t *wait_free = cmd_info;
    if (wait_free == NULL) {
        return;
    }

    if (wait_free->cmd_str != NULL) {
        at_free(wait_free->cmd_str);
        wait_free->cmd_str = NULL;
    }

    at_free(wait_free);
    wait_free = NULL;
}

static void at_parse_del_all_remain_cmd(void)
{
    at_cmd_info_t *cmd;

    cmd = at_parse_get_next_remain_cmd();
    while (cmd != NULL) {
        at_parse_del_one_remain_cmd(cmd);
        cmd = at_parse_get_next_remain_cmd();
    }
}

static at_ret_t at_parse_add_remain_list(const char *data, uint32_t start, uint32_t len, uint16_t channel_id)
{
    at_cmd_info_t *tail = g_at_remain_list;

    if (len == 0) {
        return AT_RET_OK;
    }
    at_cmd_info_t *new_node = at_malloc(sizeof(at_cmd_info_t));
    if (new_node == NULL) {
        return AT_RET_MALLOC_ERROR;
    }

    new_node->cmd_str = at_malloc(len + AT_PARSE_STRING_END_FLAG_LEN);
    if (new_node->cmd_str == NULL) {
        at_free(new_node);
        return AT_RET_MALLOC_ERROR;
    }

    new_node->channel_id = channel_id;
    new_node->str_len = (uint16_t)(len + AT_PARSE_STRING_END_FLAG_LEN);
    new_node->next = NULL;
    memset_s(new_node->cmd_str, new_node->str_len, 0, new_node->str_len);
    memcpy_s(new_node->cmd_str, len, data + start, len);

    if (tail == NULL) {
        g_at_remain_list = new_node;
        return AT_RET_OK;
    }

    while (tail->next != NULL) {
        tail = tail->next;
    }
    tail->next = new_node;

    return AT_RET_OK;
}

static at_parse_state_t at_parse_split_combine_cmd_machines(at_parse_state_t state, char ch,
                                                            uint16_t *index, char *out_string)
{
    switch (state) {
        case AT_NORMAL_STATE:
            if (ch == '\\') {
                return AT_ESCAPE_STATE;
            }

            if (ch == ';' || ch == '\0') {
                return AT_END_STATE;
            }

            out_string[(*index)++] = ch;
            break;

        case AT_ESCAPE_STATE:
            if (ch == '\0') {
                return AT_END_STATE;
            }
            if (ch != ';') {
                out_string[(*index)++] = '\\';
            }
            out_string[(*index)++] = ch;

        default:
            break;
    }
    return AT_NORMAL_STATE;
}

at_ret_t at_parse_split_combine_cmd(const char *data, uint32_t len, uint16_t channel_id)
{
    at_ret_t ret;
    uint32_t cmd_start = 0;
    uint32_t cmd_end;
    uint16_t index = 0;
    at_parse_state_t state = AT_NORMAL_STATE;
    char *cmd_string = (char*)at_malloc(len + AT_PARSE_STRING_END_FLAG_LEN);
    if (cmd_string == NULL) {
        return AT_RET_MALLOC_ERROR;
    }
    memset_s(cmd_string, len + AT_PARSE_STRING_END_FLAG_LEN, 0, len + AT_PARSE_STRING_END_FLAG_LEN);

    for (cmd_end = cmd_start; cmd_end <= len; cmd_end++) {
        state = at_parse_split_combine_cmd_machines(state, data[cmd_end], &index, cmd_string);
        if (state != AT_END_STATE) {
            continue;
        }

        ret = at_parse_add_remain_list(cmd_string, 0, index, channel_id);
        if (ret != AT_RET_OK) {
            at_parse_del_all_remain_cmd();
            at_free(cmd_string);
            return ret;
        }
        memset_s(cmd_string, len + AT_PARSE_STRING_END_FLAG_LEN, 0, len + AT_PARSE_STRING_END_FLAG_LEN);
        state = AT_NORMAL_STATE;
        index = 0;
        cmd_start = cmd_end + 1;
    }

    at_free(cmd_string);
    return AT_RET_OK;
}

at_cmd_type_t at_parse_cmd_type(const char *str, uint16_t *offset)
{
    const char *type_start;
    type_start = str + *offset;
    if ((*type_start == '?') && (*(type_start + AT_PARSE_OFFSET_1) == '\0')) {
        *offset = *offset + AT_PARSE_OFFSET_1;
        return AT_CMD_TYPE_READ;
#ifdef CONFIG_AT_SUPPORT_QUERY
    } else if ((*type_start == '?') && (*(type_start + AT_PARSE_OFFSET_1) == '=')) {
        *offset = *offset + AT_PARSE_OFFSET_2;
        return AT_CMD_TYPE_QUERY;
#endif
    } else if ((*type_start == '=') &&
               (*(type_start + AT_PARSE_OFFSET_1) == '?') &&
               (*(type_start + AT_PARSE_OFFSET_2) == '\0')) {
        *offset = *offset + AT_PARSE_OFFSET_2;
        return AT_CMD_TYPE_TEST;
    } else if (*type_start == '=') {
        *offset = *offset + AT_PARSE_OFFSET_1;
        return AT_CMD_TYPE_SET;
    } else if (*type_start == '\0') {
        *offset = *offset + AT_PARSE_OFFSET_1;
        return AT_CMD_TYPE_CMD;
    }
    return AT_CMD_TYPE_ERROR;
}

static void at_parse_update_min_index(const at_para_parse_syntax_t *syntax, uint16_t *min_index)
{
    if ((syntax->attribute & AT_SYNTAX_ATTR_OPTIONAL) == 0) {
        (*min_index)++;
    }
}

static void at_parse_get_expect_para_number(const at_para_parse_syntax_t *syntax,
                                            uint16_t *para_min_num, uint16_t *para_max_num)
{
    while (syntax[*para_max_num].last != true) {
        (void)at_parse_update_min_index(&syntax[*para_max_num], para_min_num);
        (*para_max_num)++;
    }

    (void)at_parse_update_min_index(&syntax[*para_max_num], para_min_num);
    (*para_max_num)++;
}

static at_parse_state_t at_parse_split_params_machines(at_parse_state_t state, char ch)
{
    switch (state) {
        case AT_NORMAL_STATE:
            if (ch == '\\') {
                return AT_ESCAPE_STATE;
            }

            if (ch == ',') {
                return AT_END_STATE;
            }
            break;

        case AT_ESCAPE_STATE:
        default:
            break;
    }
    return AT_NORMAL_STATE;
}

static uint16_t at_parse_get_actual_para_number(const char *str)
{
    uint16_t num = 0;
    uint16_t index = 0;
    at_parse_state_t state = AT_NORMAL_STATE;

    while (str[index] != '\0') {
        state = at_parse_split_params_machines(state, str[index]);
        if (state == AT_END_STATE) {
            num++;
            state = AT_NORMAL_STATE;
        }
        index++;
    }

    return ++num;
}

static at_ret_t at_parse_check_para_num(const char *str, const at_para_parse_syntax_t *syntax,
                                        at_parse_param_table_t *str_table)
{
    uint16_t para_min_num = 0;
    uint16_t para_max_num = 0;
    (void)at_parse_get_expect_para_number(syntax, &para_min_num, &para_max_num);
    uint16_t para_actual_num = at_parse_get_actual_para_number(str);
    if ((para_actual_num < para_min_num) || (para_actual_num > para_max_num)) {
        return AT_RET_PARSE_PARA_ERROR;
    }

    str_table->param_count = para_actual_num;
    str_table->param_max_num = para_max_num;
    return AT_RET_OK;
}

static at_parse_state_t at_parse_create_params_machines(at_parse_state_t state, char ch)
{
    switch (state) {
        case AT_NORMAL_STATE:
            if (ch == '\\') {
                return AT_ESCAPE_STATE;
            }

            if (ch == ',' || ch == '\0') {
                return AT_END_STATE;
            }
            break;

        case AT_ESCAPE_STATE:
            if (ch == '\0') {
                return AT_END_STATE;
            }
        default:
            break;
    }
    return AT_NORMAL_STATE;
}

static at_ret_t at_parse_para_array_init(at_parse_param_table_t *str_table)
{
    uint16_t paras_array_size = str_table->param_max_num * sizeof(at_parse_param_t);
    str_table->params_array = at_malloc(paras_array_size);
    if (str_table->params_array == NULL) {
        return AT_RET_MALLOC_ERROR;
    }
    memset_s(str_table->params_array, paras_array_size, 0, paras_array_size);
    return AT_RET_OK;
}

static at_ret_t at_parse_create_para_array(const char *str, at_parse_param_table_t *str_table)
{
    at_ret_t ret = AT_RET_OK;
    const char *ch = str;
    ret = at_parse_para_array_init(str_table);
    if (ret != AT_RET_OK) {
        return ret;
    }

    for (uint16_t count = 0; count < str_table->param_count; count++, ch++) {
        const char *para_start = ch;
        uint16_t para_len = 0;
        at_parse_state_t state = AT_NORMAL_STATE;

        while (state != AT_END_STATE) {
            state = at_parse_create_params_machines(state, *ch);
            if (state != AT_END_STATE) {
                ch++;
                para_len++;
            }
        }

        if (para_len == 0) {
            str_table->params_array[count].str = NULL;
        } else {
            str_table->params_array[count].str = para_start;
        }
        str_table->params_array[count].str_len = para_len;
    }

    return ret;
}

static at_ret_t at_parse_argument_assign_int(const at_parse_param_t *para_array, const void *args,
                                             const at_para_parse_syntax_t *syntax)
{
    int64_t value;
    char *s = NULL;
    int32_t *val = (int32_t*)((uintptr_t)args + syntax->offset);

    if (para_array->str_len > AT_PARSE_UINT32_STRING_LENGTH) {
        return AT_RET_PARSE_PARA_ERROR;
    }

    /* AT_PARSE_UINT32_BASE is 0. This means that this function can convert decimal, octal, and hexadecimal. */
    value = (int64_t)strtoull(para_array->str, &s, AT_PARSE_UINT32_BASE);
    if (para_array->str_len != (uint32_t)(s - para_array->str)) {
        return AT_RET_PARSE_PARA_ERROR;
    }

    if (value > AT_PARSE_S32_MAX || value < AT_PARSE_S32_MIN) {
        return AT_RET_PARSE_PARA_ERROR;
    }

    *val = (int32_t)value;
    return AT_RET_OK;
}

static at_ret_t at_parse_verify_argument_int(const void *args, const at_para_parse_syntax_t *syntax)
{
    int32_t val = *(int32_t*)((uintptr_t)args + syntax->offset);

    if ((syntax->attribute & AT_SYNTAX_ATTR_LIST_VALUE) != 0) {
        for (uint32_t i = 0; i < syntax->entry.int_list.num; i++) {
            if (val == syntax->entry.int_list.values[i]) {
                return AT_RET_OK;
            }
        }
        return AT_RET_PARSE_PARA_ERROR;
    }

    if ((syntax->attribute & AT_SYNTAX_ATTR_AT_MIN_VALUE) != 0) {
        if (val < syntax->entry.int_range.min_val) {
            return AT_RET_PARSE_PARA_ERROR;
        }
    }

    if ((syntax->attribute & AT_SYNTAX_ATTR_AT_MAX_VALUE) != 0) {
        if (val > syntax->entry.int_range.max_val) {
            return AT_RET_PARSE_PARA_ERROR;
        }
    }

    return AT_RET_OK;
}

static void at_parse_argument_prepare_string(char *str, uint32_t index_max, const at_parse_param_t *para_array)
{
    bool escape = false;
    for (uint16_t len = 0, index = 0; (len < para_array->str_len) && (index < index_max); len++) {
        if (escape == true) {
            str[index++] = para_array->str[len];
            escape = false;
            continue;
        }
        if (para_array->str[len] == '\\') {
            escape = true;
            continue;
        }
        str[index++] = para_array->str[len];
    }
}

static at_ret_t at_parse_argument_assign_string(const at_parse_param_t *para_array, const void *args,
                                                const at_para_parse_syntax_t *syntax)
{
    char **val = (char**)((uintptr_t)args + syntax->offset);

    *val = (char*)at_malloc(para_array->str_len + 1);
    if (*val == NULL) {
        return AT_RET_MALLOC_ERROR;
    }
    memset_s(*val, para_array->str_len + 1, 0, para_array->str_len + 1);

    (void)at_parse_argument_prepare_string(*val, para_array->str_len + 1, para_array);

    if ((syntax->attribute & AT_SYNTAX_ATTR_FIX_CASE) == 0) {
        at_base_toupper(*val, para_array->str_len);
    }

    return AT_RET_OK;
}

static at_ret_t at_parse_verify_argument_string(const void *args, const at_para_parse_syntax_t *syntax)
{
    const char **val = (const char**)((uintptr_t)args + syntax->offset);
    if ((syntax->attribute & AT_SYNTAX_ATTR_LIST_VALUE) != 0) {
        for (uint32_t i = 0; i < syntax->entry.string_list.num; i++) {
            /* It's guaranteed that both sides have a terminator('\0'). */
            if (strcmp((const char*)syntax->entry.string_list.values[i], *val) == 0) {
                return AT_RET_OK;
            }
        }
        return AT_RET_PARSE_PARA_ERROR;
    }

    if ((syntax->attribute & AT_SYNTAX_ATTR_MAX_LENGTH) != 0) {
        if (strlen(*val) > syntax->entry.string.max_length) {
            return AT_RET_PARSE_PARA_ERROR;
        }
    }

    return AT_RET_OK;
}

static at_ret_t at_parse_char_to_hex(char ch, uint8_t *val)
{
    if ((ch >= '0') && (ch <= '9')) {
        *val = (uint8_t)ch - (uint8_t)'0';    /* Forcible type conversion is used to eliminate alarms(G.TYP.03). */
    } else if ((ch >= 'a') && (ch <= 'f')) {
        *val = (uint8_t)ch - (uint8_t)'a' + AT_PARSE_NUM_OF_HEX_A;
    } else if ((ch >= 'A') && (ch <= 'F')) {
        *val = (uint8_t)ch - (uint8_t)'A' + AT_PARSE_NUM_OF_HEX_A;
    } else {
        return AT_RET_PARSE_PARA_ERROR;
    }
    return AT_RET_OK;
}

static at_ret_t at_parse_hex_string_to_uint8(const char* src, uint32_t src_len, uint8_t *dst)
{
    at_ret_t ret = AT_RET_OK;
    uint32_t index = 0;
    uint8_t hex;

    while (index < src_len) {
        ret = at_parse_char_to_hex(*(src + index), &hex);
        if (ret != AT_RET_OK) {
            return ret;
        }
        /* index >> AT_PARSE_OFFSET_1 is equivalent to index / 2 */
        dst[index >> AT_PARSE_OFFSET_1] = (uint8_t)(hex << AT_PARSE_OFFSET_4);
        index++;

        ret = at_parse_char_to_hex(*(src + index), &hex);
        if (ret != AT_RET_OK) {
            return ret;
        }
        dst[index >> AT_PARSE_OFFSET_1] |= hex;
        index++;
    }
    return ret;
}

static at_ret_t at_parse_argument_assign_octet_string(const at_parse_param_t *para_array, const void *args,
                                                      const at_para_parse_syntax_t *syntax)
{
    uint8_t **val = (uint8_t**)((uintptr_t)args + syntax->offset);
    uint32_t *len = (uint32_t*)((uintptr_t)args + syntax->entry.octet_string.length_field_offset);
    uint32_t str_len = para_array->str_len;

    if ((syntax->attribute & AT_SYNTAX_ATTR_LENGTH_FIELD) != 0) {
        if (str_len != *len * AT_PARSE_OCTET_BYTES) {
            return AT_RET_PARSE_PARA_ERROR;
        }
    } else {
        if ((str_len & AT_PARSE_OFFSET_1) != 0) {  /* This is equivalent to str_len % 2 */
            return AT_RET_PARSE_PARA_ERROR;
        }
        *len = str_len >> AT_PARSE_OFFSET_1;  /* This is equivalent to str_len / 2 */
    }

    *val = at_malloc(*len + 1);
    if (*val == NULL) {
        return AT_RET_MALLOC_ERROR;
    }
    (*val)[*len] = '\0';

    return at_parse_hex_string_to_uint8(para_array->str, para_array->str_len, *val);
}

static at_ret_t at_parse_verify_argument_octet_string(const void *args, const at_para_parse_syntax_t *syntax)
{
    uint32_t len = *(uint32_t*)((uintptr_t)args + syntax->entry.octet_string.length_field_offset);

    if ((syntax->attribute & AT_SYNTAX_ATTR_MAX_LENGTH) != 0) {
        if (len > syntax->entry.octet_string.max_length) {
            return AT_RET_PARSE_PARA_ERROR;
        }
    }

    return AT_RET_OK;
}

static at_ret_t at_parse_argument_assign_bit_string(const at_parse_param_t *para_array, const void *args,
                                                    const at_para_parse_syntax_t *syntax)
{
    uint32_t *val = (uint32_t*)((uintptr_t)args + syntax->offset);
    char ch;

    if (para_array->str_len > AT_PARSE_BIT_STRING_MAX_LEN) {
        return AT_RET_PARSE_PARA_ERROR;
    }

    *val = 0;
    /* Converting a binary string to a decimal number */
    for (uint32_t index = 0; index < para_array->str_len; index++) {
        ch = para_array->str[index];
        if (ch != '0' && ch != '1') {
            return AT_RET_PARSE_PARA_ERROR;
        }

        *val = (*val << AT_PARSE_OFFSET_1) | ((uint32_t)ch - (uint32_t)'0');
    }
    return AT_RET_OK;
}

static at_ret_t at_parse_verify_argument_bit_string(const void *args, const at_para_parse_syntax_t *syntax)
{
    uint32_t val = *(uint32_t*)((uintptr_t)args + syntax->offset);

    if ((syntax->attribute & AT_SYNTAX_ATTR_LIST_VALUE) != 0) {
        for (uint16_t index = 0; index < syntax->entry.bit_string_list.num; index++) {
            if (val == syntax->entry.bit_string_list.values[index]) {
                return AT_RET_OK;
            }
        }
        return AT_RET_PARSE_PARA_ERROR;
    }

    if ((syntax->attribute & AT_SYNTAX_ATTR_AT_MAX_VALUE) != 0) {
        if (val > syntax->entry.bit_string_range.max_value) {
            return AT_RET_PARSE_PARA_ERROR;
        }
    }

    return AT_RET_OK;
}

static at_ret_t at_parse_one_argument(at_parse_param_t *para_array, void *args,
                                      const at_para_parse_syntax_t *syntax, uint16_t index)
{
    at_ret_t ret;
    at_parse_args_t parse_table[AT_SYNTAX_TYPE_NUM] = {
        {at_parse_argument_assign_int, at_parse_verify_argument_int},
        {at_parse_argument_assign_string, at_parse_verify_argument_string},
        {at_parse_argument_assign_bit_string, at_parse_verify_argument_bit_string},
        {at_parse_argument_assign_octet_string, at_parse_verify_argument_octet_string}
    };

    if (para_array->str_len == 0) {
        if ((syntax[index].attribute & AT_SYNTAX_ATTR_OPTIONAL) == 0) {
            return AT_RET_PARSE_PARA_MISSING_ERROR;
        } else {
            return AT_RET_OK;
        }
    }

    /* Fill the number of parameters in the parameter structure.Its position is at the beginning of args */
    *(uint32_t*)args |= (1 << (uint32_t)index);
    /* The value of syntax[index].type is obtained from the command list.
       The value of type must be smaller than that of AT_SYNTAX_TYPE_NUM. */
    ret = parse_table[syntax[index].type].assign_func(para_array, args, &syntax[index]);
    if (ret != AT_RET_OK) {
        return ret;
    }

    ret = parse_table[syntax[index].type].verify_func(args, &syntax[index]);
    if (ret != AT_RET_OK) {
        return ret;
    }

    return AT_RET_OK;
}

/* This is a generic interface. Args is used to store AT command parameters.Therefore, the args type is void*. */
at_ret_t at_parse_para_arguments(const char *str, void *args, const at_para_parse_syntax_t *syntax)
{
    at_ret_t ret;
    at_parse_param_table_t *str_table = NULL;

    str_table = at_malloc(sizeof(at_parse_param_table_t));
    if (str_table == NULL) {
        return AT_RET_MALLOC_ERROR;
    }

    ret = at_parse_check_para_num(str, syntax, str_table);
    if (ret != AT_RET_OK) {
        at_free(str_table);
        return ret;
    }

    ret = at_parse_create_para_array(str, str_table);
    if (ret != AT_RET_OK) {
        at_free(str_table);
        return ret;
    }

    for (uint16_t count = 0; count < str_table->param_max_num; count++) {
        ret = at_parse_one_argument(&str_table->params_array[count], args, syntax, count);
        if (ret != AT_RET_OK) {
            at_free(str_table->params_array);
            at_free(str_table);
            return ret;
        }
    }

    at_free(str_table->params_array);
    at_free(str_table);
    return AT_RET_OK;
}

static void at_parse_free_one_argument(void *args, const at_para_parse_syntax_t *syntax)
{
    switch (syntax->type) {
        case AT_SYNTAX_TYPE_STRING: {
                char **val = (char**)((uintptr_t)args + syntax->offset);
                if (*val != NULL) {
                    at_free(*val);
                }
            }
            break;

        case AT_SYNTAX_TYPE_OCTET_STRING: {
                char **val = (char**)((uintptr_t)args + syntax->offset);
                if (*val != NULL) {
                    at_free(*val);
                }
            }
            break;

        default:
            break;
    }
}

/* Args is used to store AT command parameters. */
void at_parse_free_arguments(void *args, const at_para_parse_syntax_t *syntax)
{
    for (uint16_t index = 0;; index++) {
        at_parse_free_one_argument(args, &syntax[index]);
        if (syntax[index].last == true) {
            break;
        }
    }
}
