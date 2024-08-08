/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2015 Aleph One Ltd.
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This software is the subject of a Licence Agreement between Aleph One Limited and Hi_Silicon_Huawei_DigiMedia.
 *
 */


/*
 * Header file for using yaffs in an application via
 * a direct interface.
 */


#ifndef __YAFFSFS_H__
#define __YAFFSFS_H__

#include "yaffscfg.h"
#include "yportenv.h"
#include "limits.h"
#include "yaffs_adapt_os.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define YAFFS_MAX_FILE_SIZE	YAFFS_MAX_FILE_SIZE_32

struct yaffs_dirent {
	long d_ino;			/* inode number */
	off_t d_off;			/* offset to this dirent */
	unsigned short d_reclen;	/* length of this dirent */
	YUCHAR d_type;			/* type of this record */
	YCHAR d_name[NAME_MAX+1];	/* file name (null-terminated) */
	unsigned long d_dont_use;	/* debug: not for public consumption */
};

typedef struct opaque_structure yaffs_DIR;

struct yaffs_stat {
	long		st_dev;		/* device */
	int		st_ino;		/* inode */
	unsigned	st_mode;	/* protection */
	int		st_nlink;	/* number of hard links */
	int		st_uid;		/* user ID of owner */
	int		st_gid;		/* group ID of owner */
	unsigned	st_rdev;	/* device type (if inode device) */
	Y_LOFF_T	st_size;	/* total size, in bytes */
	unsigned long	st_blksize;	/* blocksize for filesystem I/O */
	unsigned long	st_blocks;	/* number of blocks allocated */
#ifdef CONFIG_YAFFS_WINCE
	/* Special 64-bit times for WinCE */
	unsigned long	yst_wince_atime[2];
	unsigned long	yst_wince_mtime[2];
	unsigned long	yst_wince_ctime[2];
#else
	YTIME_T	yst_atime;	/* time of last access */
	YTIME_T	yst_mtime;	/* time of last modification */
	YTIME_T	yst_ctime;	/* time of last change */
#endif
};


struct yaffs_utimbuf {
	YTIME_T actime;
	YTIME_T modtime;
};

/* Normal POSIX-style API functions */

int yaffs_open(const YCHAR *path, int oflag, int mode) ;

int yaffs_close(long fd) ;
long yaffs_fsync(long fd) ;
#ifdef YAFFS_EXTEND_SUPPORT
long yaffs_fdatasync(long fd) ;
#endif
long yaffs_flush(long fd) ; /* same as yaffs_fsync() */

#ifdef YAFFS_EXTEND_SUPPORT
int yaffs_access(const YCHAR *path, int amode);
#endif

long yaffs_dup(long fd);

int yaffs_fgetfl(int fd, int *flags);

long yaffs_read(long fd, void *buf, unsigned int nbyte) ;
long yaffs_write(long fd, const void *buf, unsigned int nbyte) ;

#ifdef YAFFS_EXTEND_SUPPORT
long yaffs_pread(long fd, void *buf, unsigned int nbyte, Y_LOFF_T offset);
int yaffs_pwrite(long fd, const void *buf, unsigned int nbyte, Y_LOFF_T offset);
#endif

Y_LOFF_T yaffs_lseek(int fd, Y_LOFF_T offset, int whence) ;

#ifdef YAFFS_EXTEND_SUPPORT
int yaffs_truncate(const YCHAR *path, Y_LOFF_T new_size);
int yaffs_ftruncate(int fd, Y_LOFF_T new_size);
#endif

int yaffs_unlink(const YCHAR *path) ;
int yaffs_funlink(int fd);

int yaffs_rename(const YCHAR *oldPath, const YCHAR *newPath) ;

int yaffs_stat(const YCHAR *path, struct yaffs_stat *buf) ;
int yaffs_lstat(const YCHAR *path, struct yaffs_stat *buf) ;
#ifdef YAFFS_EXTEND_SUPPORT
int yaffs_fstat(int fd, struct yaffs_stat *buf) ;

int yaffs_utime(const YCHAR *path, const struct yaffs_utimbuf *buf);
int yaffs_futime(int fd, const struct yaffs_utimbuf *buf);


int yaffs_setxattr(const char *path, const char *name,
			const void *data, int size, int flags);
