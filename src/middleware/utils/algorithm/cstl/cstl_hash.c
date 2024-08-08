/**
 * @file cstl_hash.c
 * @copyright Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief cstl_hash 实现源码
 * @details 哈希表实现源码
 * @date 2021-05-14
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * </table>
 * *******************************************************************************************
 */

#include "cstl_rawlist.h"
#include "securec.h"
#include "cstl_public_inner.h"
#include "cstl_hash.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CSTL_HASH_OPTION3 3
#define CSTL_HASH_OPTION2 2
#define CSTL_HASH_OPTION1 1

struct TagHashNode {
    CstlRawListNode  node;            /**< 链表node */
    uintptr_t        key;             /**< key或保存key的地址 */
    uintptr_t        value;           /**< value或保存value的地址 */
};

typedef struct TagHashNode CstlHashNode;

struct CstlHashInfo {
    CstlDupFreeFuncPair keyFunc;     /**< key拷贝、释放函数对 */
    CstlDupFreeFuncPair valueFunc;   /**< value拷贝、释放函数对 */
    CstlHashMatchFunc matchFunc;     /**< 匹配函数 */
    CstlHashCodeCalcFunc hashFunc;   /**< 哈希函数 */
    size_t bucketSize;               /**< Hash表大小 */
    size_t hashCount;                /**< Hash表中表项的个数 */
    CstlRawList listArray[0];        /**< 链表控制块数组 */
};

/* murmurhash算法 */
/* 定义常量 */
#define HASH_VC1 0xCC9E2D51
#define HASH_VC2 0x1B873593
#define HASH_HC1 0xE6546B64
#define HASH_HC2 0x85EBCA6B
#define HASH_HC3 0xC2B2AE35
#define HASH_HC4 5

#define CHAR_BIT 8
#define CHAR_FOR_PER_LOOP 4
#define HASH_V_ROTATE 15
#define HASH_H_ROTATE 13
#define SYS_BUS_WIDTH sizeof(size_t)
#define HASH_SEED 0x3B9ACA07        /* 大质数1000000007，种子可以随机也可以指定 */

enum CstlByte {
    ONE_BYTE = 1,
    TWO_BYTE = 2,
};

enum CstlShiftBit {
    SHIFT8 = 8,
    SHIFT13 = 13,
    SHIFT16 = 16,
    SHIFT24 = 24
};

CstlHashIterator CstlHashPrev(const CstlHash *hash, CstlHashIterator hashNode);

CSTL_STATIC size_t CstlHashRotate(size_t v, uint32_t offset)
{
    return ((v << offset) | (v >> (SYS_BUS_WIDTH * CHAR_BIT - offset)));
}

CSTL_STATIC size_t CstlHashMixV(size_t v)
{
    v = v * HASH_VC1;
    v = CstlHashRotate(v, HASH_V_ROTATE);
    v = v * HASH_VC2;

    return v;
}

CSTL_STATIC size_t CstlHashMixH(size_t h, size_t v)
{
    size_t res = h;

    res ^= v;
    res = CstlHashRotate(res, HASH_H_ROTATE);
    res = res * HASH_HC4 + HASH_HC1;

    return res;
}

