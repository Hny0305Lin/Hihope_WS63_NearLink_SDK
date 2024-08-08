include $(LITEOSTOPDIR)/config.mk
LIBC_RELATIVE_PATH := ../../$(LITEOS_OPENSOURCE_PATHNAME)/musl

MODULE_NAME := m

LOCAL_SRCS = $(wildcard $(LIBC_RELATIVE_PATH)/src/math/*.c)

LOCAL_INCLUDE := \
        -I $(LITEOS_LIBC_PATH)/src/include \
        -I $(LITEOS_LIBC_PATH)/src/internal

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS)
LOCAL_CFLAGS += -Wno-parentheses -Wno-strict-prototypes
ifeq ($(LOSCFG_COMPILER_CLANG), y)
    LOCAL_CFLAGS += -Wno-uninitialized -Wno-ignored-pragmas
else
    LOCAL_CFLAGS += -Wno-maybe-uninitialized -Wno-unknown-pragmas \
                    -Wno-unused-but-set-variable
endif

# C++ library symbols in the compiler conflict with those in the LiteOS libm #
ifeq ($(LOSCFG_COMPILER_ARM_NONE_EABI)$(LOSCFG_KERNEL_CPPSUPPORT), yy)
FILTER_FILES += $(wildcard $(LIBC_RELATIVE_PATH)/src/math/frexpl.c) \
                $(wildcard $(LIBC_RELATIVE_PATH)/src/math/ceill.c)

LOCAL_SRCS := $(filter-out $(FILTER_FILES), $(LOCAL_SRCS))
endif

include $(MODULE_LIBC)