int yaffs_lsetxattr(const char *path, const char *name,
			const void *data, int size, int flags);
int yaffs_fsetxattr(int fd, const char *name,
			const void *data, int size, int flags);

int yaffs_getxattr(const char *path, const char *name,
			void *data, int size);
int yaffs_lgetxattr(const char *path, const char *name,
			void *data, int size);
int yaffs_fgetxattr(int fd, const char *name,
			void *data, int size);

int yaffs_removexattr(const char *path, const char *name);
int yaffs_lremovexattr(const char *path, const char *name);
int yaffs_fremovexattr(int fd, const char *name);

int yaffs_listxattr(const char *path, char *list, int size);
int yaffs_llistxattr(const char *path, char *list, int size);
int yaffs_flistxattr(int fd, char *list, int size);

int yaffs_chmod(const YCHAR *path, mode_t mode);
int yaffs_fchmod(int fd, mode_t mode);
#endif

int yaffs_mkdir(const YCHAR *path, mode_t mode) ;
int yaffs_rmdir(const YCHAR *path) ;

yaffs_DIR *yaffs_opendir(const YCHAR *dirname) ;
struct yaffs_dirent *yaffs_readdir(yaffs_DIR *dirp) ;
void yaffs_rewinddir(yaffs_DIR *dirp) ;
int yaffs_closedir(yaffs_DIR *dirp) ;

int yaffs_mount(const YCHAR *path) ;
#ifdef YAFFS_EXTEND_SUPPORT
int yaffs_mount2(const YCHAR *path, int read_only);
int yaffs_mount3(const YCHAR *path, int read_only, int skip_checkpt);
#endif

int yaffs_unmount(const YCHAR *path) ;
int yaffs_unmount2(const YCHAR *path, int force);
#ifdef YAFFS_EXTEND_SUPPORT
int yaffs_remount(const YCHAR *path, int force, int read_only);

int yaffs_format(const YCHAR *path,
		int unmount_flag,
		int force_unmount_flag,
		int remount_flag);

/*
 * yaffs_sync() does a full sync, including checkpoint.
 * yaffs_sync_files() just flushes the cache and does not write a checkpoint.
 */
int yaffs_sync(const YCHAR *path);
int yaffs_sync_files(const YCHAR *path) ;

int yaffs_symlink(const YCHAR *oldpath, const YCHAR *newpath);
int yaffs_readlink(const YCHAR *path, YCHAR *buf, int bufsiz);

int yaffs_link(const YCHAR *oldpath, const YCHAR *newpath);
int yaffs_mknod(const YCHAR *pathname, mode_t mode, dev_t dev);
#endif

Y_LOFF_T yaffs_freespace(const YCHAR *path);
Y_LOFF_T yaffs_totalspace(const YCHAR *path);

/* Function variants that use a relative directory */
struct yaffs_obj;
int yaffs_open_sharing_reldir(struct yaffs_obj *reldir, const YCHAR *path, int oflag, int mode, int sharing);
int yaffs_open_reldir(struct yaffs_obj *reldir,const YCHAR *path, int oflag, int mode);
#ifdef YAFFS_EXTEND_SUPPORT
int yaffs_truncate_reldir(struct yaffs_obj *reldir, const YCHAR *path, Y_LOFF_T new_size);
#endif
int yaffs_unlink_reldir(struct yaffs_obj *reldir, const YCHAR *path);
int yaffs_rename_reldir(struct yaffs_obj *reldir,
			const YCHAR *oldPath, const YCHAR *newPath);
int yaffs_stat_reldir(struct yaffs_obj *reldir, const YCHAR *path, struct yaffs_stat *buf);
int yaffs_lstat_reldir(struct yaffs_obj *reldir, const YCHAR *path, struct yaffs_stat *buf);
#ifdef YAFFS_EXTEND_SUPPORT
int yaffs_utime_reldir(struct yaffs_obj *reldir, const YCHAR *path, const struct yaffs_utimbuf *buf);
int yaffs_setxattr_reldir(struct yaffs_obj *reldir, const YCHAR *path,
			const char *name, const void *data, int size, int flags);
int yaffs_lsetxattr_reldir(struct yaffs_obj *reldir, const YCHAR *path,
			const char *name, const void *data, int size, int flags);
