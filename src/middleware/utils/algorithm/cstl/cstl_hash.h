/**
 * @file cstl_hash.h
 * @copyright Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief cstl_hash 对外头文件
 * @details  1、遍历过程中，迭代器指向冲突链中节点的位置；但end迭代器指向最后一个保留的hash桶，该桶无数据。
 *           2、关于(key, value)中的value
 *           a) 如果key、value是整型数据，且长度 <= sizeof(uintptr_t)，则直接记录在node后面紧邻位置；
 *           b）其它场景，key或value位置记录的是指针，指向真正的用户key或用户数据；
 *              此时用户必须注册duplicate函数和free函数对。
 *           3、duplicate函数：用户需要先根据源数据长度申请内存，在把用户数拷贝到申请的内存中去，最后返回分配的内存地址。
 *   +--------+
 *   | 控制块 |
 *   +------- +    iter--->+------+                                       iter-->+------+
 *   |   0    | <-冲突链-> | prev |                                              | prev |
 *   +--------+            | next |   场景1：                                    | next |     场景2:
 *   |   1    |            +------+   key长度<=sizeof(uintptr_t)                 +------+     key长度<sizeof(uintptr_t)
 *   |--------+            | key  |   value长度<=sizeof(uintptr_t)               | key  |     value长度>sizeof(uintptr_t)
 *   |        +            +------+                                              +------+    或字符串或自定义数据结构
 *   |  ...   |            | value|                                              | data |--+
 *   +--------+            +------+                                              | 指针 |  +
 *                                                                               +------+  +
 *                                                                                         +
 *                                                                                         +----->+----------+
 *                                                                                                | userdata |
 *                                                                                                |   area   |
 *                                                                                                +----------+
  *  iter-->+------+                                                     iter-->+------+    场景4:
 *          | prev |                                                            | prev |    key长度>sizeof(uintptr_t)
 *          | next |     场景3:                                                 | next |     或字符串或自定义数据结构
 *          +------+        key长度>sizeof(uintptr_t)或字符串或自定义数据结构   +------+
 *          | key  |---+    value长度<=sizeof(uintptr_t)                        | key  |---+ value长度>sizeof(uintptr_t)
 *          | 指针 |   +                                                        | 指针 |   +  或字符串或自定义数据结构
 *          +------+   +---->+----------+                                       +------+   +---->+----------+
 *          | data |         | userkey  |                                       | data |--+      | userkey  |
 *          +------+         |   area   |                                       | 指针 |  +      |   area   |
 *                           +----------+                                       +------+  +      +----------+
 *                                                                                        +
 *                                                                                        +----->+----------+
 *                                                                                               | userdata |
 *                                                                                               |   area   |
 *                                                                                               +----------+
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

/**
 * @defgroup cstl_hash 哈希表
 * @ingroup cstl
 */

#ifndef CSTL_HASH_H
#define CSTL_HASH_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "cstl_public.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup cstl_hash
 * @brief hash表句柄
 */
typedef struct CstlHashInfo CstlHash;

/**
 * @ingroup cstl_hash
 * @brief hash表迭代器定义，指向hash节点
 */
typedef struct TagHashNode *CstlHashIterator;

/**
 * @ingroup cstl_hash
 * @brief 根据输入的Key生成hash表索引。
 * @param key      [IN] hash key
 * @param bktSize  [IN] hash桶大小
 */
typedef size_t (*CstlHashCodeCalcFunc)(uintptr_t key, size_t bktSize);

/**
 * @ingroup cstl_hash
 * @brief 该函数把输入数据与Key进行匹配比较。第一个哈希表中保存的key，第二个是用户需要匹配的Key。如果不匹配，返回值为false。
 * @param key1     [IN] 哈希表中保存的key
 * @param key2     [IN] 待匹配的key
 * @retval #true key1和key2匹配。
 * @retval #false key1和key2不匹配。
 */
typedef bool (*CstlHashMatchFunc)(uintptr_t key1, uintptr_t key2);

