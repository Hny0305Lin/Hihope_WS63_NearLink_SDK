# dir errno
set(LIBC_ERRNO_SRC strerror.c)
list(TRANSFORM LIBC_ERRNO_SRC PREPEND ${LIBC_DIR_PATH}/src/errno/)

# dir string
set(LIBC_STRING_SRC
    bcmp.c bcopy.c bzero.c memchr.c memcmp.c memcpy.c memmove.c
	memrchr.c memset.c stpcpy.c stpncpy.c strcasecmp.c strcasestr.c strcat.c
	strchr.c strchrnul.c strcmp.c strcpy.c strcspn.c strdup.c strerror_r.c
	strlcpy.c strlen.c strncasecmp.c strncat.c strncmp.c strncpy.c strnlen.c
	strpbrk.c strrchr.c strsep.c strspn.c strstr.c strtok.c strtok_r.c wcschr.c
	wcscmp.c wcslen.c wcsncmp.c wcsncpy.c wcsnlen.c wcsstr.c wmemchr.c wmemcmp.c
	wmemcpy.c wmemmove.c wmemset.c
)
list(TRANSFORM LIBC_STRING_SRC PREPEND ${LIBC_DIR_PATH}/src/string/)

# dir stdlib
set(LIBC_STDLIB_SRC
    abs.c atof.c atoi.c atol.c atoll.c bsearch.c labs.c llabs.c
	qsort.c strtod.c strtol.c wcstol.c qsort_nr.c
)
list(TRANSFORM LIBC_STDLIB_SRC PREPEND ${LIBC_DIR_PATH}/src/stdlib/)

# dir internal
set(LIBC_INTERNAL_SRC floatscan.c intscan.c shgetc.c syscall_ret.c)
list(TRANSFORM LIBC_INTERNAL_SRC PREPEND ${LIBC_DIR_PATH}/src/internal/)

# dir time
set(LIBC_TIME_SRC
    __month_to_secs.c __secs_to_tm.c __tm_to_secs.c __year_to_secs.c
	asctime.c asctime_r.c ctime.c ctime_r.c gmtime.c gmtime_r.c localtime.c
	strftime.c strptime.c time.c wcsftime.c difftime.c
)
list(TRANSFORM LIBC_TIME_SRC PREPEND ${LIBC_DIR_PATH}/src/time/)

# dir misc
set(LIBC_MISC_SRC getopt.c)
if (NOT LOSCFG_LIB_STDIO)
	list(REMOVE_ITEM LIBC_MISC_SRC getopt.c)
endif()
list(TRANSFORM LIBC_MISC_SRC PREPEND ${LIBC_DIR_PATH}/src/misc/)

# dir locale
set(LIBC_LOCALE_SRC
	__lctrans.c c_locale.c langinfo.c strcoll.c strxfrm.c
	wcscoll.c wcsxfrm.c strtod_l.c
)
list(TRANSFORM LIBC_LOCALE_SRC PREPEND ${LIBC_DIR_PATH}/src/locale/)

# dir ctype
set(LIBC_CTYPE_SRC
	__ctype_get_mb_cur_max.c isalnum.c isalpha.c isascii.c isblank.c
	iscntrl.c isdigit.c isgraph.c islower.c isprint.c ispunct.c isspace.c isupper.c
	iswalnum.c iswalpha.c iswblank.c iswcntrl.c iswctype.c iswdigit.c iswgraph.c
	iswlower.c iswprint.c iswpunct.c iswspace.c iswupper.c iswxdigit.c isxdigit.c
	toascii.c tolower.c toupper.c towctrans.c
)
list(TRANSFORM LIBC_CTYPE_SRC PREPEND ${LIBC_DIR_PATH}/src/ctype/)

# dir temp
set(LIBC_TEMP_SRC __randname.c mkostemps.c mkstemp.c mkstemps.c)
list(TRANSFORM LIBC_TEMP_SRC PREPEND ${LIBC_DIR_PATH}/src/temp/)

# dir fcntl
set(LIBC_FCNTL_SRC creat.c)
list(TRANSFORM LIBC_FCNTL_SRC PREPEND ${LIBC_DIR_PATH}/src/fcntl/)

# dir signal
set(LIBC_SIGNAL_SRC sigrtmax.c sigrtmin.c)
list(TRANSFORM LIBC_SIGNAL_SRC PREPEND ${LIBC_DIR_PATH}/src/signal/)

# dir network
set(LIBC_NETWORK_SRC dn_comp.c)
list(TRANSFORM LIBC_NETWORK_SRC PREPEND ${LIBC_DIR_PATH}/src/network/)

