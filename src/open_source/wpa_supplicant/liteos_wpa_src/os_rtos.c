/*
 * OS specific functions for UNIX/POSIX systems
 * Copyright (c) 2005-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "time.h"
#include "os.h"
#include "utils/common.h"
#include "utils/list.h"
#include "malloc.h"
#include "securec.h"
#include "crypto.h"

unsigned long os_get_ticks(void)
{
	return (unsigned long)os_tick_cnt_get();
}

void os_sleep(os_time_t sec, os_time_t usec)
{
	if (sec)
		(void)sleep(sec);
	if (usec)
		(void)usleep(usec);
}

int os_get_time(struct os_time *t)
{
	int res;
	struct timeval tv;
	if (t ==  NULL)
		return -1;
	res = gettimeofday(&tv, NULL);
	t->sec = tv.tv_sec;
	t->usec = tv.tv_usec;
	return res;
}

int os_get_reltime(struct os_reltime *t)
{
#if defined(CLOCK_BOOTTIME)
	static clockid_t clock_id = CLOCK_BOOTTIME;
#elif defined(CLOCK_MONOTONIC)
	static clockid_t clock_id = CLOCK_MONOTONIC;
#else
	static clockid_t clock_id = CLOCK_REALTIME;
#endif
	struct timespec ts;
	int res;
	if (t == NULL)
		return -1;
	while (1) {
		res = clock_gettime(clock_id, &ts);
		if (res == 0) {
			t->sec = ts.tv_sec;
			t->usec = ts.tv_nsec / 1000;
			return 0;
		}
		switch (clock_id) {
#ifdef CLOCK_BOOTTIME
			case CLOCK_BOOTTIME:
				clock_id = CLOCK_MONOTONIC;
				break;
#endif
#ifdef CLOCK_MONOTONIC
			case CLOCK_MONOTONIC:
				clock_id = CLOCK_REALTIME;
				break;
#endif
			case CLOCK_REALTIME:
				return -1;
			default:
				return -1;
		}
	}
}

#ifdef LOS_INLINE_FUNC_CROP
int os_reltime_before(struct os_reltime *a,
				    struct os_reltime *b)
{
	if ((a == NULL) || (b == NULL))
		return 0;
	return (a->sec < b->sec) ||
	       (a->sec == b->sec && a->usec < b->usec);
}

void os_reltime_sub(struct os_reltime *a, struct os_reltime *b,
				  struct os_reltime *res)
{
	if ((a != NULL) && (b != NULL) && (res != NULL)) {
		res->sec = a->sec - b->sec;
		res->usec = a->usec - b->usec;
		if (res->usec < 0) {
			res->sec--;
			res->usec += 1000000;
		}
	}
}
#endif /* LOS_INLINE_FUNC_CROP */

int os_mktime(int year, int month, int day, int hour, int min, int sec,
		  os_time_t *t)
{
	struct tm tm;
	struct tm *tm1 = NULL;
	time_t t_local, t1, t2;
	os_time_t tz_offset;
	if (t == NULL)
		return -1;
	if (year < 1970 || month < 1 || month > 12 || day < 1 || day > 31 ||
		hour < 0 || hour > 23 || min < 0 || min > 59 || sec < 0 ||
		sec > 60)
		return -1;

	(void)os_memset(&tm, 0, sizeof(tm));
	tm.tm_year = year - 1900;
	tm.tm_mon = month - 1;
	tm.tm_mday = day;
	tm.tm_hour = hour;
	tm.tm_min = min;
	tm.tm_sec = sec;

	t_local = mktime(&tm);

	/* figure out offset to UTC */
	tm1 = localtime(&t_local);
	if (tm1 != NULL) {
		t1 = mktime(tm1);
		tm1 = gmtime(&t_local);
		if (tm1) {
			t2 = mktime(tm1);
			tz_offset = t2 - t1;
		} else
			tz_offset = 0;
	} else
		tz_offset = 0;

	*t = (os_time_t)t_local - tz_offset;
	return 0;
}

