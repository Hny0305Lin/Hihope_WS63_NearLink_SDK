/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: littlefs adapt layer
 */

#include "lfs.h"
#include "fcntl.h"
#include "sfc.h"
#include "securec.h"
#include "soc_osal.h"
#include "littlefs_config.h"
#include "securec.h"
#include "partition.h"
#include "littlefs_adapt.h"

#define LFS_NAME_LEN_MAX 64
#define LFS_OPEN_MAX 32
#define LFS_FLASH_START 0x200000
#define LFS_FLASH_4K_MASK 0xFFF
#define LFS_FLASH_4K 0x1000
#define LFS_RAM_BASE 0xA00000
// variables used by the filesystem
typedef struct {
    uint32_t start_block;
    uint32_t max_block;
} lfs_block_info_t;
static lfs_block_info_t g_lfs_block_info;
static lfs_t g_lfs;
static uint8_t g_lfs_open_file_cnt = 0;
#ifdef LFS_THREADSAFE
uint32_t g_lfs_lock;
#endif
static const char lfs_root_path[] = ".";

static int littlefs_adapt_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer,
    lfs_size_t size)
{
    errcode_t ret = ERRCODE_FAIL;
    ret = uapi_sfc_reg_read(c->block_size * (g_lfs_block_info.start_block + block) + off, (uint8_t *)buffer, size);
    if (ret != ERRCODE_SUCC) {
        return (int)ret;
    }
    return LFS_ERR_OK;
}

static int littlefs_adapt_write(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer,
    lfs_size_t size)
{
    errcode_t ret = ERRCODE_FAIL;
    ret = uapi_sfc_reg_write(c->block_size * (g_lfs_block_info.start_block + block) + off, (uint8_t *)buffer, size);
    if (ret != ERRCODE_SUCC) {
        return (int)ret;
    }

    return LFS_ERR_OK;
}

static int littlefs_adapt_erase(const struct lfs_config *c, lfs_block_t block)
{
    lfs_debug_print_info("littlefs_adapt_erase enter\r\n");
    errcode_t ret = ERRCODE_FAIL;
    uint32_t start_sector = c->block_size * (block + g_lfs_block_info.start_block);
    ret = uapi_sfc_reg_erase(start_sector, c->block_size);
    lfs_debug_print_info("uapi_sfc_reg_erase, ret=%d\r\n", ret);
    if (ret != ERRCODE_SUCC) {
        return (int)ret;
    }
    return LFS_ERR_OK;
}

static int littlefs_adapt_sync(const struct lfs_config *c)
{
    (void)c;
    return LFS_ERR_OK;
}

#ifdef LFS_THREADSAFE
static int littlefs_adapt_lock(const struct lfs_config *c)
{
    (void)c;
    g_lfs_lock = osal_irq_lock();
    return LFS_ERR_OK;
}

static int littlefs_adapt_unlock(const struct lfs_config *c)
{
    (void)c;
    osal_irq_restore(g_lfs_lock);
    return LFS_ERR_OK;
}
#endif

static errcode_t littlefs_adapt_get_block_info(uint32_t *start_block, uint32_t *max_block)
{
    partition_information_t info;

    errcode_t ret_val = uapi_partition_get_info(CONFIG_LFS_PARTITION_ID, &info);
    if (ret_val != ERRCODE_SUCC || info.part_info.addr_info.size == 0 ||
        (info.part_info.addr_info.addr & LFS_FLASH_4K_MASK) != 0 ||
        (info.part_info.addr_info.size & LFS_FLASH_4K_MASK) != 0) {
        return ERRCODE_FAIL;
    }
    *start_block = info.part_info.addr_info.addr / LFS_FLASH_4K;
    *max_block = info.part_info.addr_info.size / LFS_FLASH_4K;
    return ERRCODE_SUCC;
}

