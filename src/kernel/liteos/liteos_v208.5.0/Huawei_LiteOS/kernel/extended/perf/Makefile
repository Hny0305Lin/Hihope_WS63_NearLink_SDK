include $(LITEOSTOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS := $(wildcard *.c)

LOCAL_INCLUDE := \
        -I $(LITEOSTOPDIR)/kernel/base/include \
        -I $(LITEOSTOPDIR)/kernel/extended/include \
        -I $(LITEOSTOPDIR)/kernel/extended/perf

ifeq ($(LOSCFG_PERF_HW_PMU), y)
LOCAL_SRCS += $(wildcard pmu/perf_hw_pmu.c)
endif

ifeq ($(LOSCFG_PERF_TIMED_PMU), y)
LOCAL_SRCS += $(wildcard pmu/perf_timed_pmu.c)
endif

ifeq ($(LOSCFG_PERF_SW_PMU), y)
LOCAL_SRCS += $(wildcard pmu/perf_sw_pmu.c)
endif
LOCAL_SRCS_NEED_SORT := y

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)

include $(MODULE)

