/**
 * @file cstl_public.h
 * @copyright Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief cstl_public 公共模块对外定义
 * @details cstl公共定义实现源码
 * @date 2021-05-14
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Description
 * <tr><td>2021-05-14  <td>0.1.0    <td>创建初始版本
 * </table>
 * *******************************************************************************************
 */

#ifndef CSTL_PUBLIC_H
#define CSTL_PUBLIC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cpluscplus */

/* ******************************************************************************** */
/* 公共错误码定义                                                                   */
/* ******************************************************************************** */

enum {
    ERRNO_ELEMENT_EMPTY = 1,
    ERRNO_INPUT_INVALID = 2,
    ERRNO_NODE_CREATE_FAIL = 3,
};

/**
* @ingroup cstl_public
* 0x0 正确。
*/
#define CSTL_OK     0

/**
* @ingroup cstl_public
* -1 错误。
*/
#define CSTL_ERROR  (-1)

/**
* @ingroup cstl_public
* CSTL_ERRNO_BASE 错误码定义
*/
#define CSTL_ERRNO_BASE (uint32_t)0x0a03u

/**
* @ingroup cstl_public
* 容器不是NULL但是内容为空，该值为十六进制的0xa030001
*/
#define ERRNO_CSTL_ELEMENT_EMPTY ((CSTL_ERRNO_BASE << 16u) | ((uint32_t)ERRNO_ELEMENT_EMPTY))

/**
* @ingroup cstl_public
* 容器的入参是非法的，该值为十六进制的0xa030002
*/
#define ERRNO_CSTL_INPUT_INVALID ((CSTL_ERRNO_BASE << 16u) | ((uint32_t)ERRNO_INPUT_INVALID))

/**
* @ingroup cstl_public
* 节点创建失败，该值为十六进制的0xa030003
*/
#define ERRNO_CSTL_NODE_CREATE_FAIL ((CSTL_ERRNO_BASE << 16u) | ((uint32_t)ERRNO_NODE_CREATE_FAIL))

/**
 * @ingroup cstl_public
 * @brief 比较函数原型
 * @par 描述：比较函数原型，用于排序场景。
 * @attention 注意：这里只定义了比较函数原型，由于不知道数据类型和长度，因此钩子函数需要业务自己实现。
 * @param data1    [IN] 数据1
 * @param data2    [IN] 数据2
 * @retval >0 升序排序
 * @retval =0 不做交换
 * @retval <0 降序排序
 */
typedef int32_t (*CstlDataCmpFunc)(const void *data1, const void *data2);

/**
 * @ingroup cstl_public
 * @brief 比较函数原型
 * @par 描述：比较函数原型，用于排序场景。
 * @attention 注意：这里只定义了比较函数原型，由于不知道数据类型和长度，因此钩子函数需要业务自己实现。\n
 * 当前源码内有默认的比较函数：该函数不对外提供，但是用户如果不指定默认比较方法会调用它，对此简单解释：
 * 其比较方式为把当前数据转化为有符号数进行比较，即处理含有负数的场景，比较方式为升序。
 * 如果用户需要存储的数据是无符号整数类型。此时排序结果可能不是预期的。
 * 这种场景下的数据比较，用户需要自定义比较函数来解决这种情况的数据比较。
 * 例如对于大数A = uintptr_t(-1) 和 大数 B = 1ULL << 50，目前的函数会认为A < B，实际上A是大于B的。
 * 综上所述，用户对于使用什么样的比较函数，应该根据自己的数据类型来编写(包括降序或其它比较规则)
 * @param key1    [IN] key1
 * @param key2    [IN] key2
 * @retval >0 升序排序
 * @retval =0 不做交换
 * @retval <0 降序排序
 */
typedef int32_t (*CstlKeyCmpFunc)(uintptr_t key1, uintptr_t key2);

/**
 * @ingroup cstl_public
 * @brief 匹配函数原型
 * @par 描述：用于匹配查询场景。
 * @attention 注意：这里只定义了函数原型，由于不知道用户查询匹配机制，因此钩子函数需要业务自己实现。
 * @param node    [IN] 算法结构体节点
 * @param data    [IN] 关键信息
 * @retval true  匹配成功
 * @retval false 匹配失败
 */
typedef bool (*CstlMatchFunc)(const void *node, uintptr_t data);

/**
 * @ingroup cstl_public
 * @brief 用户数据拷贝函数原型
 * @attention 注意：源缓冲区长度需要调用者获取，由于不知道数据类型和长度，因此钩子函数需要业务自己实现。
 * @param ptr    [IN] 指向用户数据的指针
 * @param size   [IN] 用户数据拷贝长度
 * @retval 目标缓冲区，NULL表示失败。
 */
typedef void *(*CstlDupFunc)(void *ptr, size_t size);

/**
 * @ingroup cstl_public
 * @brief 用户内存释放函数原型
 * @par 描述：资源释放函数原型，一般用于机制批量free内存时，内存中可能含有用户私有资源，这是需要用户自行释放
 * @param ptr    [IN] 指向用户数据的指针
 * @retval 无
 */
typedef void (*CstlFreeFunc)(void *ptr);

/**
 * @ingroup cstl_public
 * @brief 用户内存申请函数原型
 * @attention 注意：用户内存申请函数原型，需要用户自己实现。
 * @param size    [IN] 内存申请大小
 * @retval 指向分配内存空间的指针，NULL表示失败
 */
typedef void *(*CstlMallocFunc)(size_t size);

/**
 * @ingroup cstl_public
 * @brief 用户内存申请释放函数对
 * @par 描述：内存申请和释放函数成对出现。
 */
typedef struct {
    CstlMallocFunc mallocFunc;
    CstlFreeFunc freeFunc;
} CstlMallocFreeFuncPair;

/**
 * @ingroup cstl_public
 * @brief key和value函数原型对
 * @par 描述：key和value的拷贝及释放函数成对出现。
 */
typedef struct {
    CstlDupFunc dupFunc;
    CstlFreeFunc freeFunc;
} CstlDupFreeFuncPair;

/**
 * @ingroup cstl_public
 * @brief 该API通过结构的某个成员变量，得到这个结构的起始地址。
 * @par 描述:
 * 该API通过结构的某个成员变量，得到这个结构的起始地址。该API是一个特殊的宏，输入参数取决于宏的实现。
 * @attention
 * @param ptr [IN]  该参数表示结点某成员的地址。取值范围为数据类型。
 * @param type [IN]  该参数表示传入的成员所属的结点类型结构。取值范围为数据类型。
 * @param member [IN]  该参数表示结构中成员变量的名字。取值范围为数据类型。
 * @retval 与入参类型相同结构的地址。
 * @see 无。
*/
#define CSTL_CONTAINER_OF(ptr, type, member) \
    ((type *)((uintptr_t)(ptr) - (uintptr_t)(&(((type *)0)->member))))

#ifdef __cplusplus
}
#endif /* __cpluscplus */

#endif /* CSTL_PUBLIC_H */
