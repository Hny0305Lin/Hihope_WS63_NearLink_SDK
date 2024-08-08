/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides AT command source \n
 */

#include "at_base.h"
#include "at_cmd.h"

typedef struct at_cmd_list_t {
    const at_cmd_entry_t  *table;
    uint32_t              len;
    struct at_cmd_list_t  *next;
} at_cmd_list_t;

at_cmd_list_t *g_at_cmd_list = NULL;
uint32_t g_at_cmd_struct_max_size = 0;

uint32_t at_cmd_get_entry_total(void)
{
    uint32_t nr_size = 0;
    at_cmd_list_t *list = g_at_cmd_list;

    if (list == NULL) {
        return 0;
    }

    while (list != NULL) {
        for (uint32_t index = 0; index < list->len; index++) {
            nr_size++;
        }
        list = list->next;
    }
    return nr_size;
}

uint32_t at_cmd_get_all_entrys(const at_cmd_entry_t **entrys, uint32_t cnt)
{
    uint32_t total = 0;
    at_cmd_list_t *list = g_at_cmd_list;

    if (list == NULL) {
        return 0;
    }

    while (list != NULL) {
        for (uint32_t index = 0; index < list->len; index++) {
            entrys[total] = &list->table[index];
            total++;
            if (total > cnt) {
                return total;
            }
        }
        list = list->next;
    }
    return total;
}

static errcode_t at_cmd_table_add(const at_cmd_entry_t *table, uint32_t length)
{
    at_cmd_list_t *tail = g_at_cmd_list;
    at_cmd_list_t *new_node = at_malloc(sizeof(at_cmd_list_t));
    if (new_node == NULL) {
        return ERRCODE_MALLOC;
    }
    new_node->table = table;
    new_node->len = length;
    new_node->next = NULL;

    if (tail == NULL) {
        g_at_cmd_list = new_node;
        return ERRCODE_SUCC;
    }

    while (tail->next != NULL) {
        tail = tail->next;
    }
    tail->next = new_node;

    return ERRCODE_SUCC;
}

static uint16_t at_cmd_get_name_length(const char* str)
{
    uint16_t len = 0;
    const char* ch = str;

    while ((*ch >= 'A' && *ch <= 'Z') ||
           (*ch >= 'a' && *ch <= 'z') ||
           (*ch >= '0' && *ch <= '9')) {
        len++;
        ch++;
    }

    return len;
}

const at_cmd_entry_t* at_cmd_find_entry(char *str, uint16_t *offset)
{
    at_cmd_list_t *list = g_at_cmd_list;
    if (list == NULL || str == NULL) {
        return NULL;
    }

    char *name = str + *offset;
    uint16_t name_len = at_cmd_get_name_length(name);
    at_base_toupper(name, name_len);
    while (list != NULL) {
        for (uint32_t index = 0; index < list->len; index++) {
            if (list->table[index].name == NULL) { return NULL; }
            if ((strncmp(list->table[index].name, name, name_len) == 0) &&
                (name_len == strlen(list->table[index].name))) {
                *offset = *offset + name_len;
                return &list->table[index];
            }
        }
        list = list->next;
    }
    return NULL;
}

#ifdef CONFIG_AT_SUPPORT_CMD_TABLE_CHECK
const at_cmd_entry_t* at_cmd_find_entry_by_name(const char *name)
{
    at_cmd_list_t *list = g_at_cmd_list;
    if (list == NULL) {
        return NULL;
    }

    while (list != NULL) {
        for (uint32_t index = 0; index < list->len; index++) {
            if (strcmp((char*)list->table[index].name, name) == 0) {
                return &list->table[index];
            }
        }
        list = list->next;
    }
    return NULL;
}

static errcode_t at_cmd_entry_name_check(const char *name)
{
    uint16_t name_len = (uint16_t)strlen(name);
    if (name_len > AT_CMD_NAME_MAX_LENGTH) {
        return ERRCODE_AT_CMD_TABLE_PARA_ERROR;
    }

    for (uint16_t index = 0; index < name_len; index++) {
        if (name[index] < 'A' || name[index] > 'Z') {
            return ERRCODE_AT_CMD_TABLE_PARA_ERROR;
        }
    }
    return ERRCODE_SUCC;
}

static errcode_t at_cmd_entry_syntax_int_check(const at_para_parse_syntax_t *syntax)
{
    if (((syntax->attribute & (AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE)) != 0) &&
        ((syntax->attribute & AT_SYNTAX_ATTR_LIST_VALUE) != 0)) {
        return ERRCODE_AT_CMD_TABLE_PARA_ERROR;
    }
    return ERRCODE_SUCC;
}

