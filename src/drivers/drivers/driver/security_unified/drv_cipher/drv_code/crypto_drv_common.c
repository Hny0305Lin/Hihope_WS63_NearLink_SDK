/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides cipher driver common utils. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#include "drv_common.h"
#include "drv_trng.h"
#include "crypto_drv_common.h"

/* crypto dump functions. */
#define MAX_DUMP_LENGHT     (512)
#define BYTES_IN_ONE_LINE   (16)
#ifndef KERN_CONT
#define KERN_CONT
#endif

static crypto_drv_func g_drv_func = {
    .malloc_coherent = TD_NULL,
    .free_coherent = TD_NULL,
    .get_phys_addr = TD_NULL
};

td_void *crypto_malloc_coherent(td_u32 size)
{
    crypto_mem_type mem_type = CRYPTO_MEM_TYPE_MMZ;
    if (g_drv_func.malloc_coherent == TD_NULL) {
        return TD_NULL;
    }
    if (crypto_smmu_support()) {
        mem_type = CRYPTO_MEM_TYPE_SMMU;
    }
    return g_drv_func.malloc_coherent(size, mem_type);
}

td_void *crypto_malloc_mmz(td_u32 size)
{
    if (g_drv_func.malloc_coherent == TD_NULL) {
        return TD_NULL;
    }
    return g_drv_func.malloc_coherent(size, CRYPTO_MEM_TYPE_MMZ);
}

td_void crypto_free_coherent(td_void *ptr)
{
    if (g_drv_func.free_coherent == TD_NULL) {
        return;
    }
    g_drv_func.free_coherent(ptr);
}

td_phys_addr_t crypto_get_phys_addr(td_void *ptr)
{
    if (g_drv_func.get_phys_addr == TD_NULL) {
        return (td_phys_addr_t)(uintptr_t)(ptr);
    }
    return g_drv_func.get_phys_addr(ptr);
}

td_bool crypto_smmu_support(td_void)
{
    if (g_drv_func.get_smmu_table_addr != TD_NULL) {
        return TD_TRUE;
    } else {
        return TD_FALSE;
    }
}

td_void crypto_get_smmu_table_addr(unsigned long *table, unsigned long *rdaddr, unsigned long *wraddr)
{
    if (g_drv_func.get_smmu_table_addr == TD_NULL) {
        return;
    }
    g_drv_func.get_smmu_table_addr(table, rdaddr, wraddr);
}

td_s32 drv_cipher_register_func(const crypto_drv_func *drv_func_list)
{
    if (drv_func_list == TD_NULL) {
        return TD_FAILURE;
    }

    g_drv_func.malloc_coherent = drv_func_list->malloc_coherent;
    g_drv_func.free_coherent = drv_func_list->free_coherent;
    g_drv_func.get_phys_addr = drv_func_list->get_phys_addr;
    g_drv_func.get_smmu_table_addr = drv_func_list->get_smmu_table_addr;
    g_drv_func.get_cpu_type = drv_func_list->get_cpu_type;
    g_drv_func.get_pke_rom_lib_start_addr = drv_func_list->get_pke_rom_lib_start_addr;

    return TD_SUCCESS;
}

crypto_cpu_type crypto_get_cpu_type(td_void)
{
    if (g_drv_func.get_cpu_type == TD_NULL) {
        return CRYPTO_CPU_TYPE_ACPU;
    }
    return g_drv_func.get_cpu_type();
}

td_u32 crypto_get_pke_rom_lib_start_addr(td_void)
{
    if (g_drv_func.get_pke_rom_lib_start_addr == TD_NULL) {
        return PKE_ROM_LIB_INVALID_ADDR;
    }
    return g_drv_func.get_pke_rom_lib_start_addr();
}

td_bool __attribute__((weak)) crypto_sm_support(crypto_sm_alg alg)
{
    crypto_unused(alg);
    return TD_TRUE;
}

td_bool crypto_rsa_support(td_u32 klen, drv_pke_rsa_scheme scheme)
{
    crypto_unused(klen);
    crypto_unused(scheme);

    return TD_TRUE;
}

td_bool crypto_hash_support(crypto_hash_type hash_type)
{
    crypto_unused(hash_type);

    return TD_TRUE;
}

td_bool crypto_symc_support(crypto_symc_alg alg, crypto_symc_work_mode mode, crypto_symc_key_length key_len,
    crypto_symc_bit_width bit_width)
{
    crypto_unused(alg);
    crypto_unused(mode);
    crypto_unused(key_len);
    crypto_unused(bit_width);

    return TD_TRUE;
}

void crypto_dump_data(const char *name, const td_u8 *data, td_u32 data_len)
{
    td_u32 i;
    td_u32 dump_length = data_len;
    crypto_unused(name);
    crypto_unused(data);
    crypto_print("%s(addr is 0x%x, size is %d Bytes):\r\n", name, (uintptr_t)data, data_len);

    if (dump_length > MAX_DUMP_LENGHT) {
        dump_length = MAX_DUMP_LENGHT;
    }
    for (i = 0; i < dump_length; i++) {
        crypto_print(KERN_CONT"%02x ", data[i]);
        if ((i + 1) % BYTES_IN_ONE_LINE == 0) {
            crypto_print("\r\n");
        }
    }
    if (dump_length % BYTES_IN_ONE_LINE != 0) {
        crypto_print("\r\n");
    }
}

