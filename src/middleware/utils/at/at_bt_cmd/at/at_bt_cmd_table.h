/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: At cmd tab \n
 */

#if ! defined (AT_BT_CMD_AT_CMD_TALBE_H)
#define AT_BT_CMD_AT_CMD_TALBE_H

#include "at.h"

/* HID服务 */
typedef struct {
    char                *para1;
} bt_args_t;
const at_para_parse_syntax_t bt_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(bt_args_t, para1)
    }
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
} hid_send_args_t;
const at_para_parse_syntax_t hid_send_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(hid_send_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(hid_send_args_t, para2)
    }
};

/* BLE管理 */
typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
} send_mouse_args_t;
const at_para_parse_syntax_t send_mouse_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(send_mouse_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 5,
        .offset = offsetof(send_mouse_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 5,
        .offset = offsetof(send_mouse_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(send_mouse_args_t, para4)
    }
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
} bgle_set_name_syntax_t;
const at_para_parse_syntax_t bgle_set_name_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(bgle_set_name_syntax_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 510,
        .offset = offsetof(bgle_set_name_syntax_t, para2)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
} ble_scan_args_t;
const at_para_parse_syntax_t ble_scan_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 5,
        .offset = offsetof(ble_scan_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 5,
        .offset = offsetof(ble_scan_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(ble_scan_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(ble_scan_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(ble_scan_args_t, para5)
    },
};

/* SLE管理 */
typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
} gle_connect_args_t;
const at_para_parse_syntax_t gle_connect_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(gle_connect_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(gle_connect_args_t, para2)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} gle_set_adv_args_t;
const at_para_parse_syntax_t gle_set_adv_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(gle_set_adv_args_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} gle_enable_adv_args_t;
const at_para_parse_syntax_t gle_enable_adv_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 1,
        .offset = offsetof(gle_enable_adv_args_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
    char                 *para7;
    char                 *para8;
    char                 *para9;
} sle_set_phy_t;
const at_para_parse_syntax_t sle_set_phy[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_phy_t, para8)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .last = true,
        .offset = offsetof(sle_set_phy_t, para9)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
} sle_conn_para_update_t;
const at_para_parse_syntax_t sle_conn_para_update[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_conn_para_update_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_conn_para_update_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_conn_para_update_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_conn_para_update_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .last = true,
        .offset = offsetof(sle_conn_para_update_t, para5)
    },
};