CSTL_STATIC size_t CstlHashCodeCalc(void *key, size_t keySize)
{
    uint8_t *tmpKey = (uint8_t *)key;
    size_t i = 0;
    size_t v;
    size_t h = HASH_SEED;
    uint8_t c0, c1, c2, c3;
    size_t tmpLen = keySize - keySize % CHAR_FOR_PER_LOOP;

    while ((i + CHAR_FOR_PER_LOOP) <= tmpLen) {
        c0 = tmpKey[i++];
        c1 = tmpKey[i++];
        c2 = tmpKey[i++];
        c3 = tmpKey[i++];

        v = (size_t)c0 | ((size_t)c1 << SHIFT8) |
            ((size_t)c2 << SHIFT16) | ((size_t)c3 << SHIFT24);
        v = CstlHashMixV(v);
        h = CstlHashMixH(h, v);
    }

    v = 0;

    switch (keySize & CSTL_HASH_OPTION3) {
        case CSTL_HASH_OPTION3:
            v ^= ((size_t)tmpKey[i + TWO_BYTE] << SHIFT16);
            /* (keySize % 4)等于3，fallthrough，其他分支类似。 */
            /* fall-through */
        case CSTL_HASH_OPTION2:
            v ^= ((size_t)tmpKey[i + ONE_BYTE] << SHIFT8);
            /* fall-through */
        case CSTL_HASH_OPTION1:
            v ^= tmpKey[i];
            v = CstlHashMixV(v);
            h ^= v;
            break;
        default:
            break;
    }

    h ^= h >> SHIFT16;

    h *= HASH_HC2;
    h ^= h >> SHIFT13;
    h *= HASH_HC3;
    h ^= h >> SHIFT16;

    return h;
}

/* 内部接口定义 */
CSTL_STATIC void CstlHashHookRegister(CstlHash *hash,
                                      CstlHashCodeCalcFunc hashFunc,
                                      CstlHashMatchFunc matchFunc,
                                      CstlDupFreeFuncPair *keyFunc,
                                      CstlDupFreeFuncPair *valueFunc)
{
    CstlDupFreeFuncPair *hashKeyFunc = &hash->keyFunc;
    CstlDupFreeFuncPair *hashValueFunc = &hash->valueFunc;

    if (hashFunc == NULL) {
        hash->hashFunc = CstlHashCodeCalcInt;
    } else {
        hash->hashFunc = hashFunc;
    }

    if (matchFunc == NULL) {
        hash->matchFunc = CstlHashMatchInt;
    } else {
        hash->matchFunc = matchFunc;
    }

    if (keyFunc == NULL) {
        hashKeyFunc->dupFunc = NULL;
        hashKeyFunc->freeFunc = NULL;
    } else {
        hashKeyFunc->dupFunc = keyFunc->dupFunc;
        hashKeyFunc->freeFunc = keyFunc->freeFunc;
    }

    if (valueFunc == NULL) {
        hashValueFunc->dupFunc = NULL;
        hashValueFunc->freeFunc = NULL;
    } else {
        hashValueFunc->dupFunc = valueFunc->dupFunc;
        hashValueFunc->freeFunc = valueFunc->freeFunc;
    }
}

CSTL_STATIC inline CstlHashIterator CstlHashIterEndGet(const CstlHash *hash)
{
    return (CstlHashIterator)(uintptr_t)(&hash->listArray[hash->bucketSize].head);
}