int os_gmtime(os_time_t t, struct os_tm *tm)
{
	struct tm *tm2 = NULL;
	time_t t2 = t;

	tm2 = gmtime(&t2);
	if ((tm2 == NULL) || (tm == NULL))
		return -1;
	tm->sec = tm2->tm_sec;
	tm->min = tm2->tm_min;
	tm->hour = tm2->tm_hour;
	tm->day = tm2->tm_mday;
	tm->month = tm2->tm_mon + 1;
	tm->year = tm2->tm_year + 1900;
	return 0;
}

int os_get_random(unsigned char *buf, size_t len)
{
	if (crypto_get_random(buf, len) != 0)
		return -1;
	return 0;
}

unsigned long os_random(void)
{
	return rand();
}

char * os_rel2abs_path(const char *rel_path)
{
	(void)rel_path;
	return NULL;
}

int os_setenv(const char *name, const char *value, int overwrite)
{
	(void)name;
	(void)value;
	(void)overwrite;
	return 0;
}

int os_unsetenv(const char *name)
{
	(void)name;
	return 0;
}

char * os_readfile(const char *name, size_t *len)
{
	(void)name;
	(void)len;
	return NULL;
}

int os_file_exists(const char *fname)
{
	(void)fname;
	return 1;
}

size_t os_strlcpy(char *dst, const char *src, size_t n)
{
	return strlcpy(dst, src, n);
}

void *os_malloc(size_t size)
{
	void *ptr = malloc(size);
	return ptr;
}

void *os_realloc(void *ptr, size_t size)
{
	void *reptr = realloc(ptr, size);
	return reptr;
}

void os_free(void *ptr)
{
	if (ptr != NULL)
		free(ptr);
}

__attribute__((weak)) void os_free_drv_scan_bss(void *ptr)
{
    return;
}

void *os_zalloc(size_t size)
{
	void *ptr = malloc(size);
    if (ptr != NULL) {
        memset(ptr, 0, size);
    }
	return ptr;
}

#ifdef LOS_INLINE_FUNC_CROP
void * os_calloc(size_t nmemb, size_t size)
{
	if (size && nmemb > (~(size_t)0) / size)
		return NULL;
	return os_zalloc(nmemb * size);
}
#endif /* LOS_INLINE_FUNC_CROP */

char *os_strdup(const char *src)
{
	void *ptr = strdup(src);
	return ptr;
}

int os_memcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1, s2, n);
}

void *os_memset(void*  dst, int c, size_t n)
{
	return memset(dst, c, n);
}

void *os_memcpy(void *dst, const void *src, size_t n)
{
	return memcpy(dst, src, n);
}

void * os_memdup(const void *src, size_t len)
{
	void *r = os_malloc(len);
	if (r != NULL)
		(void)os_memcpy(r, src, len);
	return r;
}

int os_strcmp(const char *str1, const char *str2)
{
	return strcmp(str1, str2);
}

int os_strcasecmp(const char *str1, const char *str2)
{
	return strcmp(str1, str2);
}

int os_strncmp(const char *str1, const char *str2, size_t n)
{
	return strncmp(str1, str2, n);
}

int os_memcmp_const(const void *a, const void *b, size_t len)
{
	const u8 *aa = a;
	const u8 *bb = b;
	size_t i;
	u8 res;
	if ((aa == NULL) || (bb == NULL))
		return -1;
	for (res = 0, i = 0; i < len; i++)
		res |= aa[i] ^ bb[i];

	return res;
}

char *os_strstr(const char *str1, const char *str2)
{
	return strstr(str1, str2);
}

char *os_strchr(const char *src, int c)
{
	return strchr(src, c);
}

char *os_strrchr(const char *src, int c)
{
	return strrchr(src, c);
}

void *os_memmove(void *dst, const void *src, size_t n)
{
	return memmove(dst, src, n);
}

size_t os_strlen(const char *src)
{
	return strlen(src);
}

int os_strncasecmp(const char *s1, const char *s2, size_t n)
{
	return strncasecmp(s1, s2, n);
}
