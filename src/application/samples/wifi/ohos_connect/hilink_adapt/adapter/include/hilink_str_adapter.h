/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: 系统适配层字符串接口（此文件为DEMO，需集成方适配修改）
 */
#ifndef HILINK_STR_ADAPTER_H
#define HILINK_STR_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 描述: 用于计算字符串的长度
 * 参数: src，待计算长度的字符串
 * 返回: 字符串长度
 */
unsigned int HILINK_Strlen(const char *src);

/*
 * 描述: 用于在字符串str中查找字符ch
 * 参数: str，待查找字符串
 *       ch，待查找字符
 * 返回: NULL没有查找到字符，非NULL指向ch的指针
 */
char *HILINK_Strchr(const char *str, int ch);

/*
 * 描述: 在字符串str中逆向查找字符ch
 * 参数: str，字符串
 *       c，待查找字符
 * 返回: NULL没有查找到字符，非NULL指向ch的指针
 */
char *HILINK_Strrchr(const char *str, int ch);

/*
 * 描述: 把字符串转换成int整形数字
 * 参数: str，传入需要转换成 int 类型字符串
 * 返回: 字符串转换成的int整形数字
 */
int HILINK_Atoi(const char *str);

/*
 * 描述: 在str1中查找是否存在str2
 * 参数: str1，被查找目标
 *       str2，要查找对象
 * 返回: NULL不存在，非NULLstr2在str1中首次出现位置的指针
 */
char *HILINK_Strstr(const char *str1, const char *str2);

/*
 * 描述: 比较两个字符串str1和str2
 * 参数: str1，目标字符串1
 *       str2，目标字符串2
 * 返回: 0成功，小于0表示str1小于str2, 大于0表示str1大于str2
 */
int HILINK_Strcmp(const char *str1, const char *str2);

/*
 * 描述: 比较两个字符串str1和str2
 * 参数: str1，目标字符串1
 *       str2，目标字符串2
 *       len，比较的长度
 * 返回: 0成功，小于0表示str1小于str2, 大于0表示str1大于str2
 */
int HILINK_Strncmp(const char *str1, const char *str2, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* HILINK_STR_ADAPTER_H */