# dir unistd
set(LIBC_UNISTD_SRC _exit.c)
list(TRANSFORM LIBC_UNISTD_SRC PREPEND ${LIBC_DIR_PATH}/src/unistd/)

# dir complex
set(LIBC_COMPLEX_SRC cabs.c)
list(TRANSFORM LIBC_COMPLEX_SRC PREPEND ${LIBC_DIR_PATH}/src/complex/)

# dir stdio
set(LIBC_STDIO_SRC
	__fdopen.c __fmodeflags.c __lockfile.c __overflow.c __stdio_close.c
	__stdio_read.c __stdio_seek.c __stdio_write.c __stdout_write.c
	__toread.c __towrite.c __uflow.c asprintf.c clearerr.c fclose.c feof.c ferror.c
	fflush.c fgetc.c fgetpos.c fgets.c fgetwc.c fileno.c fopen.c fprintf.c fputc.c
	fputs.c fputwc.c fread.c freopen.c fscanf.c fseek.c fsetpos.c ftell.c fwide.c
	fwrite.c getc.c getchar.c gets.c getwc.c ofl.c ofl_add.c perror.c printf.c
	putc.c putchar.c puts.c putw.c putwc.c remove.c rewind.c scanf.c setbuf.c
	setbuffer.c setvbuf.c snprintf.c sprintf.c sscanf.c stderr.c stdin.c stdout.c
	swprintf.c tmpnam.c ungetc.c ungetwc.c vasprintf.c vfprintf.c vfscanf.c vfwprintf.c
	vfwscanf.c vprintf.c vscanf.c vsnprintf.c vsprintf.c vsscanf.c vswprintf.c
)
set(LIBC_FORMAT_SRC
	sprintf.c asprintf.c snprintf.c vsprintf.c vasprintf.c vsnprintf.c
	vfprintf.c sscanf.c vsscanf.c vfscanf.c)
set(LIBC_STDIO_BASE __uflow.c __toread.c __towrite.c fwrite.c printf.c)
list(REMOVE_ITEM LIBC_STDIO_SRC ${LIBC_FORMAT_SRC})
list(REMOVE_ITEM LIBC_STDIO_SRC ${LIBC_STDIO_BASE})
list(TRANSFORM LIBC_STDIO_SRC PREPEND ${LIBC_DIR_PATH}/src/stdio/)
list(TRANSFORM LIBC_FORMAT_SRC PREPEND ${LIBC_DIR_PATH}/src/stdio/)
list(TRANSFORM LIBC_STDIO_BASE PREPEND ${LIBC_DIR_PATH}/src/stdio/)

# dir exit
set(LIBC_EXIT_SRC)
if(LOSCFG_LIB_STDIO)
	list(APPEND LIBC_EXIT_SRC assert.c)
endif()
list(TRANSFORM LIBC_EXIT_SRC PREPEND ${LIBC_DIR_PATH}/src/exit/)

# dir prng
set(LIBC_PRNG_SRC __rand48_step.c __seed48.c lrand48.c rand.c random.c seed48.c srand48.c)
list(TRANSFORM LIBC_PRNG_SRC PREPEND ${LIBC_DIR_PATH}/src/prng/)

# dir multibyte
set(LIBC_MULTIBYTE_SRC
	btowc.c c32rtomb.c internal.c mblen.c mbrlen.c mbrtoc32.c
	mbrtowc.c mbsinit.c mbsnrtowcs.c mbsrtowcs.c mbstowcs.c mbtowc.c wcrtomb.c
	wcsnrtombs.c wcsrtombs.c wctob.c wctomb.c
)
list(TRANSFORM LIBC_MULTIBYTE_SRC PREPEND ${LIBC_DIR_PATH}/src/multibyte/)

# dir linux
set(LIBC_LINUX_SRC stime.c)
list(TRANSFORM LIBC_LINUX_SRC PREPEND ${LIBC_DIR_PATH}/src/linux/)

# dir legacy
set(LIBC_LEGACY_SRC err.c getpagesize.c)
if (NOT LOSCFG_LIB_STDIO)
	list(REMOVE_ITEM LIBC_LEGACY_SRC err.c)
endif()
list(TRANSFORM LIBC_LEGACY_SRC PREPEND ${LIBC_DIR_PATH}/src/legacy/)

# dir thread
set(LIBC_THREAD_SRC pthread_getspecific.c pthread_key_create.c pthread_setspecific.c)
list(TRANSFORM LIBC_THREAD_SRC PREPEND ${LIBC_DIR_PATH}/src/thread/)