int yaffs_getxattr_reldir(struct yaffs_obj *reldir, const YCHAR *path,
			const char *name, void *data, int size);
int yaffs_lgetxattr_reldir(struct yaffs_obj *reldir, const YCHAR *path,
			const char *name, void *data, int size);
int yaffs_listxattr_reldir(struct yaffs_obj *reldir, const YCHAR *path,
			char *data, int size);
int yaffs_llistxattr_reldir(struct yaffs_obj *reldir, const YCHAR *path,
			char *data, int size);
int yaffs_removexattr_reldir(struct yaffs_obj *reldir, const YCHAR *path,
			const char *name);
int yaffs_lremovexattr_reldir(struct yaffs_obj *reldir, const YCHAR *path,
			const char *name);
int yaffs_access_reldir(struct yaffs_obj *reldir, const YCHAR *path, int amode);
int yaffs_chmod_reldir(struct yaffs_obj *reldir, const YCHAR *path, mode_t mode);
#endif
int yaffs_mkdir_reldir(struct yaffs_obj *reldir, const YCHAR *path, mode_t mode);
int yaffs_rmdir_reldir(struct yaffs_obj *reldir, const YCHAR *path);
yaffs_DIR *yaffs_opendir_reldir(struct yaffs_obj *reldir, const YCHAR *dirname);
#ifdef YAFFS_EXTEND_SUPPORT
int yaffs_symlink_reldir(struct yaffs_obj *reldir,
			const YCHAR *oldpath, const YCHAR *newpath);
int yaffs_readlink_reldir(struct yaffs_obj *reldir,const YCHAR *path,
			YCHAR *buf, int bufsiz);
int yaffs_link_reldir(struct yaffs_obj *reldir,
			const YCHAR *oldpath, const YCHAR *linkpath);
int yaffs_mknod_reldir(struct yaffs_obj *reldir, const YCHAR *pathname,
		     mode_t mode, dev_t dev);
#endif

/* Function variants that use a relative device */
struct yaffs_dev;
int yaffs_mount_reldev(struct yaffs_dev *dev);
#ifdef YAFFS_EXTEND_SUPPORT
int yaffs_open_sharing_reldev(struct yaffs_dev *dev, const YCHAR *path, int oflag, int mode, int sharing);
int yaffs_open_reldev(struct yaffs_dev *dev,const YCHAR *path, int oflag, int mode);
int yaffs_truncate_reldev(struct yaffs_dev *dev, const YCHAR *path, Y_LOFF_T new_size);
int yaffs_unlink_reldev(struct yaffs_dev *dev, const YCHAR *path);
int yaffs_rename_reldev(struct yaffs_dev *dev,
			const YCHAR *oldPath, const YCHAR *newPath);
int yaffs_stat_reldev(struct yaffs_dev *dev, const YCHAR *path, struct yaffs_stat *buf);
int yaffs_lstat_reldev(struct yaffs_dev *dev, const YCHAR *path, struct yaffs_stat *buf);
int yaffs_utime_reldev(struct yaffs_dev *dev, const YCHAR *path, const struct yaffs_utimbuf *buf);
int yaffs_setxattr_reldev(struct yaffs_dev *dev, const YCHAR *path,
			const char *name, const void *data, int size, int flags);
int yaffs_lsetxattr_reldev(struct yaffs_dev *dev, const YCHAR *path,
			const char *name, const void *data, int size, int flags);
int yaffs_getxattr_reldev(struct yaffs_dev *dev, const YCHAR *path,
			const char *name, void *data, int size);
int yaffs_lgetxattr_reldev(struct yaffs_dev *dev, const YCHAR *path,
			const char *name, void *data, int size);
int yaffs_listxattr_reldev(struct yaffs_dev *dev, const YCHAR *path,
			char *data, int size);
int yaffs_llistxattr_reldev(struct yaffs_dev *dev, const YCHAR *path,
			char *data, int size);
int yaffs_removexattr_reldev(struct yaffs_dev *dev, const YCHAR *path,
			const char *name);
int yaffs_lremovexattr_reldev(struct yaffs_dev *dev, const YCHAR *path,
			const char *name);
