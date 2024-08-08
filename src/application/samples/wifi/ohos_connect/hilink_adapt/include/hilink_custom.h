/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: HiLink SDK 定制化接口头文件（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_CUSTOM_H
#define HILINK_CUSTOM_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    unsigned int method;
    const char *uri;
    const char *data;
} CloudMsgInfo;

typedef int (*RecvSysEventCb)(const char *payload);

#if defined(AM_PART_APOLLO3P)
/*
 * 挂起hilink任务
 * 该函数由设备开发者或厂商调用
 */
void HILINK_MainTaskSuspend();

/*
 * 恢复hilink任务
 * 该函数由设备开发者或厂商调用
 */
void HILINK_MainTaskResume();
#endif

/*
 * 获取设备表面的最强点信号发射功率强度，最强点位置的确定以及功率测试方
 * 法，参照hilink认证wifi靠近发现功率设置及测试方法指导文档，power为出参
 * ，单位dbm，返回设备表面的最强信号强度值，如果厂商不想使用wifi靠近发现功
 * 能，接口直接返-1，sdk就不做wifi靠近发现的初始化，如果需要使用wifi靠近
 * 发现，则接口返回0，power返回对应的功率值，power的有效值必须<=20dbm，如
 * 果接口返回0，但power大于20，则也不做wifi靠近发现的初始化，功能不可用
 */
typedef int (*HILINK_GetDevSurfaceWifiPower)(char *power);

/*
 * 功能: 获取当前设备唯一身份标识回调函数
 * 返回: 0，获取成功；返回非0，获取失败。
 * 注意: (1)仅android系统设备适配此接口
 *       (2)固定长度6字节
 *       (3)整个设备生命周期不可改变，包括设备重启和恢复出厂等
 */
typedef int (*HILINK_GetUniqueIdentifier)(unsigned char *id, unsigned int len);

/*
 * 功能: 获取当前设备udid，仅ohos系统适用
 * 返回: 0，获取成功；返回非0，获取失败。
 */
typedef int (*HILINK_GetDeviceUdidCallback)(char *udid, int size);

/*
 * 功能: 接收report消息后云侧返回的结果回调函数
 * 返回: 0，获取成功；返回非0，获取失败。
 */
typedef void (*NotifyTokenCallback)(unsigned short token, const unsigned char *payload, unsigned int payloadLen);

/*
 * 获取家居云的URL
 * 该函数由设备开发者或厂商调用
 * 返回0为获取成功，否则失败
 */
int HILINK_GetCloudUrl(char *url, unsigned int len);

/*
 * 获取HOTA服务器的URL
 * 该函数由设备开发者或厂商调用，不需要释放
 * 返回非NULL为获取成功，否则失败
 */
const char *HILINK_GetHotaUrl(void);

/*
 * 设置配网信息
 * 入参数据为json格式字符串，具体内容字段如下：
 * {
 *   "ssid": "xxx-xxxx", //路由器SSID字段,必选
 *   "password": "xxxxxxxx", //路由器密码字段,必选
 *   "devId": "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxx", //注册信息-设备ID字段,必选
 *   "psk": "xxxxxxxxxxxxxxxxxxxxxxx", //注册信息-预制秘钥字段(转换十六进制字符串),必选
 *   "code": "xxxxxxx", //注册信息-激活码字段,必选
 *   "cloudPrimaryUrl": "xxxxx.xxxxx.xx", //主域名字段(主备域名),必选
 *   "cloudStandbyUrl": "xxxxx.xxxxx.xx", //备份域名字段(主备域名),必选
 *   "cloudUrl": "xxxxx.xxxxx.xx", //域名字段(兼容方案),可选
 *   "WifiBgnSwitch": 1, //WiFi工作模式字段,可选
 *   "timeZoneDisplay": "GMT+08:00", //时区信息,可选
 *   "timeZoneId": "Asia/Shanghai" //时区ID,可选
 * }
 * 返回0表示设置成功，其他表示设置失败(-2表示HiLink未处于接收配网数据状态)
 */
int HILINK_SetNetConfigInfo(const char *info);

/*
 * 功能: 查询HiLink SDK配置信息保存路径，仅posix或utils_file文件系统适用。
 * 参数: (1) path，出入参，保存路径的缓冲区；
 *       (2) len，入参，缓冲区长度。
 * 返回: 0，获取成功；非0，获取失败。
 * 注意: 非posix或utils_file文件系统无此接口实现
 */
int HILINK_GetConfigInfoPath(char *path, unsigned int len);