void fs_adapt_mount(void)
{
    // configuration of the filesystem is provided by this struct
    uint32_t ret_val = littlefs_adapt_get_block_info(&g_lfs_block_info.start_block, &g_lfs_block_info.max_block);
    if (ret_val != ERRCODE_SUCC) {
        lfs_debug_print_error("fs init failed, ret = 0x%x\r\n", ret_val);
    }
    static struct lfs_config cfg = {
    // block device operations
    .read  = littlefs_adapt_read,
    .prog  = littlefs_adapt_write,
    .erase = littlefs_adapt_erase,
    .sync  = littlefs_adapt_sync,
#ifdef LFS_THREADSAFE
    .lock = littlefs_adapt_lock,
    .unlock = littlefs_adapt_unlock,
#endif

    // block device configuration
    .read_size = 4,
    .prog_size = 4,
    .block_size = 4096,
    .cache_size = 16,
    .lookahead_size = 16,
    .block_cycles = 500,
    .name_max = LFS_NAME_LEN_MAX,
};
    cfg.block_count = g_lfs_block_info.max_block;

    int ret = memset_s(&g_lfs, sizeof(g_lfs), 0, sizeof(g_lfs));
    ret = lfs_mount(&g_lfs, &cfg);
    if (ret < LFS_ERR_OK) {
        ret = lfs_format(&g_lfs, &cfg);
        if (ret < LFS_ERR_OK) {
            lfs_debug_print_error("lfs_format failed, ret = %d\r\n", ret);
        }
        ret = lfs_mount(&g_lfs, &cfg);
        lfs_debug_print_info("lfs_mount 2, ret = %d\r\n", ret);
        if (ret < LFS_ERR_OK) {
            lfs_debug_print_error("lfs_mount failed, ret = %d\r\n", ret);
        }
    }
}

void fs_adapt_unmount(void)
{
    lfs_unmount(&g_lfs);
}

int fs_adapt_mkdir(const char *path)
{
    if (path == NULL) {
        return -1;
    }
    int ret = lfs_mkdir(&g_lfs, path);
    if (ret == LFS_ERR_EXIST) {
        lfs_debug_print_info("dir exists, ret = %d, dir = %s\r\n", ret, path);
        return LFS_ERR_OK;
    }
    if  (ret < 0) {
        lfs_debug_print_error("lfs_mkdir failed, ret = %d, dir = %s\r\n", ret, path);
        return -1;
    }
    return ret;
}

static int fs_adapt_path_format(const char* path)
{
    if (path == NULL) {
        return -1;
    }
    char *dir = NULL;
    dir = strrchr(path, '/');
    if (dir == NULL || strlen(path) == strlen(dir)) {
        return LFS_ERR_OK;
    }
    uint32_t len = strlen(path) - strlen(dir);
    char *dir_str = (char *)malloc(len + 1);
    if (dir_str == NULL) {
        lfs_debug_print_error("dir string malloc failed\r\n");
        return -1;
    }
    dir_str[len] = '\0';
    int ret = strncpy_s(dir_str, len + 1, path, len);
    if (ret != 0) {
        lfs_debug_print_error("ret : %d\r\n", ret);
        free(dir_str);
        return -1;
    }
    lfs_debug_print_info("dir_str: %s, %d\r\n", dir_str, strlen(dir_str));

    ret = fs_adapt_mkdir(dir_str);
    if (ret < 0 && ret != LFS_ERR_EXIST) {
        lfs_debug_print_error("mkdir failed: %s\r\n", dir_str);
        free(dir_str);
        return ret;
    }
    free(dir_str);
    return LFS_ERR_OK;
}

static int fs_adapt_flag_format(int oflag)
{
    uint32_t input_flag = (uint32_t)oflag;
    uint32_t ret_flag = 0;
    if (input_flag == O_RDONLY) {
        return LFS_O_RDONLY;
    }
    if ((input_flag & O_WRONLY) != 0) {
        ret_flag |= LFS_O_RDWR;
    }
    if ((input_flag & O_RDWR) != 0) {
        ret_flag |= LFS_O_RDWR;
    }
    if ((input_flag & O_CREAT) != 0) {
        ret_flag |= LFS_O_CREAT;
    }
    if ((input_flag & O_EXCL) != 0) {
        ret_flag |= LFS_O_EXCL;
    }
    if ((input_flag & O_TRUNC) != 0) {
        ret_flag |= LFS_O_TRUNC;
    }
    if ((input_flag & O_APPEND) != 0) {
        ret_flag |= LFS_O_APPEND;
    }
    return (int)ret_flag;
}

