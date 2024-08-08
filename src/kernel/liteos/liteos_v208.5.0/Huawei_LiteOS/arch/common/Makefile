include $(LITEOSTOPDIR)/config.mk

MODULE_NAME := archcommon
ifeq ($(LOSCFG_ARCH_MMU_ENABLE), y)
LOCAL_SRCS :=  $(wildcard mmu/src/*.c )
endif
LOCAL_FLAGS := $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)
LOCAL_SRCS_NEED_SORT := y

include $(MODULE)