/* SLE数传 */
typedef struct {
    uint32_t             para_map;
    char                 *para1;
} gle_add_server_args_t;
const at_para_parse_syntax_t gle_add_server_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(gle_add_server_args_t, para1)
    },
};
typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
} gle_add_service_args_t;
const at_para_parse_syntax_t gle_add_service_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(gle_add_service_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(gle_add_service_args_t, para2)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
} gle_add_property_args_t;
const at_para_parse_syntax_t gle_add_property_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(gle_add_property_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(gle_add_property_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(gle_add_property_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(gle_add_property_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(gle_add_property_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(gle_add_property_args_t, para6)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
    char                 *para7;
    char                 *para8;
} gle_add_descriptor_args_t;
const at_para_parse_syntax_t gle_add_descriptor_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_add_descriptor_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_add_descriptor_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(gle_add_descriptor_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_add_descriptor_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_add_descriptor_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(gle_add_descriptor_args_t, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_add_descriptor_args_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 512,
        .offset = offsetof(gle_add_descriptor_args_t, para8)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} gle_start_service_args_t;
const at_para_parse_syntax_t gle_start_service_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_start_service_args_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
} gle_discovery_services_args_t;
const at_para_parse_syntax_t gle_discovery_services_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(gle_discovery_services_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_discovery_services_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_discovery_services_args_t, para3)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
} gle_write_value_args_t;
const at_para_parse_syntax_t gle_write_value_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(gle_write_value_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_write_value_args_t, para2)
    },    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_write_value_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(gle_write_value_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_write_value_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 521,
        .offset = offsetof(gle_write_value_args_t, para6)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
} gle_read_value_args_t;
const at_para_parse_syntax_t gle_read_value_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(gle_read_value_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_read_value_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 8,
        .offset = offsetof(gle_read_value_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(gle_read_value_args_t, para4)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
} sle_sample_server_send_data_t;
const at_para_parse_syntax_t sle_sample_server_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_sample_server_send_data_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 800,
        .last = true,
        .offset = offsetof(sle_sample_server_send_data_t, para2)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
    char                 *para7;
    char                 *para8;
    char                 *para9;
    char                 *para10;
    char                 *para11;
} ble_set_adv_para_t;
const at_para_parse_syntax_t ble_set_adv_para[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(ble_set_adv_para_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(ble_set_adv_para_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(ble_set_adv_para_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(ble_set_adv_para_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(ble_set_adv_para_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(ble_set_adv_para_t, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(ble_set_adv_para_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(ble_set_adv_para_t, para8)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(ble_set_adv_para_t, para9)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(ble_set_adv_para_t, para10)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .last = true,
        .offset = offsetof(ble_set_adv_para_t, para11)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
} at_ble_set_adv_data_t;
const at_para_parse_syntax_t at_ble_set_adv_data[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(at_ble_set_adv_data_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 128,
        .offset = offsetof(at_ble_set_adv_data_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(at_ble_set_adv_data_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 128,
        .offset = offsetof(at_ble_set_adv_data_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .last = true,
        .offset = offsetof(at_ble_set_adv_data_t, para5)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} ble_enable_adv_by_param_t;
const at_para_parse_syntax_t ble_enable_adv_by_param[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .last = true,
        .offset = offsetof(ble_enable_adv_by_param_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
    char                 *para6;
    char                 *para7;
    char                 *para8;
} sle_set_adv_par_t;
const at_para_parse_syntax_t sle_set_adv_par[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_adv_par_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_adv_par_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_set_adv_par_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_set_adv_par_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_adv_par_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .offset = offsetof(sle_set_adv_par_t, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_adv_par_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 16,
        .last = true,
        .offset = offsetof(sle_set_adv_par_t, para8)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
} sle_set_adv_data_t;
const at_para_parse_syntax_t sle_set_adv_data[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .offset = offsetof(sle_set_adv_data_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_set_adv_data_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .offset = offsetof(sle_set_adv_data_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 521,
        .offset = offsetof(sle_set_adv_data_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 521,
        .last = true,
        .offset = offsetof(sle_set_adv_data_t, para5)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} sle_enable_adv_t;
const at_para_parse_syntax_t sle_enable_adv[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 4,
        .last = true,
        .offset = offsetof(sle_enable_adv_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} ble_read_name_t;
const at_para_parse_syntax_t ble_read_name[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 10,
        .last = true,
        .offset = offsetof(ble_read_name_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} ble_set_local_addr_t;
const at_para_parse_syntax_t ble_set_local_addr[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 12,
        .last = true,
        .offset = offsetof(ble_set_local_addr_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
} bgle_connect_args_t;
const at_para_parse_syntax_t bgle_connect_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 14,
        .last = true,
        .offset = offsetof(bgle_connect_args_t, para1)
    },
};

typedef struct {
    uint32_t             para_map;
    char                 *para1;
    char                 *para2;
    char                 *para3;
    char                 *para4;
    char                 *para5;
} chba_start_args_t;
const at_para_parse_syntax_t bgle_chba_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 1,
        .offset = offsetof(chba_start_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 1,
        .offset = offsetof(chba_start_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 3,
        .offset = offsetof(chba_start_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 1,
        .offset = offsetof(chba_start_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 2,
        .last = true,
        .offset = offsetof(chba_start_args_t, para5)
    },
};

/* SLE管理 */
at_ret_t bt_at_sle_enable(void);
at_ret_t bt_at_gle_at_cmd_set_adv(void);
at_ret_t bt_at_gle_at_cmd_enable_adv(const gle_enable_adv_args_t *args);
at_ret_t bt_at_sle_set_adv_param(const sle_set_adv_par_t *args);
at_ret_t bt_at_sle_set_adv_data(const sle_set_adv_data_t *args);
at_ret_t bt_at_sle_enable_adv_by_param(const sle_enable_adv_t *args);
at_ret_t bt_at_sle_disable_adv_by_param(const sle_enable_adv_t *args);
at_ret_t bt_at_gle_at_cmd_connect(const gle_connect_args_t *args);
at_ret_t bt_at_gle_at_cmd_disconnect(const gle_connect_args_t *args);
at_ret_t bt_at_gle_pair_remote(const gle_connect_args_t *args);
at_ret_t bt_at_gle_remove_pair_remote(const gle_connect_args_t *args);
at_ret_t bt_at_gle_sdk_get_paired_devices_num(void);
at_ret_t bt_at_gle_sdk_get_paired_devices(void);
at_ret_t bt_at_gle_sdk_get_pair_state(void);
at_ret_t bt_at_sle_set_phy(const sle_set_phy_t *arg);
at_ret_t bt_at_sle_conn_para_update(const sle_conn_para_update_t *arg);
/* SLE数传 */
at_ret_t bt_at_gle_at_cmd_add_server(const gle_add_server_args_t *args);
at_ret_t bt_at_gle_at_cmd_add_service(const gle_add_service_args_t *args);
at_ret_t bt_at_gle_at_cmd_add_property(const gle_add_property_args_t *args);
at_ret_t bt_at_gle_at_cmd_add_descriptor(const gle_add_descriptor_args_t *args);
at_ret_t bt_at_gle_at_cmd_start_service(const gle_start_service_args_t *args);
at_ret_t bt_at_gle_at_cmd_ssaps_register_cbks(void);
at_ret_t bt_at_gle_at_cmd_dd_register_cbks(void);
at_ret_t bt_at_gle_at_cmd_ssapc_register_cbks(void);
at_ret_t bt_at_gle_at_cmd_cm_register_cbks(void);
at_ret_t bt_at_gle_at_cmd_discovery_services(const gle_discovery_services_args_t *args);
at_ret_t bt_at_gle_at_cmd_ssapc_write_req(const gle_write_value_args_t *args);
at_ret_t bt_at_gle_at_cmd_ssapc_read_req(const gle_read_value_args_t *args);

/* BLE管理 */
at_ret_t bt_at_ble_adv_enable(void);
at_ret_t bt_at_ble_adv_disable(void);
at_ret_t bt_at_ble_disconnect(void);
at_ret_t bt_at_ble_conn_par_upd(const sle_conn_para_update_t *arg);
at_ret_t bt_at_ble_set_adv_param(const ble_set_adv_para_t *arg);
at_ret_t bt_at_ble_set_adv_data(const at_ble_set_adv_data_t *arg);
at_ret_t bt_at_ble_enable_adv_by_param(const ble_enable_adv_by_param_t *arg);
at_ret_t bt_at_ble_disable_adv_by_param(const ble_enable_adv_by_param_t *arg);
at_ret_t bt_at_ble_enter_scan(const ble_scan_args_t *args);
at_ret_t bt_at_send_cmd_high_mouse_input(const send_mouse_args_t *args);
at_ret_t bt_at_send_cmd_ble_connect_peer(const bgle_connect_args_t *args);
at_ret_t bt_at_cmd_gap_enable_ble(void);
at_ret_t bt_at_cmd_hid_mouse_init(void);

/* HID服务 */
at_ret_t bt_at_ble_at_cmd_gap_ble_set_scan_parameters(const ble_scan_args_t *args);
at_ret_t bt_at_ble_at_cmd_gap_start_scan(void);
at_ret_t bt_at_ble_at_cmd_gap_stop_scan(void);

/* HID服务 */
at_ret_t bt_at_hid_init(void);
at_ret_t bt_at_hid_mouse_server_send_input_report(const send_mouse_args_t *args);
at_ret_t bt_at_hid_mouse_server_send_input_report_by_uuid(const send_mouse_args_t *args);
at_ret_t bt_at_hid_server_send_input_report_down(const hid_send_args_t *args);
at_ret_t bt_at_hid_server_send_input_report_up(const hid_send_args_t *args);
at_ret_t bt_at_hid_server_send_input_report_down_by_uuid(const hid_send_args_t *args);
at_ret_t bt_at_hid_server_send_input_report_up_by_uuid(const hid_send_args_t *args);
at_ret_t bt_at_query_paired_device_info(void);
at_ret_t bt_at_set_product_type(void);
at_ret_t bt_at_read_bdaddr(void);
at_ret_t bt_at_read_name(const ble_read_name_t *args);
at_ret_t bt_at_write_addr(const ble_set_local_addr_t *args);
at_ret_t bt_at_write_name(const bgle_set_name_syntax_t *args);

// sle service sample
at_ret_t bt_at_sle_sample_client_init(void);
at_ret_t bt_at_sle_sample_client_scan(void);
at_ret_t bt_at_sle_sample_server_init(void);
at_ret_t bt_at_sle_sample_server_send_data_by_uuid(const sle_sample_server_send_data_t *arg);
at_ret_t bt_at_sle_sample_server_send_data_by_handle(const sle_sample_server_send_data_t *arg);

#ifdef CHBA_SUPPORT
// chba
at_ret_t bt_at_chba_start(const chba_start_args_t *args);
#endif


const at_cmd_entry_t at_bt_cmd_parse_table[] = {
    {
        "BLECONNPARUPD",
        1,
        0,
        sle_conn_para_update,
        NULL,
        (at_set_func_t)bt_at_ble_conn_par_upd,
        NULL,
        NULL,
    },
    {
        "BTENABLE",    // 起广播
        1,
        0,
        NULL,   // 参数类型
        bt_at_ble_adv_enable,        // 无参数时调用的函数
        (at_set_func_t)NULL,   // 有参数时调用的函数
        NULL,
        NULL,
    },
    {
        "BTDISABLE",    // 停广播
        1,
        0,
        NULL,   // 参数类型
        bt_at_ble_adv_disable,        // 无参数时调用的函数
        (at_set_func_t)NULL,   // 有参数时调用的函数
        NULL,
        NULL,
    },
    {
        "BLESETADVPAR",
        1,
        0,
        ble_set_adv_para,
        NULL,
        (at_set_func_t)bt_at_ble_set_adv_param,
        NULL,
        NULL,
    },
    {
        "BLESETADVDATA",
        1,
        0,
        at_ble_set_adv_data,
        NULL,
        (at_set_func_t)bt_at_ble_set_adv_data,
        NULL,
        NULL,
    },
    {
        "BLEENADVBYPAR",
        1,
        0,
        ble_enable_adv_by_param,
        NULL,
        (at_set_func_t)bt_at_ble_enable_adv_by_param,
        NULL,
        NULL,
    },
    {
        "BLEDISADVBYPAR",
        1,
        0,
        ble_enable_adv_by_param,
        NULL,
        (at_set_func_t)bt_at_ble_disable_adv_by_param,
        NULL,
        NULL,
    },
    {
        "GETBTNAME",    // 查名称
        2,
        0,
        ble_read_name,
        NULL,
        (at_set_func_t)bt_at_read_name,
        NULL,
        NULL,
    },
    {
        "GETPHYNUM",    // 查地址
        2,
        0,
        NULL,
        bt_at_read_bdaddr,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "BTNAME",   // 更改蓝牙名称
        3,
        0,
        bgle_set_name_syntax,
        NULL,
        (at_set_func_t)bt_at_write_name,
        NULL,
        NULL,
    },
    {
        "PHYNUM",   // 更改蓝牙地址
        4,
        0,
        ble_set_local_addr,
        NULL,
        (at_set_func_t)bt_at_write_addr,
        NULL,
        NULL,
    },
    {
        "BTDISCONN",    // BLE 断链
        3,
        0,
        NULL,
        bt_at_ble_disconnect,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "GAPENABLEBLE",
        15,
        0,
        NULL,
        bt_at_cmd_gap_enable_ble,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SETPRODUCTTYPE",
        4,
        0,
        NULL,
        bt_at_set_product_type,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
#ifdef CONFIG_ENABLE_BT_SAMPLE
    {
        "HIDMOUSEINIT",
        15,
        0,
        NULL,
        bt_at_cmd_hid_mouse_init,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "HIDKEYBOARDINIT",
        5,
        0,
        NULL,
        bt_at_hid_init,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "KEYBOARDINPUTPDOWN",
        6,
        0,
        hid_send_syntax,
        NULL,
        (at_set_func_t)bt_at_hid_server_send_input_report_down,
        NULL,
        NULL,
    },
    {
        "KEYBOARDINPUTPUP",
        7,
        0,
        hid_send_syntax,
        NULL,
        (at_set_func_t)bt_at_hid_server_send_input_report_up,
        NULL,
        NULL,
    },
    {
        "KEYBOARDINPUTPDOWNBYUUID",
        6,
        0,
        hid_send_syntax,
        NULL,
        (at_set_func_t)bt_at_hid_server_send_input_report_down_by_uuid,
        NULL,
        NULL,
    },
    {
        "KEYBOARDINPUTPUPBYUUID",
        7,
        0,
        hid_send_syntax,
        NULL,
        (at_set_func_t)bt_at_hid_server_send_input_report_up_by_uuid,
        NULL,
        NULL,
    },
    {
        "HIDMOUSEINPUT",
        8,
        0,
        send_mouse_syntax,
        NULL,
        (at_set_func_t)bt_at_hid_mouse_server_send_input_report,
        NULL,
        NULL,
    },
    {
        "HIDMOUSEINPUTBYUUID",
        8,
        0,
        send_mouse_syntax,
        NULL,
        (at_set_func_t)bt_at_hid_mouse_server_send_input_report_by_uuid,
        NULL,
        NULL,
    },
#endif
    {
        "MOUSEINPUT",
        8,
        0,
        send_mouse_syntax,
        NULL,
        (at_set_func_t)bt_at_send_cmd_high_mouse_input,
        NULL,
        NULL,
    },
    {
        "BTCONNADDR",
        9,
        0,
        bgle_connect_syntax,
        NULL,
        (at_set_func_t)bt_at_send_cmd_ble_connect_peer,
        NULL,
        NULL,
    },
    {
        "BLEENTERSCAN",
        9,
        0,
        ble_scan_syntax,
        NULL,
        (at_set_func_t)bt_at_ble_enter_scan,
        NULL,
        NULL,
    },
    {
        "BLESETSCANPARAM",
        9,
        0,
        ble_scan_syntax,
        NULL,
        (at_set_func_t)bt_at_ble_at_cmd_gap_ble_set_scan_parameters,
        NULL,
        NULL,
    },
    {
        "BLESCANOPEN",
        12,
        0,
        NULL,
        bt_at_ble_at_cmd_gap_start_scan,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "BLESCANCLOSE",
        12,
        0,
        NULL,
        bt_at_ble_at_cmd_gap_stop_scan,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "BTPAIRINFO?",
        10,
        0,
        NULL,
        bt_at_query_paired_device_info,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SLEENABLE",    // 使能SLE
        1,
        0,
        NULL,   // 参数类型
        bt_at_sle_enable,        // 无参数时调用的函数
        (at_set_func_t)NULL,   // 有参数时调用的函数
        NULL,
        NULL,
    },
    {
        "SLESETADV",
        12,
        0,
        NULL,
        bt_at_gle_at_cmd_set_adv,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SLEENABLEADV",
        13,
        0,
        gle_enable_adv_syntax,
        NULL,
        (at_set_func_t)bt_at_gle_at_cmd_enable_adv,
        NULL,
        NULL,
    },
    {
        "SLESETADVPAR",
        13,
        0,
        sle_set_adv_par,
        NULL,
        (at_set_func_t)bt_at_sle_set_adv_param,
        NULL,
        NULL,
    },
    {
        "SLESETADVDATA",
        13,
        0,
        sle_set_adv_data,
        NULL,
        (at_set_func_t)bt_at_sle_set_adv_data,
        NULL,
        NULL,
    },
    {
        "SLEENADVBYPAR",
        13,
        0,
        sle_enable_adv,
        NULL,
        (at_set_func_t)bt_at_sle_enable_adv_by_param,
        NULL,
        NULL,
    },
    {
        "SLEDISADVBYPAR",
        13,
        0,
        sle_enable_adv,
        NULL,
        (at_set_func_t)bt_at_sle_disable_adv_by_param,
        NULL,
        NULL,
    },
    {
        "SLECONNADDR",
        11,
        0,
        gle_connect_syntax,
        NULL,
        (at_set_func_t)bt_at_gle_at_cmd_connect,
        NULL,
        NULL,
    },
    {
        "SLEDISCONN",
        14,
        0,
        gle_connect_syntax,
        NULL,
        (at_set_func_t)bt_at_gle_at_cmd_disconnect,
        NULL,
        NULL,
    },
    {
        "SLEPAIRREMOTE",
        14,
        0,
        gle_connect_syntax,
        NULL,
        (at_set_func_t)bt_at_gle_pair_remote,
        NULL,
        NULL,
    },
    {
        "SLEREMOVEPAIR",
        14,
        0,
        gle_connect_syntax,
        NULL,
        (at_set_func_t)bt_at_gle_remove_pair_remote,
        NULL,
        NULL,
    },
    {
        "SLESETPHY",
        14,
        0,
        sle_set_phy,
        NULL,
        (at_set_func_t)bt_at_sle_set_phy,
        NULL,
        NULL,
    },
    {
        "SLECONNPARUPD",
        14,
        0,
        sle_conn_para_update,
        NULL,
        (at_set_func_t)bt_at_sle_conn_para_update,
        NULL,
        NULL,
    },
    {
        "SLEGETPAIRDEVICESNUM",
        14,
        0,
        NULL,
        bt_at_gle_sdk_get_paired_devices_num,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SLEGETPAIRDEVICES",
        14,
        0,
        NULL,
        bt_at_gle_sdk_get_paired_devices,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SLEGETPAIRSTATE",
        14,
        0,
        NULL,
        bt_at_gle_sdk_get_pair_state,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SLEADDSERVER",
        16,
        0,
        gle_add_server_syntax,
        NULL,
        (at_set_func_t)bt_at_gle_at_cmd_add_server,
        NULL,
        NULL,
    },
    {
        "SLEADDSERVICE",
        17,
        0,
        gle_add_service_syntax,
        NULL,
        (at_set_func_t)bt_at_gle_at_cmd_add_service,
        NULL,
        NULL,
    },
    {
        "SLEADDPROPERTY",
        18,
        0,
        gle_add_property_syntax,
        NULL,
        (at_set_func_t)bt_at_gle_at_cmd_add_property,
        NULL,
        NULL,
    },
    {
        "SLEADDDESCRIPTOR",
        19,
        0,
        gle_add_descriptor_syntax,
        NULL,
        (at_set_func_t)bt_at_gle_at_cmd_add_descriptor,
        NULL,
        NULL,
    },
    {
        "SLESTARTSERVICE",
        20,
        0,
        gle_start_service_syntax,
        NULL,
        (at_set_func_t)bt_at_gle_at_cmd_start_service,
        NULL,
        NULL,
    },
    {
        "SLESSAPSERREGISTER",
        21,
        0,
        NULL,
        bt_at_gle_at_cmd_ssaps_register_cbks,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SLESSAPCENREGISTER",
        23,
        0,
        NULL,
        bt_at_gle_at_cmd_ssapc_register_cbks,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SLEDDREGISTER",
        22,
        0,
        NULL,
        bt_at_gle_at_cmd_dd_register_cbks,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SLECMREGISTER",
        24,
        0,
        NULL,
        bt_at_gle_at_cmd_cm_register_cbks,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SLEDISCOVERYSERVICES",
        25,
        0,
        gle_discovery_services_syntax,
        NULL,
        (at_set_func_t)bt_at_gle_at_cmd_discovery_services,
        NULL,
        NULL,
    },
    {
        "SLESSAPCENWRITE",
        26,
        0,
        gle_write_value_syntax,
        NULL,
        (at_set_func_t)bt_at_gle_at_cmd_ssapc_write_req,
        NULL,
        NULL,
    },
    {
        "SLESSAPCENREAD",
        27,
        0,
        gle_read_value_syntax,
        NULL,
        (at_set_func_t)bt_at_gle_at_cmd_ssapc_read_req,
        NULL,
        NULL,
    },
    {
        "SLECLIENTINIT",
        28,
        0,
        NULL,
        bt_at_sle_sample_client_init,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SLECLIENTSCAN",
        28,
        0,
        NULL,
        bt_at_sle_sample_client_scan,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SLESERVERINIT",
        28,
        0,
        NULL,
        bt_at_sle_sample_server_init,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "SLESSENDBYUUID",
        28,
        0,
        sle_sample_server_syntax,
        NULL,
        (at_set_func_t)bt_at_sle_sample_server_send_data_by_uuid,
        NULL,
        NULL,
    },
    {
        "SLESSENDBYHANDLE",
        28,
        0,
        sle_sample_server_syntax,
        NULL,
        (at_set_func_t)bt_at_sle_sample_server_send_data_by_handle,
        NULL,
        NULL,
    },
#ifdef CHBA_SUPPORT
    {
        "CHBASTART",
        29,
        0,
        bgle_chba_syntax,
        NULL,
        (at_set_func_t)bt_at_chba_start,
        NULL,
        NULL,
    },
#endif
};

#endif  /* AT_BT_CMD_TALBE_H */
