/**
 * @defgroup seq_file Seq_file
 * @ingroup linux
 */

#ifndef _LINUX_SEQ_FILE_H
#define _LINUX_SEQ_FILE_H

#include "sys/types.h"
#include "pthread.h"
#include "linux/types.h"
#include "linux/list.h"
#include "linux/kernel.h"
#ifdef LOSCFG_FS_VFS
#include "fs/fs.h"
#ifdef LOSCFG_FS_PROC
#include "proc_fs.h"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef char seq_char;
typedef size_t seq_size_t;
typedef loff_t seq_loff_t;
typedef int seq_int;
typedef void seq_void;

struct seq_file {
    seq_char *buf;                      /**< Data buffer for sequential file. */
    seq_size_t size;                    /**< Size of the buf. */
    seq_size_t from;                    /**< The start offset of the data that is not copied in the buf. */
    seq_size_t count;                   /**< Bytes of data that can be copied. */
    seq_loff_t index;                   /**< Index of the operation provided to the start and next functions. */
    seq_loff_t read_pos;                /**< Size of the data that has been copied. */
    unsigned long long version;         /**< Version of the sequential file. */
    pthread_mutex_t lock;               /**< Mutex lock for this seq_file operation. */
    const struct seq_operations *op;    /**< Point to a function that manipulates the underlying data. */
    seq_int poll_event;                 /**< Not used. */
    seq_void *private;                  /**< Private data of sequential file. */
};

typedef struct seq_operations {
    void *(*start)(struct seq_file *, loff_t *);            /**< Start read data. */
    void (*stop)(struct seq_file *, void *);                /**< Stop read data. */
    void *(*next)(struct seq_file *, void *, loff_t *);     /**< Gets the next data to be processed. */
    int (*show)(struct seq_file *, void *);                 /**< Print data to temporary buffer. */
} seq_operations_t;

struct proc_file;

/**
 * @ingroup  seq_file
 * @brief initialize sequential file.
 *
 * @par Description:
 * seq_open sets procfilep, associating it with a sequence described by op.
 * @attention
 * <ul>
 * <li> The input parameter procfilep and op must point to valid memory,
 * or it will return errno -1.</li>
 * <li> It is recommended that user call the single_open interface to replace this interface.</li>
 * </ul>
 *
 * @param  procfilep [IN] the file to initialize.
 * @param  op        [IN] method table describing the sequence.
 *
 * @retval #-1 initialize sequential file fail.
 * @retval #0 initialize sequential file success.
 * @par Dependency:
 * <ul><li>seq_file.h: the header file that contains the API declaration.</li></ul>
 * @see seq_realse
 */
int seq_open(struct proc_file *procfilep, const struct seq_operations *op);