td_s32 __attribute__((weak)) crypto_get_multi_random(td_u32 size, td_u8 *randnum)
{
    return drv_cipher_trng_get_multi_random(size, randnum);
}

#if defined(CRYPTO_SYMC_ADDR_NOT_ALIGN_SUPPORT)
td_s32 __attribute__((weak)) crypto_virt_xor_phys_copy_to_phys(td_phys_addr_t dst_phys_addr, const td_u8 *a_virt_addr,
    td_phys_addr_t b_phys_addr, td_u32 length)
{
    td_s32 ret;
    td_u32 i;
    td_u8 *b_virt_addr = TD_NULL;
    td_u8 *dst_virt_addr = TD_NULL;

    b_virt_addr = crypto_ioremap_nocache(b_phys_addr, length);
    crypto_chk_goto_with_ret(ret, b_virt_addr == TD_NULL, exit, TD_FAILURE, "crypto_ioremap_nocache failed\n");

    dst_virt_addr = crypto_ioremap_nocache(dst_phys_addr, length);
    crypto_chk_goto_with_ret(ret, dst_virt_addr == TD_NULL, exit, TD_FAILURE, "crypto_ioremap_nocache failed\n");

#if defined(CRYPTO_CTR_TRACE_ENABLE)
    crypto_dump_data("a_virt_addr", a_virt_addr, length);
    crypto_dump_data("b_virt_addr", b_virt_addr, length);
#endif

    for (i = 0; i < length; i++) {
        dst_virt_addr[i] = a_virt_addr[i] ^ b_virt_addr[i];
    }

#if defined(CRYPTO_CTR_TRACE_ENABLE)
    crypto_dump_data("dst_virt_addr", dst_virt_addr, length);
#endif
    ret = TD_SUCCESS;
exit:
    if (b_virt_addr != TD_NULL) {
        crypto_iounmap(b_virt_addr, length);
    }
    if (dst_virt_addr != TD_NULL) {
        crypto_iounmap(dst_virt_addr, length);
    }
    return ret;
}

td_s32 __attribute__((weak)) crypto_virt_copy_to_phys(td_phys_addr_t dst_phys_addr,
    const td_u8 *src_virt_addr, td_u32 length)
{
    td_s32 ret;
    td_u8 *dst_virt_addr = TD_NULL;

    dst_virt_addr = crypto_ioremap_nocache(dst_phys_addr, length);
    crypto_chk_goto_with_ret(ret, dst_virt_addr == TD_NULL, exit, TD_FAILURE, "crypto_ioremap_nocache failed\n");

    ret = memcpy_s(dst_virt_addr, length, src_virt_addr, length);
    crypto_chk_goto_with_ret(ret, ret != EOK, exit, TD_FAILURE, "memcpy_s failed\n");

    ret = TD_SUCCESS;
exit:
    if (dst_virt_addr != TD_NULL) {
        crypto_iounmap(dst_virt_addr, length);
    }
    return ret;
}

td_s32 __attribute__((weak)) crypto_phys_copy_to_virt(td_u8 *dst_virt_addr, td_phys_addr_t src_phys_addr, td_u32 length)
{
    td_s32 ret;
    td_u8 *src_virt_addr = TD_NULL;

    src_virt_addr = crypto_ioremap_nocache(src_phys_addr, length);
    crypto_chk_goto_with_ret(ret, src_virt_addr == TD_NULL, exit, TD_FAILURE, "crypto_ioremap_nocache failed\n");

    ret = memcpy_s(dst_virt_addr, length, src_virt_addr, length);
    crypto_chk_goto_with_ret(ret, ret != EOK, exit, TD_FAILURE, "memcpy_s failed\n");

    ret = TD_SUCCESS;
exit:
    if (src_virt_addr != TD_NULL) {
        crypto_iounmap(src_virt_addr, length);
    }
    return ret;
}
#endif /* CRYPTO_SYMC_ADDR_NOT_ALIGN_SUPPORT */

td_s32 crypto_get_value_by_index(const crypto_table_item *table, td_u32 table_size,
    td_u32 index, td_u32 *value)
{
    const crypto_table_item *item = TD_NULL;
    td_u32 i;

    crypto_chk_return(table == TD_NULL, TD_FAILURE, "table is NULL\n");
    crypto_chk_return(value == TD_NULL, TD_FAILURE, "value is NULL\n");

    for (i = 0; i < table_size; i++) {
        item = &table[i];
        if (item->index == index) {
            *value = item->value;
            return TD_SUCCESS;
        }
    }
    crypto_log_err("Invalid Index!\n");
    return TD_FAILURE;
}