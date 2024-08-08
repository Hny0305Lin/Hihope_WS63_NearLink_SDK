LITEOS_BASELIB          += -larchcommon

LIB_SUBDIRS             += arch/common/

ifeq ($(LOSCFG_ARCH_MMU_ENABLE), y)
ARCH_INCLUDE            := -I $(LITEOSTOPDIR)/arch/common/mmu/include
endif

LITEOS_PLATFORM_INCLUDE += $(ARCH_INCLUDE)
LITEOS_CXXINCLUDE       += $(ARCH_INCLUDE)