/*
 * 功能: 设置HiLink SDK配置信息保存路径，默认保存在/usrdata/hilink目录下，仅posix或utils_file文件系统适用。
 * 参数: path，路径信息，绝对路径长度在posix下不超过127，在utils_file下不超过15。
 * 返回: 0，设置成功；非0，设置失败。
 * 注意: 非posix或utils_file文件系统无此接口实现
 */
int HILINK_SetConfigInfoPath(const char *path);

/*
 * HiLink SDK外部诊断信息记录接口
 */
void HILINK_DiagnosisRecordEx(int errCode);

/*
 * 注册获取设备表面的最强点信号发射功率强度回调函数
 * 返回0表示成功，返回非0失败
 */
int HILINK_RegDevSurfacePowerCallback(HILINK_GetDevSurfaceWifiPower cb);

/*
 * 功能: 注册获取当前设备唯一身份标识回调函数
 * 返回: 0表示成功，返回非0失败
 */
int HILINK_RegUniqueIdentifierCallback(HILINK_GetUniqueIdentifier cb);

/*
 * 功能: 注册获取当前设备udid接口
 * 返回: 0表示成功，返回非0失败
 */
int HILINK_RegGetUdidCallback(HILINK_GetDeviceUdidCallback cb);

/*
 * 功能：注册上报消息时云侧返回token的回调，与 HILINK_IndicateCharState 配合确定消息上报结果
 * 参数：(1) NotifyTokenCallback 入参，上报结果的处理函数
 * 返回：返回0表示注册成功，否则失败
 * 注意：此接口当前仅适用于直连云模式
 */
int HILINK_RegisterNotifyTokenCallback(NotifyTokenCallback cb);

/*
 * 功能：(1) 主动上报服务属性状态且得到消息的token1
 *       (2) 在 HILINK_RegisterNotifyTokenCallback 注册的回调中返回上报结果的token2
 *       (3) 匹配token1和token2得到本次上报结果
 * 参数：(1) svcId 入参，服务ID
 *       (2) payload 入参，json格式服务属性数据
 *       (3) payloadLen 入参，payload长度
 *       (4) token 出参，此次上报消息的token值
 * 返回：返回0表示服务状态上报成功，返回-1表示服务状态上报失败
 * 注意：(1) 该接口只有同步使用方式，对于事件类上报推荐使用同步上报
 *       (2) 同步上报：payload不为NULL且len不为0时，调用该接口时，HiLink SDK会立即上报该payload
 *       (3) 此处token与凭据无关，仅为此次上报消息的标识符
 *       (4) 此接口当前仅适用于直连云模式
 */
int HILINK_IndicateCharState(const char *svcId, const char *payload, unsigned int payloadLen,
    unsigned short *token);

/*
 * 功能：HiLink SDK 清理资源并退出
 */
void HILINK_Exit(void);

/**
 * @brief 发送device event消息到设备云
 *
 * @param[in] <info> device event消息请求详情
 * @return 0 成功 ; -1 失败
 */
int HILINK_SendDeviceEventToCloud(const CloudMsgInfo *info);

/**
 * @brief 注册接收设备云的device event回复消息
 *
 * @param[in] <cb> 接收回复消息的回调函数
 * @return 0 成功 ; -1 失败
 */
int HILINK_RegRecvSysEventFromCloud(RecvSysEventCb cb);

/**
 * @brief 获取设备的devId
 *
 * @return 返回空字符串则获取失败，当前设备处于未注册绑定状态, 返回的指针不能释放
 */
char *HILINK_GetDeviceID(void);

/**
 * @brief 设备向中枢请求配网注册信息
 *
 * @param regInfoNums [IN] 申请注册信息的总数量
 * @return 正常返回0，异常返回其他错误码
 */
int HILINK_RequestRegInfo(unsigned int regInfoNums);

/**
 * @brief 设备批量配网失败后，切换至SoftAP模式
 *
 * @return 0表示成功，其他表示失败
 */
int HILINK_SetSoftAPMode(void);

/**
 * @brief 设置OTA固件hash签名的填充方式
 *
 * @param mode [IN]
 *             0 - PKCS#1 v1.5（默认方式）
 *             1 - PKCS#1 v2.1
 * @return 0表示成功，其他表示失败
 */
int HILINK_SetOtaSigPaddingMode(unsigned int mode);

/**
 * @brief 设置优先搜索中枢间隔，默认3min
 *
 * @param interval [IN]
 *             单位为秒，取值范围[5,600]
 * @return 设置失败返回-1，成功返回0
 */
