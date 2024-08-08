/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Common operations on the hichain, including session creation and destruction. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hichain.h"

DLL_API_PUBLIC void registe_log(struct log_func_group *log)
{
    app_call1(APP_CALL_REGISTE_LOG, void, struct log_func_group *, log);
}

DLL_API_PUBLIC hc_handle get_instance(const struct session_identity *identity, enum hc_type type,
    const struct hc_call_back *call_back)
{
    return app_call3(APP_CALL_GET_INSTANCE, hc_handle, const struct session_identity *, identity,
        enum hc_type, type, const struct hc_call_back *, call_back);
}

DLL_API_PUBLIC void destroy(hc_handle *handle)
{
    app_call1(APP_CALL_DESTROY, void, hc_handle *, handle);
}

DLL_API_PUBLIC void set_context(hc_handle handle, void *context)
{
    app_call2(APP_CALL_SET_CONTEXT, void, hc_handle, handle, void *, context);
}

DLL_API_PUBLIC int32_t receive_data(hc_handle handle, struct uint8_buff *data)
{
    return app_call2(APP_CALL_RECEIVE_DATA, int32_t, hc_handle, handle, struct uint8_buff *, data);
}

DLL_API_PUBLIC int32_t receive_data_with_json_object(hc_handle handle, const void *json_object)
{
    return app_call2(APP_CALL_RECEIVE_DATA_WITH_JSON_OBJECT, int32_t, hc_handle, handle, const void *, json_object);
}

#ifndef _CUT_API_
DLL_API_PUBLIC int32_t init_center(const struct hc_package_name *package_name,
    const struct hc_service_type *service_type, const struct hc_auth_id *auth_id, struct hc_key_alias *dek)
{
    return app_call4(APP_CALL_INIT_CENTER, int32_t, const struct hc_package_name *, package_name,
        const struct hc_service_type *, service_type, const struct hc_auth_id *, auth_id,
        struct hc_key_alias *, dek);
}

DLL_API_PUBLIC int32_t start_pake(hc_handle handle, const struct operation_parameter *params)
{
    return app_call2(APP_CALL_START_PAKE, int32_t, hc_handle, handle, const struct operation_parameter *, params);
}

DLL_API_PUBLIC int32_t authenticate_peer(hc_handle handle, struct operation_parameter *params)
{
    return app_call2(APP_CALL_AUTHENTICATE_PEER, int32_t, hc_handle, handle, struct operation_parameter *, params);
}

DLL_API_PUBLIC int32_t delete_local_auth_info(hc_handle handle, struct hc_user_info *user_info)
{
    return app_call2(APP_CALL_DELETE_LOCAL_AUTH_INFO, int32_t, hc_handle, handle, struct hc_user_info *, user_info);
}

DLL_API_PUBLIC int32_t import_auth_info(hc_handle handle, struct hc_user_info *user_info,
    struct hc_auth_id *auth_id, enum hc_export_type auth_info_type, struct uint8_buff *auth_info)
{
    return app_call5(APP_CALL_IMPORT_AUTH_INFO, int32_t, hc_handle, handle, struct hc_user_info *, user_info,
        struct hc_auth_id *, auth_id, enum hc_export_type, auth_info_type, struct uint8_buff *, auth_info);
}

int32_t add_auth_info(hc_handle handle, const struct operation_parameter *params,
    const struct hc_auth_id *auth_id, int32_t user_type)
{
    return app_call4(APP_CALL_ADD_AUTH_INFO, int32_t, hc_handle, handle, const struct operation_parameter *, params,
        const struct hc_auth_id *, auth_id, int32_t, user_type);
}

int32_t remove_auth_info(hc_handle handle, const struct operation_parameter *params,
    const struct hc_auth_id *auth_id, int32_t user_type)
{
    return app_call4(APP_CALL_REMOVE_AUTH_INFO, int32_t, hc_handle, handle, const struct operation_parameter *, params,
        const struct hc_auth_id *, auth_id, int32_t, user_type);
}

DLL_API_PUBLIC int32_t is_trust_peer(hc_handle handle, struct hc_user_info *user_info)
{
    return app_call2(APP_CALL_IS_TRUST_PEER, int32_t, hc_handle, handle, struct hc_user_info *, user_info);
}

DLL_API_PUBLIC uint32_t list_trust_peers(hc_handle handle, int32_t trust_user_type,
    struct hc_auth_id *owner_auth_id, struct hc_auth_id **auth_id_list)
{
    return app_call4(APP_CALL_LIST_TRUST_PEERS, uint32_t, hc_handle, handle, int32_t, trust_user_type,
        struct hc_auth_id *, owner_auth_id, struct hc_auth_id **, auth_id_list);
}
#endif /* _CUT_XXX_ */

DLL_API_PUBLIC void set_self_auth_id(hc_handle handle, struct uint8_buff *data)
{
    app_call2(APP_CALL_SET_SELF_AUTH_ID, void, hc_handle, handle, struct uint8_buff *, data);
}