int fs_adapt_open(const char* path, int oflag)
{
    if (path == NULL || strlen(path) >= LFS_NAME_LEN_MAX || g_lfs_open_file_cnt >= LFS_OPEN_MAX) {
        return -1;
    }
    int ret;
    const char *name = NULL;
    int real_flag = fs_adapt_flag_format(oflag);
    lfs_file_t *fp = (lfs_file_t *)malloc(sizeof(lfs_file_t));
    if  (fp == NULL) {
        lfs_debug_print_error("fp malloc failed\r\n");
        return -1;
    }
    name = path;
    if (name[0] == '/' && name[1] == 0) {
        name = lfs_root_path;
    } else if (name[0] == '/') {
        name++;
    }
    lfs_debug_print_info("fs_adapt_open enter, name = %s, oflag = 0x%x, real_flag = 0x%x\r\n",
        name, oflag, real_flag);
    ret = fs_adapt_path_format(path);
    if (ret != LFS_ERR_OK) {
        free(fp);
        return -1;
    }
    ret = lfs_file_open(&g_lfs, fp, name, real_flag);
    if (ret < LFS_ERR_OK) {
        lfs_debug_print_error("lfs_file_open failed, ret = %d, name = %s\r\n", ret, name);
        free(fp);
        return -1;
    }
    g_lfs_open_file_cnt++;
    lfs_debug_print_info("fs_adapt_open done\r\n");
    return (int)(intptr_t)fp;
}

int fs_adapt_close(int fd)
{
    lfs_debug_print_info("fs_adapt_close enter, fd = 0x%x\r\n", fd);
    if (fd < LFS_RAM_BASE) {
        return -1;
    }
    int ret = lfs_file_close(&g_lfs, (lfs_file_t *)(intptr_t)fd);
    if  (ret < 0) {
        lfs_debug_print_error("lfs_file_close failed, ret = %d\r\n", ret);
        free((lfs_file_t *)(intptr_t)fd);
        return -1;
    }
    if (g_lfs_open_file_cnt > 0) {
        g_lfs_open_file_cnt--;
    }
    free((lfs_file_t *)(intptr_t)fd);
    lfs_debug_print_info("fs_adapt_close done\r\n");
    return ret;
}

int fs_adapt_read(int fd, char *buf, unsigned int len)
{
    lfs_debug_print_info("fs_adapt_read enter, len = %d, fd = 0x%x\r\n", len, fd);
    if (len == 0 || fd < LFS_RAM_BASE) {
        return -1;
    }
    int ret = lfs_file_read(&g_lfs, (lfs_file_t *)(intptr_t)fd, buf, len);
    if  (ret < 0) {
        lfs_debug_print_error("lfs_file_read failed, ret = %d\r\n", ret);
        return -1;
    }
    lfs_debug_print_info("fs_adapt_read done\r\n");
    return ret;
}

int fs_adapt_write(int fd, const char *buf, unsigned int len)
{
    lfs_debug_print_info("fs_adapt_write enter,  len = %d, fd = 0x%x\r\n", len, fd);
    if (len == 0 || fd < LFS_RAM_BASE) {
        return -1;
    }
    int ret = lfs_file_write(&g_lfs, (lfs_file_t *)(intptr_t)fd, buf, len);
    if  (ret < 0) {
        lfs_debug_print_error("lfs_file_write failed, ret = %d\r\n", ret);
        return -1;
    }
    lfs_debug_print_info("fs_adapt_write done\r\n");
    return ret;
}

int fs_adapt_delete(const char *path)
{
    lfs_debug_print_info("fs_adapt_delete enter, path = %s\r\n", path);
    if (path == NULL) {
        return -1;
    }
    int ret = lfs_remove(&g_lfs, path);
    if  (ret < 0) {
        lfs_debug_print_error("lfs_remove failed, ret = %d\r\n", ret);
        return -1;
    }
    lfs_debug_print_info("fs_adapt_delete done\r\n");
    return ret;
}