/**
 * @ingroup cstl_hash
 * @brief 默认整型哈希函数。
 * @par 默认整型哈希函数。
 * @attention \n
 *  1.key为用户调用其他接口时的入参。\n
 *  2.若key为整型，用户可在创建hash时将该函数作为hashFunc参数。\n
 * @param key      [IN] 待计算的key。
 * @param bktSize  [IN] 哈希桶大小。
 * @retval #哈希值 根据用户key计算出的哈希值。
 * @par 依赖：无
 * <ul><li>cstl_hash.h：该接口声明所在的头文件。</li></ul>
 */
size_t CstlHashCodeCalcInt(uintptr_t key, size_t bktSize);

/**
 * @ingroup cstl_hash
 * @brief 默认字符串哈希函数。
 * @par 默认字符串哈希函数。
 * @attention \n
 *  1.key为用户调用其他接口时的入参，用户必须保证传入的key为合法的字符串首地址。\n
 *  2.若key为字符串，用户可在创建hash时将该函数作为hashFunc参数。\n
 * @param key      [IN] 待计算的key。
 * @param bktSize  [IN] 哈希桶大小。
 * @retval #哈希值 根据用户key计算出的哈希值。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
size_t CstlHashCodeCalcStr(uintptr_t key, size_t bktSize);

/**
 * @ingroup cstl_hash
 * @brief 默认整型匹配函数。
 * @par 默认整型匹配函数。
 * @attention \n
 *  1.key为用户调用其他接口时的入参。\n
 *  2.若key为整型，用户可在创建hash时将该函数作为matchFunc参数。\n
 * @param key1      [IN] 待匹配的key。
 * @param key2      [IN] 待匹配的key。
 * @retval #true    key1和key2匹配。
 * @retval #false   key1和key2不匹配。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
bool CstlHashMatchInt(uintptr_t key1, uintptr_t key2);

/**
 * @ingroup cstl_hash
 * @brief 默认字符串匹配函数。
 * @par 默认字符串匹配函数。
 * @attention \n
 *  1.key1为用户调用其他接口时的入参，用户必须保证传入的key1为合法的字符串首地址。\n
 *  2.若key为字符串，用户可在创建hash时将该函数作为matchFunc参数。\n
 * @param key1      [IN] 待匹配的key。
 * @param key2      [IN] 待匹配的key。
 * @retval #true    key1和key2匹配。
 * @retval #false   key1和key2不匹配。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
bool CstlHashMatchStr(uintptr_t key1, uintptr_t key2);

/**
 * @ingroup cstl_hash
 * @brief 创建一个Hash表，返回Hash表的句柄
 * @attention \n
 * 1、关于key和data的拷贝函数：\n
 * 如下场景不需要注册拷贝函数：如果是int型数据，且长度<=sizeof(uintptr_t)。\n
 * 其它场景必须注册拷贝函数：\n
 * a）是int型数据，但长度 >sizeof(uintptr_t)；\n
 * b）字符串；\n
 * c）用户自定义数据结构；\n
 * 2、关于free函数：简单来说，如果注册了duplicate函数，就必须注册相应的free函数。 \n
 * 3、提供默认的整型、字符串哈希函数：#CstlHashCodeCalcInt、#CstlHashCodeCalcStr。 \n
 * 4、提供默认的整型、字符串匹配函数：#CstlHashMatchInt、#CstlHashMatchStr。 \n
 * @param bktSize       [IN] hash桶的个数
 * @param hashCalcFunc  [IN] hash值计算函数。如果为NULL，则默认key为整型，使用#CstlHashCodeCalcInt。
 * @param matchFunc     [IN] hash key匹配函数。如为NULL，则默认key为整型，使用#CstlHashMatchInt。
 * @param keyFunc       [IN] hash key拷贝及释放函数对，如果用户未注册keyFunc->dupFunc，则默认为key为整型。
 * @param valueFunc      [IN] hash data拷贝及释放函数对。如果用户未注册valueFunc->dupFunc，则默认为data为整型。
 * @retval hash表句柄。NULL表示创建失败。
 * @par 依赖：无
 * @see #CstlHashCodeCalcInt、#CstlHashCodeCalcStr、#CstlHashMatchInt、#CstlHashMatchStr。
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
CstlHash *CstlHashCreate(size_t bktSize,
                         CstlHashCodeCalcFunc hashFunc,
                         CstlHashMatchFunc matchFunc,
                         CstlDupFreeFuncPair *keyFunc,
                         CstlDupFreeFuncPair *valueFunc);

/**
 * @ingroup cstl_hash
 * @brief 插入hash数据
 * @par 描述：创建节点，把数据(key、value)插入hash表
 * @attention
 *  1.不支持重复key。\n
 *  2.key和value为整型值或指向用户key或value的地址。\n
 *  3.如果扩展数据的生命周期小于节点的生命周期，则需要在创建哈希表时注册拷贝函数及释放函数。\n
 * @param hash          [IN] hash表的句柄。
 * @param key           [IN] key或保存key的地址。
 * @param keySize       [IN] key拷贝长度，若用户未注册dupFunc，该参数将不被使用
 * @param value         [IN] value或保存value的地址。
 * @param valueSize     [IN] value拷贝长度，若用户未注册dupFunc，该参数将不被使用
 * @retval #CSTL_OK      插入节点成功。
 * @retval #CSTL_ERROR   插入失败。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
int32_t CstlHashInsert(CstlHash *hash, uintptr_t key, size_t keySize, uintptr_t value, size_t valueSize);

/**
 * @ingroup cstl_hash
 * @brief 插入或更新hash数据
 * @par 描述：该接口用于将不存在的key插入到哈希表或更新已存在的key对应的value。
 * @attention
 *  1.支持重复key。\n
 *  2.当key不存在时，该接口的使用与#CstlHashInsert保持一致。\n
 *  3.当key存在时，该接口会更新value。\n
 * @param hash          [IN] hash表的句柄。
 * @param key           [IN] key或保存key的地址。
 * @param keySize       [IN] key拷贝长度，若用户未注册dupFunc，该参数将不被使用
 * @param value         [IN] value或保存value的地址。
 * @param valueSize     [IN] value拷贝长度，若用户未注册dupFunc，该参数将不被使用
 * @retval #CSTL_OK     插入或更新节点成功。
 * @retval #CSTL_ERROR  插入或更新节点失败。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
int32_t CstlHashPut(CstlHash *hash, uintptr_t key, size_t keySize, uintptr_t value, size_t valueSize);

/**
 * @ingroup cstl_hash
 * @brief 查找节点，返回节点数据。
 * @par 描述: 根据key查找并返回节点数据。
 * @param hash          [IN]  hash表的句柄。
 * @param key           [IN]  key或保存key的地址。
 * @param value         [OUT] 查到的数据。
 * @retval #CSTL_OK     查找成功。
 * @retval #CSTL_ERROR  查找失败。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
int32_t CstlHashAt(const CstlHash *hash, uintptr_t key, uintptr_t *value);

/**
 * @ingroup cstl_hash
 * @brief 查找key所在迭代器。
 * @par 描述: 根据key查找并返回key所在的迭代器。
 * @param hash   [IN] hash表的句柄。
 * @param key    [IN] key或保存key的地址。
 * @retval key存在时，返回key所在迭代器（指向Node所在地址）,其他情况返回#CstlHashIterEnd()。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
CstlHashIterator CstlHashFind(const CstlHash *hash, uintptr_t key);

/**
 * @ingroup cstl_hash
 * @brief 判断当前HASH表是否为空。
 * @par 描述: 判断当前HASH表是否为空，为空返回true，否则返回false。
 * @param  hash [IN] hash表句柄。取值范围为有效指针。
 * @retval #true， 表示HASH表为空。
 * @retval #false，表示HASH表非空。
 * @see #CstlHashSize
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
bool CstlHashEmpty(const CstlHash *hash);

/**
 * @ingroup cstl_hash
 * @brief 获取HASH表的节点数量。
 * @par 描述: 用于获取HASH表的节点数量，返回节点个数。
 * @param  hash [IN] hash表句柄。取值范围为有效指针。
 * @retval hash节点数。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
size_t CstlHashSize(const CstlHash *hash);

/**
 * @ingroup cstl_hash
 * @brief 从hash表中移除指定结点。
 * @par 描述: 根据key查找到节点并删除（释放内存），同时释放相应的节点内存。
 * @param hash [IN] hash表句柄。取值范围为有效指针。
 * @param key   [IN] 移除节点key。
 * @retval key存在时，返回key所在迭代器的下一个迭代器（指向Node所在地址）,其他则返回#CstlHashIterEnd()。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
CstlHashIterator CstlHashErase(CstlHash *hash, uintptr_t key);

/**
 * @ingroup cstl_hash
 * @brief 删除hash表所有节点。
 * @par 描述：删除所有节点，回收节点内存（hash表还在，只是没有成员）。
 * @attention 注意：如果用户数据中有私有资源，则需要在创建时注册free钩子函数。
 * @param  hash [IN] hash表句柄。
 * @retval 无。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
void CstlHashClear(CstlHash *hash);

/**
 * @ingroup cstl_hash
 * @brief 删除hash表
 * @par 描述：删除hash表，如里面有节点先删除节点，回收内存。
 * @attention 注意：如果用户数据中有私有资源，则需要在创建时注册free钩子函数。
 * @param  hash [IN] hash表句柄。
 * @retval 无。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
void CstlHashDestory(CstlHash *hash);

/**
 * @ingroup cstl_hash
 * @brief 获取hash表中的第一个节点的迭代器。
 * @par 描述：获取hash表中的第一个节点所在的迭代器。
 * @param hash  [IN]  hash表的句柄。
 * @retval 第一个节点迭代器，若hash为空则返回#CstlHashIterEnd()。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
CstlHashIterator CstlHashIterBegin(const CstlHash *hash);

/**
 * @ingroup cstl_hash
 * @brief 获取hash表中最后一个节点之后预留的迭代器。
 * @par 描述：获取hash表中的最后一个节点之后预留的迭代器。该节点指向最后保留的hash桶，该桶没有任何成员。
 * @param hash  [IN]  hash表的句柄。
 * @retval 最后一个节点之后预留的迭代器。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
CstlHashIterator CstlHashIterEnd(const CstlHash *hash);

/**
 * @ingroup cstl_hash
 * @brief 获取hash表的下一个节点迭代器。
 * @par 描述：获取hash表下一个节点迭代器。
 * @param hash     [IN]  hash表的句柄。
 * @param it       [IN]  当前迭代器。
 * @retval 下一个节点迭代器。若当前节点已是最后一个迭代器则返回#CstlHashIterEnd()。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
CstlHashIterator CstlHashIterNext(const CstlHash *hash, CstlHashIterator it);

/**
 * @ingroup cstl_hash
 * @brief 获取迭代器的key。
 * @par 描述：获取hash表中迭代器当前key。
 * @attention \n
 *  1.当hash为空指针或it等于#CstlHashIterEnd()时，接口返回0，该接口无法区分是错误码还是用户数据，
 *    用户调用该接口时必须保证hash为合法指针，并且it不等于#CstlHashIterEnd()
 * @param hash     [IN]  hash表的句柄。
 * @param it       [IN]  当前迭代器。
 * @retval 迭代器对应的key。it等于#CstlHashIterEnd()时则返回0。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
uintptr_t CstlHashIterKey(const CstlHash *hash, CstlHashIterator it);

/**
 * @ingroup cstl_hash
 * @brief 获取迭代器的value。
 * @par 描述：获取hash表中迭代器当前key。
 * @attention \n
 *  1.当hash为空指针或it等于#CstlHashIterEnd()时，接口返回0，该接口无法区分是错误码还是用户数据，
 *    用户调用该接口时必须保证hash为合法指针，并且it不等于#CstlHashIterEnd()
 * @param hash     [IN]  hash表的句柄。
 * @param it       [IN]  当前迭代器。
 * @retval 迭代器对应的value。it等于#CstlHashIterEnd()时则返回0。
 * @par 依赖：无
 * @li cstl_hash.h：该接口声明所在的头文件.
 */
uintptr_t CstlHashIterValue(const CstlHash *hash, CstlHashIterator it);

#ifdef __cplusplus
}
#endif

#endif /* CSTL_HASH_H */