int HILINK_SetPrioritySearchCentralInterval(unsigned long interval);

typedef enum {
    EVENT_ID_CENTRAL_INDEPEND_ERROR = 0, /* 协同升级中枢自立异常 */
} EventId;

/**
 * @brief 回调函数
 *
 * @param eventId 通知事件ID
 */
typedef void (*NotifyEventId)(EventId eventId);

/**
 * @brief 注册事件通知回调函数
 *
 * @param cb [IN] 回调函数注册
 *
 * @return 成功返回0 返回其他注册失败
*/
int HILINK_RegNotifyEventIdCallback(NotifyEventId cb);

/**
 * @brief 设备能力注册接口
 *
 * @param pubCap [IN] 输入公共能力
 * @param prvCap [IN] 输入各网关定制的私有能力（功能预埋）
 * @attention 当前协议已有的能力bit0~bit9：
 * bit0：桥能力 bit1：全屋主机能力 bit2：场景本地控能力 bit3：中枢处理跨网关场景拆分能力
 * bit4：中枢处理动态场景拆分能力 bit5：设备是否支持V2版本COAP bit6：批控batchCmd的type1类型中的组合批控groups字段
 * bit7：设备处理去重能力 bit8：删除报文的回复能力 bit9：子设备(比如hiBeacon设备)漫游能力
 * bit10：查询和删除主机上异常设备的能力
 * @return 返回0表示成功，其他错误码异常
 */
int HILINK_RegCapabilitySet(unsigned int pubCap, unsigned int prvCap);

/**
 * @brief 配置端云通道协议类型
 *
 * @param protType [IN] 协议类型，与云端配置一样
 */
void HILINK_SetProtType(const int protType);

typedef enum {
    CERT_UPDATE_TYPE_ADD = 0,
    CERT_UPDATE_TYPE_DEL,
} CertUpdateType;

/**
 * @brief 证书更新回调
 *
 * @param type [IN] 更新类型
 * @param certInfo [IN] 证书信息
 */
typedef void (*CertUpdateCallback)(CertUpdateType type, const char *certInfo);

/**
 * @brief 注册服务证书更新回调
 *
 * @param cb [IN] 回调函数
 * @return 0成功，其他失败
 */
int HILINK_RegServiceCertUpdateCallback(CertUpdateCallback update);

/**
 * @brief 更新对应服务的证书
 *
 * @retval 0成功，成功后通过HILINK_RegServiceCertUpdateCallback注册的回调通知
 * @retval 其他失败
 */
int HILINK_UpdateServiceCert(const char *service);

/**
 * @brief 获取SDK中导入的证书
 *
 * @param service [IN] 服务名
 * @param certInfo [OUT] 证书信息，json格式字符串
 *     {
 *          "serviceName":"xxx", //证书服务名
 *          "fingerprint":"xxx", // 证书指纹
 *          "cert":"xxx", // pem格式证书
 *     }
 * @return 0成功，其他失败
 * @attention certInfo内存需由外部释放
 */
int HILINK_GetServiceCert(const char *service, char **certInfo);

/**
 * @brief 向SDK导入/更新/删除证书
 *
 * @param type [IN] 证书导入类型
 * @param certInfo [IN] 证书信息
 *     {
 *          "serviceName":"xxx", //证书服务名
 *          "fingerprint":"xxx", // 证书指纹
 *          "cert":"xxx", // pem格式证书，删除证书不需要该字段
 *     }
 * @return 0成功，其他失败
 */
int HILINK_ImportServiceCert(CertUpdateType type, const char *certInfo);

typedef struct {
    unsigned char *buffer; /* 未加密的原始coap报文数据 */
    unsigned int len;      /* 未加密的原始coap报文长度 */
} PacketBuffer;

/**
 * @brief 中枢接收SDK的消息回调函数
 *
 * @param PacketBuffer [IN] 接收的未加密的coap报文数据以及长度
 * @return 0 成功 其他失败
 */
typedef int (*CenterRecvPacketCb)(const PacketBuffer *packetBuffer);

/**
 * @brief 中枢发送消息到SDK
 *
 * @param packetBuffer [IN] 发送的二进制明文coap报文以及长度
 * @return 0 成功 其他失败
 */
int HILINK_CenterSendPacket(const PacketBuffer *packetBuffer);

/**
 * @brief 中枢注册接收SDK的消息回调函数
 *
 * @param cb [IN] 接收消息的回调函数
 * @return 0 成功 其他失败
 */