CSTL_STATIC CstlHashNode *CstlHashNodeCreate(const CstlHash *hash, uintptr_t key, size_t keySize,
                                             uintptr_t value, size_t valueSize)
{
    uintptr_t tmpKey;
    uintptr_t tmpValue;
    CstlHashNode *hashNode;
    void *tmpPtr;

    hashNode = (CstlHashNode *)malloc(sizeof(CstlHashNode));
    if (hashNode == NULL) {
        CSTL_PRINTF("[Cstlhash]: malloc failed. File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return NULL;
    }

    if (hash->keyFunc.dupFunc != NULL) {
        tmpPtr = hash->keyFunc.dupFunc((void *)key, keySize);
        tmpKey = (uintptr_t)tmpPtr;
        if (tmpKey == (uintptr_t)NULL) {
            /* 考虑用户返回失败的场景 */
            free(hashNode);
            return NULL;
        }
    } else {
        tmpKey = key;
    }

    if (hash->valueFunc.dupFunc != NULL) {
        tmpPtr = hash->valueFunc.dupFunc((void *)value, valueSize);
        tmpValue = (uintptr_t)tmpPtr;
        if (tmpValue == (uintptr_t)NULL) {
            /* 考虑用户返回失败的场景 */
            if (hash->keyFunc.freeFunc != NULL) {
                hash->keyFunc.freeFunc((void *)tmpKey);
            }

            free(hashNode);
            return NULL;
        }
    } else {
        tmpValue = value;
    }

    hashNode->key = tmpKey;
    hashNode->value = tmpValue;

    return hashNode;
}

CSTL_STATIC CstlHashNode *CstlHashFindNode(const CstlRawList *list, uintptr_t key, CstlHashMatchFunc matchFunc)
{
    CstlHashNode *hashNode;
    CstlRawListNode *rawListNode;

    for (rawListNode = CstlRawListFront(list); rawListNode != NULL; rawListNode = CstlRawListNext(list, rawListNode)) {
        hashNode = CSTL_CONTAINER_OF(rawListNode, CstlHashNode, node);
        if (matchFunc(hashNode->key, key)) {
            return hashNode;
        }
    }

    return NULL;
}

CSTL_STATIC CstlHashIterator CstlHashFront(const CstlHash *hash)
{
    uint32_t i = 0;
    const CstlRawList *list;
    CstlRawListNode *rawListNode;

    while (i < hash->bucketSize) {
        list = &hash->listArray[i];
        rawListNode = CstlRawListFront(list);
        if (rawListNode != NULL) {
            return CSTL_CONTAINER_OF(rawListNode, CstlHashNode, node);
        }

        i++;
    }

    return CstlHashIterEndGet(hash);
}

CSTL_STATIC CstlHashIterator CstlHashNext(const CstlHash *hash, CstlHashIterator hashNode)
{
    size_t i;
    size_t hashCode;
    const CstlRawList *list;
    CstlRawListNode *rawListNode;

    hashCode = hash->hashFunc(hashNode->key, hash->bucketSize);
    if (hashCode >= hash->bucketSize) {
        CSTL_PRINTF("[Cstlhash]: hashCode is invalid.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return CstlHashIterEndGet(hash);
    }

    list = hash->listArray + hashCode;
    rawListNode = CstlRawListNext(list, &hashNode->node);
    if (rawListNode != NULL) {
        return CSTL_CONTAINER_OF(rawListNode, CstlHashNode, node);
    }

    for (i = hashCode + 1; i < hash->bucketSize; ++i) {
        list = &hash->listArray[i];
        rawListNode = CstlRawListFront(list);
        if (rawListNode != NULL) {
            return CSTL_CONTAINER_OF(rawListNode, CstlHashNode, node);
        }
    }

    return CstlHashIterEndGet(hash);
}

CstlHashIterator CstlHashPrev(const CstlHash *hash, CstlHashIterator hashNode)
{
    ssize_t i;
    size_t hashCode;
    const CstlRawList *list;
    CstlRawListNode *rawListNode;

    hashCode = hash->hashFunc(hashNode->key, hash->bucketSize);
    if (hashCode >= hash->bucketSize) {
        CSTL_PRINTF("[Cstlhash]: hashCode is invalid.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return CstlHashIterEndGet(hash);
    }

    list = hash->listArray + hashCode;
    rawListNode = CstlRawListPrev(list, &hashNode->node);
    if (rawListNode != NULL) {
        return CSTL_CONTAINER_OF(rawListNode, CstlHashNode, node);
    }

    for (i = (ssize_t)(hashCode - 1); i >= 0; --i) {
        list = &hash->listArray[i];
        rawListNode = CstlRawListBack(list);
        if (rawListNode != NULL) {
            return CSTL_CONTAINER_OF(rawListNode, CstlHashNode, node);
        }
    }

    return CstlHashIterEndGet(hash);
}

CSTL_STATIC void CstlHashNodeFree(CstlHash *hash, CstlHashNode *node)
{
    CstlFreeFunc keyFreeFunc = hash->keyFunc.freeFunc;
    CstlFreeFunc valueFreeFunc = hash->valueFunc.freeFunc;

    if (keyFreeFunc != NULL) {
        keyFreeFunc((void *)node->key);
    }

    if (valueFreeFunc != NULL) {
        valueFreeFunc((void *)node->value);
    }

    free(node);
}

__attribute__((visibility("default"))) size_t CstlHashCodeCalcInt(uintptr_t key, size_t bktSize)
{
    size_t hashCode = CstlHashCodeCalc(&key, sizeof(key));

    return hashCode % bktSize;
}

__attribute__((visibility("default"))) bool CstlHashMatchInt(uintptr_t key1, uintptr_t key2)
{
    return key1 == key2;
}

__attribute__((visibility("default"))) size_t CstlHashCodeCalcStr(uintptr_t key, size_t bktSize)
{
    char *tmpKey = (char *)key;
    size_t hashCode = CstlHashCodeCalc(tmpKey, strlen(tmpKey));

    return hashCode % bktSize;
}

__attribute__((visibility("default"))) bool CstlHashMatchStr(uintptr_t key1, uintptr_t key2)
{
    char *tkey1 = (char *)key1;
    char *tkey2 = (char *)key2;

    if (strcmp(tkey1, tkey2) == 0) {
        return true;
    }

    return false;
}

__attribute__((visibility("default"))) CstlHash *CstlHashCreate(size_t bktSize,
                                                                CstlHashCodeCalcFunc hashFunc,
                                                                CstlHashMatchFunc matchFunc,
                                                                CstlDupFreeFuncPair *keyFunc,
                                                                CstlDupFreeFuncPair *valueFunc)
{
    size_t i;
    size_t size;
    CstlHash *hash;
    CstlRawList *listAddr;
    if (bktSize == 0U) {
        CSTL_PRINTF("[Cstlhash]: bktSize is 0.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return NULL;
    }

    size = (bktSize + 1) * sizeof(CstlRawList);
    if (IsMultiOverflow((bktSize + 1), sizeof(CstlRawList)) || IsAddOverflow(size, sizeof(CstlHash))) {
        CSTL_PRINTF("[Cstlhash]: bktSize is too big.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return NULL;
    }

    size += sizeof(CstlHash);
    hash = (CstlHash *)malloc(size);
    if (hash == NULL) {
        CSTL_PRINTF("[Cstlhash]: malloc failed.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return NULL;
    }

    (void)memset_s(hash, size, 0, size);
    hash->bucketSize = bktSize;
    CstlHashHookRegister(hash, hashFunc, matchFunc, keyFunc, valueFunc);

    listAddr = hash->listArray;
    for (i = 0; i <= bktSize; ++i) {
        CstlRawListInit(listAddr + i, NULL);
    }

    return hash;
}

CSTL_STATIC int32_t CstlHashInsertNode(CstlHash *hash, CstlRawList *rawList,
                                       const CstlUserData *inputKey, const CstlUserData *inputValue)
{
    CstlHashNode *hashNode =
        CstlHashNodeCreate(hash, inputKey->inputData, inputKey->dataSize, inputValue->inputData, inputValue->dataSize);
    if (hashNode == NULL) {
        CSTL_PRINTF("[Cstlhash]: hashNode create failed.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return CSTL_ERROR;
    }

    CstlRawListPushBack(rawList, &hashNode->node);
    hash->hashCount++;

    return CSTL_OK;
}

static int32_t CstlHashUpdateNode(CstlHash *hash, CstlHashNode *node, uintptr_t value, size_t valueSize)
{
    uintptr_t tmpValue;
    void *tmpPtr;

    if (hash->valueFunc.dupFunc != NULL) {
        tmpPtr = hash->valueFunc.dupFunc((void *)value, valueSize);
        tmpValue = (uintptr_t)tmpPtr;
        if (tmpValue == (uintptr_t)NULL) {
            /* 考虑用户返回失败的场景 */
            return CSTL_ERROR;
        }

        if (hash->valueFunc.freeFunc != NULL) {
            hash->valueFunc.freeFunc((void *)node->value);
        }
    } else {
        tmpValue = value;
    }

    node->value = tmpValue;

    return CSTL_OK;
}

CSTL_STATIC inline int32_t CstlHashCodeCheck(const CstlHash *hash, uintptr_t key, size_t *hashCode)
{
    if (hash == NULL) {
        CSTL_PRINTF("[Cstlhash]: hash is NULL.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return CSTL_ERROR;
    }

    *hashCode = hash->hashFunc(key, hash->bucketSize);
    if (*hashCode >= hash->bucketSize) {
        CSTL_PRINTF("[Cstlhash]: hashCode is invalid.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return CSTL_ERROR;
    }

    return CSTL_OK;
}

__attribute__((visibility("default"))) int32_t CstlHashInsert(CstlHash *hash, uintptr_t key, size_t keySize,
                                                              uintptr_t value, size_t valueSize)
{
    int32_t ret;
    size_t hashCode;
    CstlHashNode *hashNode;
    CstlRawList *rawList;
    CstlUserData inputKey;
    CstlUserData inputValue;

    ret = CstlHashCodeCheck(hash, key, &hashCode);
    if (ret != CSTL_OK) {
        return CSTL_ERROR;
    }

    rawList = &hash->listArray[hashCode];
    hashNode = CstlHashFindNode(rawList, key, hash->matchFunc);
    if (hashNode != NULL) {
        CSTL_PRINTF("[Cstlhash]: key is exist.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return CSTL_ERROR;
    }

    inputKey.inputData = key;
    inputKey.dataSize = keySize;
    inputValue.inputData = value;
    inputValue.dataSize = valueSize;

    return CstlHashInsertNode(hash, rawList, &inputKey, &inputValue);
}

__attribute__((visibility("default"))) int32_t CstlHashPut(CstlHash *hash, uintptr_t key, size_t keySize,
                                                           uintptr_t value, size_t valueSize)
{
    int32_t ret;
    size_t hashCode;
    CstlRawList *rawList;
    CstlHashNode *hashNode;
    CstlUserData inputKey;
    CstlUserData inputValue;

    ret = CstlHashCodeCheck(hash, key, &hashCode);
    if (ret != CSTL_OK) {
        return CSTL_ERROR;
    }

    rawList = &hash->listArray[hashCode];
    hashNode = CstlHashFindNode(rawList, key, hash->matchFunc);
    if (hashNode != NULL) {
        return CstlHashUpdateNode(hash, hashNode, value, valueSize);
    }

    inputKey.inputData = key;
    inputKey.dataSize = keySize;
    inputValue.inputData = value;
    inputValue.dataSize = valueSize;

    return CstlHashInsertNode(hash, rawList, &inputKey, &inputValue);
}

__attribute__((visibility("default"))) int32_t CstlHashAt(const CstlHash *hash, uintptr_t key, uintptr_t *value)
{
    CstlHashNode *hashNode = CstlHashFind(hash, key);

    if (hashNode == CstlHashIterEndGet(hash)) {
        return CSTL_ERROR;
    }

    *value = hashNode->value;

    return CSTL_OK;
}

__attribute__((visibility("default"))) CstlHashIterator CstlHashFind(const CstlHash *hash, uintptr_t key)
{
    int32_t ret;
    size_t hashCode;
    CstlHashNode *hashNode;

    ret = CstlHashCodeCheck(hash, key, &hashCode);
    if (ret != CSTL_OK) {
        return hash == NULL ? NULL : CstlHashIterEndGet(hash);
    }

    hashNode = CstlHashFindNode(&hash->listArray[hashCode], key, hash->matchFunc);
    if (hashNode == NULL) {
        CSTL_PRINTF("[Cstlhash]: key is not exist.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return CstlHashIterEndGet(hash);
    }

    return hashNode;
}

__attribute__((visibility("default"))) bool CstlHashEmpty(const CstlHash *hash)
{
    if ((hash == NULL) || (hash->hashCount == 0U)) {
        return true;
    }

    return false;
}

__attribute__((visibility("default"))) size_t CstlHashSize(const CstlHash *hash)
{
    if (hash == NULL) {
        return 0;
    }

    return hash->hashCount;
}

__attribute__((visibility("default"))) CstlHashIterator CstlHashErase(CstlHash *hash, uintptr_t key)
{
    size_t hashCode;
    CstlHashNode *hashNode;
    CstlHashNode *nextHashNode;
    CstlHashMatchFunc matchFunc;
    CstlHashCodeCalcFunc hashFunc;

    if (hash == NULL) {
        CSTL_PRINTF("[Cstlhash]: hash is NULL.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return NULL;
    }

    hashFunc = hash->hashFunc;
    hashCode = hashFunc(key, hash->bucketSize);
    if (hashCode >= hash->bucketSize) {
        CSTL_PRINTF("[Cstlhash]: hashCode is invalid.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return CstlHashIterEndGet(hash);
    }

    matchFunc = hash->matchFunc;
    hashNode = CstlHashFindNode(&hash->listArray[hashCode], key, matchFunc);
    if (hashNode == NULL) {
        return CstlHashIterEndGet(hash);
    }

    nextHashNode = CstlHashNext(hash, hashNode);
    CstlRawListErase(&hash->listArray[hashCode], &hashNode->node);
    CstlHashNodeFree(hash, hashNode);
    --hash->hashCount;

    return nextHashNode;
}

__attribute__((visibility("default"))) void CstlHashClear(CstlHash *hash)
{
    uint32_t i;
    CstlRawList *list;
    CstlHashNode *hashNode;
    CstlRawListNode *rawListNode;

    if (hash == NULL) {
        CSTL_PRINTF("[Cstlhash]: hash is NULL.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return;
    }

    for (i = 0; i < hash->bucketSize; ++i) {
        list = &hash->listArray[i];
        while (!CstlRawListEmpty(list)) {
            rawListNode = CstlRawListFront(list);
            hashNode = CSTL_CONTAINER_OF(rawListNode, CstlHashNode, node);
            CstlRawListErase(list, rawListNode);
            CstlHashNodeFree(hash, hashNode);
        }
    }

    hash->hashCount = 0;
}

__attribute__((visibility("default"))) void CstlHashDestory(CstlHash *hash)
{
    if (hash == NULL) {
        CSTL_PRINTF("[Cstlhash]: hash is NULL.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return;
    }

    CstlHashClear(hash);
    free(hash);
}

__attribute__((visibility("default"))) CstlHashIterator CstlHashIterBegin(const CstlHash *hash)
{
    if (hash == NULL) {
        CSTL_PRINTF("[Cstlhash]: hash is NULL.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return NULL;
    }

    return CstlHashFront(hash);
}

__attribute__((visibility("default"))) CstlHashIterator CstlHashIterEnd(const CstlHash *hash)
{
    if (hash == NULL) {
        CSTL_PRINTF("[Cstlhash]: hash is NULL.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return NULL;
    }

    return CstlHashIterEndGet(hash);
}

__attribute__((visibility("default"))) CstlHashIterator CstlHashIterNext(const CstlHash *hash, CstlHashIterator it)
{
    if ((hash == NULL) || (it == CstlHashIterEnd(hash))) {
        CSTL_PRINTF("[Cstlhash]: hash is NULL.  File = %s, line = %d.\r\n", __FILE__, __LINE__);
        return CstlHashIterEnd(hash);
    }

    return CstlHashNext(hash, it);
}

__attribute__((visibility("default"))) uintptr_t CstlHashIterKey(const CstlHash *hash, CstlHashIterator it)
{
    if (it == CstlHashIterEnd(hash)) {
        return 0;
    }

    return it->key;
}

__attribute__((visibility("default"))) uintptr_t CstlHashIterValue(const CstlHash *hash, CstlHashIterator it)
{
    if (it == CstlHashIterEnd(hash)) {
        return 0;
    }

    return it->value;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */