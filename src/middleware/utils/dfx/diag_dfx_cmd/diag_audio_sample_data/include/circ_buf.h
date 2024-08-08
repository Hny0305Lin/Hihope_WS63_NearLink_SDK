/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: audio circle buffer function
 * Author:
 * Create: 2019-05-30
 */

#ifndef __CIRC_BUF_H__
#define __CIRC_BUF_H__

#include "securec.h"
#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    volatile td_u32 *write; /* circle buffer write pointer */
    volatile td_u32 *read;  /* circle buffer read pointer */
    td_u8 *data;            /* circle buffer data virtual address */
    td_u32 size;            /* circle buffer size */
} circ_buf;

static inline td_u32 circ_buf_min(td_u32 x, td_u32 y)
{
    return (x <= y) ? x : y;
}

static inline td_u32 saturate_add(td_u32 x, td_u32 y, td_u32 saturation)
{
    /* add x and y, saturate to saturation */
    td_u32 x_temp = x + y;

    return (x_temp >= saturation) ? (x_temp - saturation) : x_temp;
}

static inline td_void circ_buf_init(circ_buf *cb, td_u32 *write, td_u32 *read, td_u8 *data, td_u32 len)
{
    cb->write = write;
    cb->read = read;
    cb->data = data;
    cb->size = len;

    *cb->write = 0;
    *cb->read = 0;
}

static inline td_void circ_buf_flush(circ_buf *cb)
{
    *cb->read = 0;
    *cb->write = 0;
}

static inline td_u32 circ_buf_data_size(td_u32 write, td_u32 read, td_u32 size)
{
    return (write >= read) ? (write - read) : (size - read + write);
}

static inline td_u32 circ_buf_free_size(td_u32 write, td_u32 read, td_u32 size)
{
    return size - circ_buf_data_size(write, read, size);
}

static inline td_u32 cb_read_data(const circ_buf *cb, td_u8 *to, td_u32 len, td_u32 off)
{
    if ((len == 0) || (memcpy_s(to, len * sizeof(td_u8), cb->data + off, len) != EOK)) {
        return 0;
    }

    return len;
}

/* read data from circle buffer */
static inline td_u32 circ_buf_read_data(const circ_buf *cb, td_u8 *to, td_u32 len, td_u32 off)
{
    td_u32 l = circ_buf_min(len, cb->size - off);

    if (to == TD_NULL) {
        /* dump data in circle buffer */
        return len;
    }

    return cb_read_data(cb, to, l, off) + cb_read_data(cb, to + l, len - l, 0);
}

static inline td_u32 cb_write_zero(circ_buf *cb, const td_u8 *from, td_u32 len, td_u32 off)
{
    if ((len == 0) || (memset_s(cb->data + off, len, 0, len) != EOK)) {
        return 0;
    }
    (td_void)from;
    return len;
}

static inline td_u32 cb_write_data(circ_buf *cb, const td_u8 *from, td_u32 len, td_u32 off)
{
    if ((len == 0) || (memcpy_s(cb->data + off, cb->size - off, from, len) != EOK)) {
        return 0;
    }

    return len;
}

/*
 * circ_buf_write_data - puts some data from kernel space into the circle buffer
 * @cb: address of the circle buffer to be used
 * @from: pointer to the data to be added
 * @len: the length of the data to be added
 * @off: data write offset in circle buffer
 *
 * Note: Check free space in circle buffer before calling this function
 */
static inline td_u32 circ_buf_write_data(circ_buf *cb, const td_u8 *from, td_u32 len, td_u32 off)
{
    td_u32 l = circ_buf_min(len, cb->size - off);

    /* generate mute data if input buffer is TD_NULL */
    td_u32 (*action)(circ_buf *, const td_u8 *, td_u32, td_u32) =
        (from == TD_NULL) ? cb_write_zero : cb_write_data;

    return action(cb, from, l, off) + action(cb, from + l, len - l, 0);
}

static inline td_u32 circ_buf_query_busy(const circ_buf *cb)
{
    return circ_buf_data_size(*cb->write, *cb->read, cb->size);
}

static inline td_u32 circ_buf_query_free(const circ_buf *cb)
{
    return circ_buf_free_size(*cb->write, *cb->read, cb->size);
}

/* read data from circle buffer */
static inline td_u32 circ_buf_read(circ_buf *cb, td_u8 *to, td_u32 len)
{
    td_u32 read = *cb->read;

    if ((len == 0) || (circ_buf_data_size(*cb->write, read, cb->size) < len) ||
        (circ_buf_read_data(cb, to, len, read) != len)) {
        return 0;
    }

    *cb->read = saturate_add(read, len, cb->size);
    return len;
}

/* write data into circle buffer */
static inline td_u32 circ_buf_write(circ_buf *cb, const td_u8 *from, td_u32 len)
{
    td_u32 write = *cb->write;

    if ((len == 0) || (circ_buf_write_data(cb, from, len, write) != len)) {
        return 0;
    }

    *cb->write = saturate_add(write, len, cb->size);
    return len;
}

static inline td_u32 circ_buf_update_read_pos(circ_buf *cb, td_u32 len)
{
    *cb->read = saturate_add(*cb->read, len, cb->size);
    return len;
}

static inline td_u32 circ_buf_update_write_pos(circ_buf *cb, td_u32 len)
{
    *cb->write = saturate_add(*cb->write, len, cb->size);
    return len;
}

static inline td_u32 circ_buf_cast_read(const circ_buf *cb, td_u32 *data_offset, td_u32 len)
{
    td_u32 read = *cb->read;

    if (circ_buf_data_size(*cb->write, read, cb->size) < len) {
        *data_offset = 0;
        return 0;
    }

    *data_offset = read;
    return len;
}

static inline td_u32 circ_buf_cast_relese(circ_buf *cb, td_u32 len)
{
    td_u32 read = *cb->read;
    td_u32 data_size = circ_buf_data_size(*cb->write, read, cb->size);
    td_u32 len_temp;

    len_temp = circ_buf_min(data_size, len);
    *cb->read = saturate_add(read, len_temp, cb->size);

    return len_temp;
}

/* peak data from circle buffer */
static inline td_u32 circ_buf_peak(circ_buf *cb, td_u8 *to, td_u32 len)
{
    td_u32 read = *cb->read;

    if ((len == 0) || (circ_buf_data_size(*cb->write, read, cb->size) < len) ||
        (circ_buf_read_data(cb, to, len, read) != len)) {
        return 0;
    }

    return len;
}

/* poke/release data from circle buffer */
static inline td_void circ_buf_poke(circ_buf *cb, td_u32 len)
{
    td_u32 read = *cb->read;

    *cb->read = saturate_add(read, len, cb->size);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CIRC_BUF_H__ */