static errcode_t at_cmd_entry_syntax_string_check(const at_para_parse_syntax_t *syntax)
{
    if (((syntax->attribute & AT_SYNTAX_ATTR_MAX_LENGTH) != 0) &&
        ((syntax->attribute & AT_SYNTAX_ATTR_LIST_VALUE) != 0)) {
        return ERRCODE_AT_CMD_TABLE_PARA_ERROR;
    }
    return ERRCODE_SUCC;
}

static errcode_t at_cmd_entry_syntax_bit_string_check(const at_para_parse_syntax_t *syntax)
{
    if (((syntax->attribute & AT_SYNTAX_ATTR_AT_MAX_VALUE) != 0) &&
        ((syntax->attribute & AT_SYNTAX_ATTR_LIST_VALUE) != 0)) {
        return ERRCODE_AT_CMD_TABLE_PARA_ERROR;
    }
    return ERRCODE_SUCC;
}

static errcode_t at_cmd_entry_syntax_check(const at_para_parse_syntax_t *syntax)
{
    errcode_t err_ret = ERRCODE_SUCC;
    switch (syntax->type) {
        case AT_SYNTAX_TYPE_INT:
            err_ret = at_cmd_entry_syntax_int_check(syntax);
            break;
        case AT_SYNTAX_TYPE_STRING:
            err_ret = at_cmd_entry_syntax_string_check(syntax);
            break;
        case AT_SYNTAX_TYPE_BIT_STRING:
            err_ret = at_cmd_entry_syntax_bit_string_check(syntax);
            break;
        default:
            break;
    }
    return err_ret;
}

static errcode_t at_cmd_entry_syntax_table_check(const at_para_parse_syntax_t *syntax)
{
    errcode_t err_ret;
    if (syntax == NULL) {
        return ERRCODE_SUCC;
    }

    for (uint16_t index = 0; index < AT_PARA_MAX_NUM; index++) {
        err_ret = at_cmd_entry_syntax_check(&syntax[index]);
        if (err_ret != ERRCODE_SUCC) {
            return err_ret;
        }
        if (syntax[index].last == true) {
            break;
        }
    }

    return ERRCODE_SUCC;
}

static errcode_t at_cmd_entry_table_check(const at_cmd_entry_t *table)
{
    if (table->cmd == NULL &&
        table->set == NULL &&
        table->read == NULL &&
#ifdef CONFIG_AT_SUPPORT_QUERY
        table->query == NULL &&
#endif
        table->test == NULL) {
        return ERRCODE_AT_CMD_TABLE_PARA_ERROR;
    }

    if (table->set != NULL && table->syntax == NULL) {
        return ERRCODE_AT_CMD_TABLE_PARA_ERROR;
    }

    return ERRCODE_SUCC;
}

static errcode_t at_cmd_entry_check(const at_cmd_entry_t *table)
{
    errcode_t err_ret;
    err_ret = at_cmd_entry_table_check(table);
    if (err_ret != ERRCODE_SUCC) {
        return err_ret;
    }

    err_ret = at_cmd_entry_name_check(table->name);
    if (err_ret != ERRCODE_SUCC) {
        return err_ret;
    }

    err_ret = at_cmd_entry_syntax_table_check(table->syntax);
    if (err_ret != ERRCODE_SUCC) {
        return err_ret;
    }
    return ERRCODE_SUCC;
}

static errcode_t at_cmd_table_check(const at_cmd_entry_t *table, uint32_t length)
{
    errcode_t ret = ERRCODE_SUCC;
    for (uint32_t index = 0; index < length; index++) {
        if (at_cmd_find_entry_by_name(table[index].name) != NULL) {
            return ERRCODE_AT_CMD_REPEAT;
        }

        ret = at_cmd_entry_check(&table[index]);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }
    return ERRCODE_SUCC;
}
#endif

uint32_t at_cmd_get_max_struct_size(void)
{
    return g_at_cmd_struct_max_size;
}

errcode_t uapi_at_cmd_table_register(const at_cmd_entry_t *table, uint32_t len,
                                     uint32_t struct_max_size)
{
    errcode_t ret;
    if (table == NULL || len == 0) {
        return ERRCODE_INVALID_PARAM;
    }
#ifdef CONFIG_AT_SUPPORT_CMD_TABLE_CHECK
    ret = at_cmd_table_check(table, len);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#endif
    ret = at_cmd_table_add(table, len);
    if ((ret == ERRCODE_SUCC) && (g_at_cmd_struct_max_size < struct_max_size)) {
        g_at_cmd_struct_max_size = struct_max_size;
    }
    return ret;
}
