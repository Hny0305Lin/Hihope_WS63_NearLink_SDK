include $(LITEOSTOPDIR)/config.mk
LIBC_RELATIVE_PATH := ../../$(LITEOS_OPENSOURCE_PATHNAME)/musl
include libc_src.mk

MODULE_NAME := c

# libc module
LOCAL_SRCS_LIBC := $(LIBC_STRING_SRC)
ifneq ($(LOSCFG_SAFETY_KERNEL), y)
LOCAL_SRCS_LIBC += $(LIBC_ERRNO_SRC) $(LIBC_STDLIB_SRC) \
					$(LIBC_INTERNAL_SRC) $(LIBC_TIME_SRC) $(LIBC_MISC_SRC) \
					$(LIBC_LOCALE_SRC) $(LIBC_CTYPE_SRC) $(LIBC_TEMP_SRC) \
					$(LIBC_FCNTL_SRC) $(LIBC_SIGNAL_SRC) $(LIBC_NETWORK_SRC) \
					$(LIBC_UNISTD_SRC) $(LIBC_COMPLEX_SRC) $(LIBC_MULTIBYTE_SRC) \
					$(LIBC_EXIT_SRC) $(LIBC_LEGACY_SRC)
endif

ifeq ($(LOSCFG_ARCH_FPU_ENABLE)_$(LOSCFG_ARCH_ARM_AARCH64), _y)
FILTER_FILES_LIBC := $(LIBC_RELATIVE_PATH)/src/complex/cabs.c \
                     $(LIBC_RELATIVE_PATH)/src/internal/floatscan.c \
                     $(LIBC_RELATIVE_PATH)/src/stdlib/atof.c \
                     $(LIBC_RELATIVE_PATH)/src/stdlib/strtod.c \
                     $(LIBC_RELATIVE_PATH)/src/time/difftime.c \
                     $(LIBC_RELATIVE_PATH)/src/locale/strtod_l.c
LOCAL_SRCS_LIBC := $(filter-out $(FILTER_FILES_LIBC), $(LOCAL_SRCS_LIBC))
endif

# liteos_libc module
LOCAL_DIRS_LITEOS_LIBC := string exit
ifneq ($(LOSCFG_SAFETY_KERNEL), y)
LOCAL_DIRS_LITEOS_LIBC += errno stdlib signal tzdst lock locale init
endif
LOCAL_DIRS_LITEOS_LIBC := $(addprefix ../liteos_libc/, $(LOCAL_DIRS_LITEOS_LIBC))