/**
 * @ingroup  seq_file
 * @brief read method for sequential files.
 *
 * @par Description:
 * This API is used to read method for sequential files, and wirte the relevant
 * kernel real-time data into the seq files, then presented to the user.
 * @attention
 * <ul>
 * <li> The input parameter procfilep and buf and ppos must point to valid memory,
 * and the value of ppos point to should >=0, or it will lead read file fail.</li>
 * <li> when the user call this interface, should call the single_open first.</li>
 * </ul>
 *
 * @param  procfilep [IN] the proc file struct to read from.
 * @param  buf       [IN] the buffer to read to.
 * @param  readsize  [IN] the maximum number of bytes to read.
 * @param  ppos      [IN] the current position in the file to read from.
 *
 * @retval #-1 read file fail.
 * @retval ssize_t the real read size copy into the buf.
 * @par Dependency:
 * <ul><li>seq_file.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
ssize_t seq_read(struct proc_file *procfilep, char *buf, size_t readsize, loff_t *ppos);

/**
 * @ingroup  seq_file
 * @brief llseek method for sequential files.
 *
 * @par Description:
 * This API is used to llseek method for sequential files.
 * @attention
 * <ul>
 * <li>The input parameter procfilep must point to valid memory.</li>
 * <li>origin should be SEEK_SET(seek from start of file) or SEEK_CUR(seek from
 * current position of file), seek from the end of file are not supported.</li>
 * </ul>
 *
 * @param  procfilep [IN] the proc file struct to seek.
 * @param  offset    [IN] the relative position in the file to seek.
 * @param  origin    [IN] SEEK_SET means the offset is relative to the start of the file,
 * SEEK_CUR means the offset is relative to the current position of the file.
 *
 * @retval loff_t the absolute offset.
 * @retval #-1 operation not permitted.
 * @retval #-EINVAL invalid argument.
 * @par Dependency:
 * <ul><li>seq_file.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
loff_t seq_lseek(struct proc_file *procfilep, loff_t offset, int origin);

/**
 * @ingroup  seq_file
 * @brief free the structures associated with sequential file.
 *
 * @par Description:
 * This API is used to free the structures associated with sequential file.
 * @attention
 * <ul>
 * <li> The input parameter procfilep must point to valid memory.</li>
 * <li> It is recommended that user call the single_release interface to replace
 * this interface.</li>
 * <li> if use this interface, it must call the seq_open first.</li>
 * </ul>
 *
 * @param  inodep    [IN] its inode (not used).
 * @param  procfilep [IN] pointer to proc file which is opened.
 *
 * @retval #-1 release file failed.
 * @retval #0 release file success.
 * @par Dependency:
 * <ul><li>seq_file.h: the header file that contains the API declaration.</li></ul>
 * @see seq_open
 */
int seq_release(struct inode *inodep, struct proc_file *procfilep);

/**
 * @ingroup  seq_file
 * @brief print a formatted string and variable data to the seq_file output buffer.
 *
 * @par Description:
 * This API is used to write the kernel data into the seq file with a certain format.
 *
 * @attention
 * <ul>
 * <li>The input parameter seqfilep must point to valid memory.</li>
 * <li>The size of seq file must less than 1M Bytes.</li>
 * </ul>
 *
 * @param  seqfilep [IN] pointer to the seq file.
 * @param  f        [IN] format control string.
 *
 * @retval #0 seq printf success.
 * @retval #-1 seq printf failed.
 * @par Dependency:
 * <ul><li>Seq_file.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
int seq_printf(struct seq_file *seqfilep, const char *f, ...);

/**
 * @ingroup  seq_file
 * @brief open the seq stream.
 *
 * @par Description:
 * This API is used to open the seq stream.
 *
 * @attention
 * <ul>
 * <li>Please make sure all the input parameters are valid.</li>
 * </ul>
 *
 * @param  procfilep [IN] pointer to the procfile.
 * @param  show      [IN] pointer to the "show" function for construct seq file.
 * @param  data      [IN] pointer to the data that is to be written.
 *
 * @retval #0 open the seq stream succeed.
 * @retval #-1 open seq failed.
 * @par Dependency:
 * <ul><li>Seq_file.h: the header file that contains the API declaration.</li></ul>
 * @see single_release
 */
int single_open(struct proc_file *procfilep, int (*show)(struct seq_file *, void *), void *data);

/**
 * @ingroup  seq_file
 * @brief release the memory malloced by single_open.
 *
 * @par Description:
 * This API is used to release the memory malloced by single_open.
 *
 * @attention
 * <ul>
 * <li>Please make sure all the input parameters are valid.</li>
 * <li>if use this interface, it must call the single_open at first.</li>
 * </ul>
 *
 * @param  inodep    [IN] pointer to struct inode.
 * @param  procfilep [IN] pointer to struct proc_file.
 *
 * @retval #0 success to release seq file.
 * @retval #-1 fail to release seq file.
 * @par Dependency:
 * <ul><li>Seq_file.h: the header file that contains the API declaration.</li></ul>
 * @see single_open
 */
int single_release(struct inode *inodep, struct proc_file *procfilep);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_SEQ_FILE_H */