int fs_adapt_stat(const char *path, unsigned int *file_size)
{
    lfs_debug_print_info("fs_adapt_stat enter, path = %s\r\n", path);
    if (path == NULL) {
        return -1;
    }
    struct lfs_info info = {0};
    int ret = lfs_stat(&g_lfs, path, &info);
    if (ret < 0) {
        lfs_debug_print_error("lfs_stat failed, ret = %d\r\n", ret);
        return -1;
    }
    *file_size = info.size;
    lfs_debug_print_info("fs_adapt_stat done\r\n");
    return 0;
}

int fs_adapt_seek(int fd, int offset, unsigned int whence)
{
    lfs_debug_print_info("fs_adapt_stat enter, fd = 0x%x\r\n", fd);
    if (fd < LFS_RAM_BASE) {
        return -1;
    }
    int file_size = lfs_file_size(&g_lfs, (lfs_file_t *)(intptr_t)fd);
    int file_pos = lfs_file_tell(&g_lfs, (lfs_file_t *)(intptr_t)fd);
    int next_pos;
    if (whence == LFS_SEEK_SET) {
        next_pos = offset;
    } else if (whence == LFS_SEEK_CUR) {
        next_pos = file_pos + offset;
    } else if (whence == LFS_SEEK_END) {
        next_pos = file_size + offset;
    } else {
        return -1;
    }
    if (next_pos < 0 || next_pos > file_size) {
        return -1;
    }
    int ret = lfs_file_seek(&g_lfs, (lfs_file_t *)(intptr_t)fd, offset, (int)whence);
    if  (ret < 0) {
        lfs_debug_print_error("lfs_file_seek failed, ret = %d\r\n", ret);
        return -1;
    }
    lfs_debug_print_info("fs_adapt_seek done\r\n");
    return ret;
}

int fs_adapt_sync(int fd)
{
    int ret = lfs_file_sync(&g_lfs, (lfs_file_t *)(intptr_t)fd);
    if  (ret < 0) {
        lfs_debug_print_error("fs_adapt_sync failed, ret = %d\r\n", ret);
        return -1;
    }
    return ret;
}

void lfs_test(void)
{
    // read current count
    char boot_count = 0;

    int fp = fs_adapt_open("/lfs_test", O_RDWR | O_CREAT);
    if (fp < 0) {
        return;
    }
    int ret = fs_adapt_read(fp, &boot_count, sizeof(boot_count));
    lfs_debug_print_info("lfs_test read, ret = %d\r\n", ret);

    // print the boot count
    lfs_debug_print_info("===========boot_count: %d=============\r\n", boot_count);

    // update boot count
    boot_count = (char)((uint8_t)boot_count + 1);
    ret = fs_adapt_seek(fp, 0, LFS_SEEK_SET);
    lfs_debug_print_info("lfs_test seek, ret = %d\r\n", ret);
    if (ret < LFS_ERR_OK) {
        return;
    }
    ret = fs_adapt_write(fp, &boot_count, sizeof(boot_count));
    lfs_debug_print_info("lfs_test write, ret = %d\r\n", ret);

    // remember the storage is not updated until the file is closed successfully
    ret = fs_adapt_close(fp);
    lfs_debug_print_info("lfs_test close, ret = %d\r\n", ret);
    if (ret < LFS_ERR_OK) {
        return;
    }
}

#ifdef LFS_NEED_LIB_STDIO
int open(const char *path, int oflags, ...)
{
    return fs_adapt_open(path, oflags);
}

int close(int fd)
{
    return fs_adapt_close(fd);
}

ssize_t read(int fd, void *buf, size_t nbytes)
{
    return fs_adapt_read(fd, buf, nbytes);
}

ssize_t write(int fd, const void *buf, size_t nbytes)
{
    return fs_adapt_write(fd, buf, nbytes);
}

off_t lseek(int fd, off_t offset, int whence)
{
    return fs_adapt_seek(fd, offset, whence);
}

int unlink(const char *pathname)
{
    return fs_adapt_delete(pathname);
}

int fsync(int fd)
{
    return fs_adapt_sync(fd);
}
#endif