LOCAL_DIRS_y := $(LOCAL_DIRS_LITEOS_LIBC)
LOCAL_SRCS_y += $(LOCAL_SRCS_LIBC)
LOCAL_SRCS_y += $(foreach dir, $(LOCAL_DIRS_y), $(wildcard $(dir)/*.c))

ifeq ($(LOSCFG_LIB_STDIO), y)
	LOCAL_SRCS_y := $(filter-out ../liteos_libc/exit/assert.c, $(LOCAL_SRCS_y))
endif

# standard io base
LOCAL_SRCS_y += $(LIBC_STDIO_BASE)
# standard io file api
LOCAL_SRCS_$(LOSCFG_LIB_STDIO) += $(LIBC_STDIO_SRC)
# standard format api
LOCAL_SRCS_$(LOSCFG_LIB_FORMAT) += $(LIBC_FORMAT_SRC)

ifneq ($(LOSCFG_SAFETY_KERNEL), y)
LOCAL_SRCS_y += ../liteos_libc/time/time.c ../liteos_libc/time/time64.c \
                ../liteos_libc/misc/getentropy.c ../liteos_libc/time/localtime_r.c \
                ../liteos_libc/time/mktime.c ../liteos_libc/time/clock_gettime.c

ifneq ($(LOSCFG_COMPAT_POSIX), y)
LOCAL_SRCS_y += $(LIBC_RELATIVE_PATH)/src/prng/rand.c
endif
endif

LOCAL_SRCS    = $(LOCAL_SRCS_y)

ifeq ($(findstring $(LOSCFG_ARCH_ARM_AARCH64)$(LOSCFG_ARCH_ARM_CORTEX_M), y y),)
LOCAL_SRCS += $(wildcard $(LIBC_RELATIVE_PATH)/src/string/$(ARCH_LOCAL)/*.S)
endif

FILTER_FILES :=
ifneq ($(findstring $(LOSCFG_ARCH_ARM_V7A)$(LOSCFG_ARCH_ARM_V7R)$(LOSCFG_ARCH_CORTEX_R52), y y y),)
FILTER_FILES += $(wildcard $(LIBC_RELATIVE_PATH)/src/string/memcpy.c) \
                $(wildcard $(LIBC_RELATIVE_PATH)/src/string/strcmp.c) \
                $(wildcard $(LIBC_RELATIVE_PATH)/src/string/memcmp.c)
ifeq ($(LOSCFG_ARCH_FPU_VFP_NEON), y)
FILTER_FILES += $(wildcard $(LIBC_RELATIVE_PATH)/src/string/memset.c)
endif
else ifneq ($(findstring $(LOSCFG_ARCH_RISCV32), y),)
FILTER_FILES += $(wildcard $(LIBC_RELATIVE_PATH)/src/string/memset.c) \
                $(wildcard $(LIBC_RELATIVE_PATH)/src/string/strcmp.c) \
                $(wildcard $(LIBC_RELATIVE_PATH)/src/string/memcpy.c)
else ifneq ($(findstring $(LOSCFG_ARCH_ARM_AARCH64), y),)
FILTER_FILES += $(wildcard $(LIBC_RELATIVE_PATH)/src/string/strcmp.c)
ifeq ($(LOSCFG_ARCH_FPU_ENABLE), y)
# memcmp: Alignment fault
# memcpy: Alignment fault
# memset: floating pointer unit is not supported
FILTER_FILES += $(wildcard $(LIBC_RELATIVE_PATH)/src/string/memset.c) \
                $(wildcard $(LIBC_RELATIVE_PATH)/src/string/memcpy.c) \
                $(wildcard $(LIBC_RELATIVE_PATH)/src/string/memcmp.c)
endif
endif
LOCAL_SRCS := $(filter-out $(FILTER_FILES), $(LOCAL_SRCS))

LOCAL_INCLUDE := \
    -I $(LITEOS_LIBC_PATH)/src/include \
    -I $(LITEOS_LIBC_PATH)/src/internal \
    -I $(LITEOS_LIBC_PATH)/src/crypt \
    -I $(LITEOS_LIBC_PATH)/src/ctype \
    -I $(LITEOS_LIBC_PATH)/src/dirent \
    -I $(LITEOS_LIBC_PATH)/src/errno \
    -I $(LITEOS_LIBC_PATH)/src/ipc \
    -I $(LITEOS_LIBC_PATH)/src/locale \
    -I $(LITEOS_LIBC_PATH)/src/math \
    -I $(LITEOS_LIBC_PATH)/src/multibyte \
    -I $(LITEOS_LIBC_PATH)/src/networknetwork \
    -I $(LITEOS_LIBC_PATH)/src/passwd \
    -I $(LITEOS_LIBC_PATH)/src/prng \
    -I $(LITEOS_LIBC_PATH)/src/process \
    -I $(LITEOS_LIBC_PATH)/src/regex \
    -I $(LITEOS_LIBC_PATH)/src/search \
    -I $(LITEOS_LIBC_PATH)/src/time \
    -I $(LITEOSTOPDIR)/lib/liteos_libc/pthread \
    -I $(LITEOSTOPDIR)/lib/liteos_libc/tzdst \
    -I $(LITEOSTOPDIR)/lib/liteos_libc/lock \
    -I $(LITEOSTOPDIR)/lib/liteos_libc/locale \
    -I $(LITEOSTOPDIR)/lib/liteos_libc/init

ifeq ($(LOSCFG_LLTSER), y)
LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/internal/tools/gcov_ser
endif

ifeq ($(LOSCFG_HW_RANDOM_ENABLE), y)
LOCAL_INCLUDE += -I ${LITEOSTOPDIR}/drivers/random/include
endif

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS)

LOCAL_CFLAGS += -Wunused -Wno-unused-value -Wno-parentheses -Wno-strict-prototypes -Wdate-time
ifeq ($(LOSCFG_COMPILER_CLANG), y)
    LOCAL_CFLAGS += -Wno-uninitialized
else
    LOCAL_CFLAGS += -Wno-maybe-uninitialized -Wno-unused-but-set-variable
endif
ifneq ($(LOSCFG_COMPILER_XTENSA_32), y)
    LOCAL_CFLAGS += -Wvla
endif

ifeq ($(LOSCFG_LMS_LIBC_FULL_CHECK), y)
LOCAL_CFLAGS += -fsanitize=kernel-address
endif

include $(MODULE_LIBC)