int HILINK_RegCenterRecvPacket(const CenterRecvPacketCb cb);

typedef struct {
    unsigned char method;  /* 直连云：header.code           中枢：payload: {"method":"xxx"} */
    PacketBuffer token;    /* request消息返回token给调用方，response消息token由调用方指定 */
    char *uri;             /* 直连云：COAP_OPTION_URI_PATH  中枢：payload: {"sid":"xxx"} */
    char *query;           /* 直连云：COAP_OPTION_URI_QUERY 中枢：payload: {"query":"xxx"} */
    char *userId;          /* 直连云：COAP_OPTION_USER_ID   中枢：payload: {"userId":"xxx"} */
    char *reqId;           /* 直连云：COAP_OPTION_REQ_ID    中枢：payload: {"reqId":"xxx"} */
    char *payload;         /* 直连云：payload               中枢：payload: {"data":xxx} */
} PacketInfo;

typedef int (*RecvPacketCallback)(const PacketInfo *msg);

/**
 * @brief 注册request消息和response消息接收函数回调
 *
 * @param[in] <uri> 消息uri
 * @param[in] <requestCallback> request消息回调函数
 * @param[in] <responseCallback> response消息回调函数
 * @return 0 成功 ; 其他 失败
 */
int HILINK_RegisterRecvPacketCallback(const char *uri, RecvPacketCallback requestCallback,
    RecvPacketCallback responseCallback);

/**
 * @brief 注册default消息接收函数回调，主要用于{deviceId}/sid等动态uri消息，
 *
 * @param[in] <uri> 消息uri
 * @param[in] <requestCallback> request消息回调函数
 * @param[in] <responseCallback> response消息回调函数
 * @return 0 成功 ; 其他 失败
 */
int HILINK_RegisterRecvReqPacketDefaultCallback(RecvPacketCallback requestCallback);

/**
 * @brief 发送response消息
 *
 * @param[in] <msg> 消息主体，token、reqId等需要指定。
 * @return 0 成功 ; 其他 失败
 */
int HILINK_SendPacketResponse(const PacketInfo *packet);

/**
 * @brief 发送request消息
 *
* @param[in] < passthrough > 中枢是否透传改消息
 * @param[in] <msg> 消息主体，token、reqId等需要指定。
 * @return 0 成功 ; 其他 失败
 */
int HILINK_SendPacketRequest(int passthrough, PacketInfo *packet);

/**
 * @brief SDK基础组件初始化函数
 *
 * @return 成功返回0 返回其他初始化失败
 *
 * @attention SDK基础组件初始化失败，此接口会一直阻塞重试,直到成功退出
 */
int HILINK_Init(void);

/**
 * @brief 启动SDK服务函数
 *
 * @return 成功返回0 返回其他启动服务失败
 *
 */
int HILINK_ServiceStart(void);

/**
 * @brief 获取默认域名接口
 *
 * @param serviceName [IN] 域名类型
 * @return NULL失败，非NULL成功
 * @attention 返回的指针不需要释放
 */
const char *HILINK_GetDefaultUrl(const char *serviceName);

/**
 * @brief 接收GRS服务消息的回调函数
 *
 * @param payload [IN] json格式的grs更新信息
 * @return 0 成功 ; 其他失败
 *
 */
typedef int (*RecvGrsCb)(const char *payload);

/**
 * @brief 注册接收GRS服务的域名更新消息
 *
 * @param cb [IN] 接收GRS服务消息的回调函数
 * @return 0 成功 ; 其他失败
 */
int HILINK_RegRecvGrsCallback(RecvGrsCb cb);

#define TRACE_ID_HEX_STR_LEN 16
#define SPAN_ID_HEX_STR_LEN 8

typedef struct {
    char traceId[TRACE_ID_HEX_STR_LEN + 1];
    char spanId[SPAN_ID_HEX_STR_LEN + 1];
} TraceIdInfo;

/**
 * @brief 获取接收到的traceid
 *
 * @param[out] <out> 输出traceid数据
 *
 * @return 成功返回0 返回其他获取失败
 */
int HILINK_GetRecevieTraceId(TraceIdInfo *out);

/**
 * @brief 设置HiLink SDK打开中枢管控下的协同升级功能，默认为打开状态
 * 该函数由设备开发者或厂商在HILINK_Main函数之前调用一次，不可动态调用
 */
void HILINK_DisableCentralOta(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HILINK_CUSTOM_H */