int yaffs_access_reldev(struct yaffs_dev *dev, const YCHAR *path, int amode);
int yaffs_chmod_reldev(struct yaffs_dev *dev, const YCHAR *path, mode_t mode);
int yaffs_mkdir_reldev(struct yaffs_dev *dev, const YCHAR *path, mode_t mode);
int yaffs_rmdir_reldev(struct yaffs_dev *dev, const YCHAR *path);
yaffs_DIR *yaffs_opendir_reldev(struct yaffs_dev *dev, const YCHAR *dirname);
int yaffs_symlink_reldev(struct yaffs_dev *dev,
			const YCHAR *oldpath, const YCHAR *newpath);
int yaffs_readlink_reldev(struct yaffs_dev *dev, const YCHAR *path,
			YCHAR *buf, int bufsiz);
int yaffs_link_reldev(struct yaffs_dev *dev,
			const YCHAR *oldpath, const YCHAR *linkpath);
int yaffs_mknod_reldev(struct yaffs_dev *dev, const YCHAR *pathname,
		     mode_t mode, dev_t dev_val);
Y_LOFF_T yaffs_freespace_reldev(struct yaffs_dev *dev);
Y_LOFF_T yaffs_totalspace_reldev(struct yaffs_dev *dev);

/*
 * yaffs_sync_reldev() does a full sync, including checkpoint.
 * yaffs_sync_files_reldev() just flushes the cache and does not write a checkpoint.
 */
int yaffs_sync_reldev(struct yaffs_dev *dev);
int yaffs_sync_files_reldev(struct yaffs_dev *dev);

int yaffs_unmount_reldev(struct yaffs_dev *dev);
int yaffs_unmount2_reldev(struct yaffs_dev *dev, int force);
int yaffs_remount_reldev(struct yaffs_dev *dev, int force, int read_only);

/*
 *  Non standard function to get at objects.
 */
struct yaffs_obj * yaffs_get_obj_from_fd(int handle);

/* Some non-standard functions to use fds to access directories */
struct yaffs_dirent *yaffs_readdir_fd(long fd);
void yaffs_rewinddir_fd(long fd);

/* Non-standard functions to pump garbage collection. */
int yaffs_do_background_gc(const YCHAR *path, int urgency);
#endif
int yaffs_do_background_gc_reldev(struct yaffs_dev *dev, int urgency);

#ifdef YAFFS_EXTEND_SUPPORT
/* Non-standard functions to get usage info */
int yaffs_inodecount(const YCHAR *path);

int yaffs_n_handles(const YCHAR *path);

int yaffs_n_handles_reldir(struct yaffs_obj *reldir, const YCHAR *path);
int yaffs_dump_dev_reldir(struct yaffs_obj *reldir, const YCHAR *path);
int yaffs_n_handles_reldev(struct yaffs_dev *dev, const YCHAR *path);
int yaffs_dump_dev_reldev(struct yaffs_dev *dev, const YCHAR *path);

#ifdef CONFIG_YAFFS_WINCE
int yaffs_set_wince_times(int fd,
			const unsigned *wctime,
			const unsigned *watime,
			const unsigned *wmtime);
int yaffs_get_wince_times(int fd,
			unsigned *wctime,
			unsigned *watime,
			unsigned *wmtime);
#endif
#endif

#define YAFFS_SHARE_READ  1
#define YAFFS_SHARE_WRITE 2

#ifdef YAFFS_EXTEND_SUPPORT
int yaffs_open_sharing(const YCHAR *path, int oflag, int mode, int shareMode);
#endif

struct yaffs_dev;
void yaffs_add_device(struct yaffs_dev *dev);

int yaffs_start_up(void);
int yaffsfs_GetLastError(void);

/* Function to check if the device list is empty */
int yaffs_device_empty(void);

/* Functions to iterate through devices. NB Use with extreme care! */
void yaffs_dev_rewind(void);
struct yaffs_dev *yaffs_next_dev(void);

#ifdef YAFFS_EXTEND_SUPPORT
/* Function to get the last error */
int yaffs_get_error(void);
const char *yaffs_error_to_str(int err);

/* Function only for debugging */
void *yaffs_getdev(const YCHAR *path);
int yaffs_dump_dev(const YCHAR *path);
int yaffs_set_error(int error);
#endif

#ifdef YAFFS_VERIFY
/* Trace control functions */
unsigned  yaffs_set_trace(unsigned tm);
unsigned  yaffs_get_trace(void);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __YAFFSFS_H__ */
