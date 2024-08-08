export SDK_ROOT:=$(abspath $(shell pwd)/../../../../..)

export CHIP=REPLACE_CHIP
export CORE=REPLACE_CORE
export ARCH=REPLACE_ARCH
export ARCH_FAMILY=REPLACE_ARCH_FAMILY
export TARGET_NAME=REPLACE_TARGET_NAME
export KERNEL=REPLACE_KERNEL
export LOG_DEF_LIST=REPLACE_LOG_DEF_LIST
export PKG_TARGET_NAME=REPLACE_PKG_TARGET_NAME

export OUTPUT_DIR:=$(SDK_ROOT)/output/$(CHIP)/$(CORE)/$(PKG_TARGET_NAME)-makefile
export HSO_TMP:=$(SDK_ROOT)/output/hso_temp

export HSO_MK_XML_PY:= $(SDK_ROOT)/build/script/hdbxml/mk_hso_prim_xml.py
export HSO_XML_PRE_PROCESS_PY:= $(SDK_ROOT)/build/script/hdbxml/process_pregenerated_xml.py
export HSO_XML_MERGE_PY:= $(SDK_ROOT)/build/script/hdbxml/hso_prim_xml_merge.py
export HSO_XML_DB_CREATE_PY:= $(SDK_ROOT)/build/script/hdbxml/database_create.py
export BUILD_UTILS_PY:=$(SDK_ROOT)/build/script/utils/build_utils.py
export CCACHE := ccache
ifeq ("$(KERNEL)", "liteos")
export LOS_PUB_CCFLAGS=REPLACE_LOS_PUB_CCFLAGS
export LOS_ROOT=REPLACE_LOS_ROOT
export LITEOS_PLATFORM_MENUCONFIG_H=$(OUTPUT_DIR)/menuconfig/menuconfig.h
endif
export COMPONENT_LIST=REPLACE_COMPONENT_LIST
export BIN_NAME=REPLACE_BIN_NAME
LINKFLAGS=REPLACE_LINKFLAGS
LINK_SCRIPT=REPLACE_LINK_SCRIPT

PUBLIC_INCLUDES_TEMP=REPLACE_PUBLIC_INCLUDES
export PUBLIC_INCLUDES = $(patsubst %,-I%,$(PUBLIC_INCLUDES_TEMP))
export PUBLIC_CCFLAGS=REPLACE_PUBLIC_CCFLAGS
PUBLIC_DEFINES_TEMP=REPLACE_PUBLIC_DEFINES
export PUBLIC_DEFINES = $(patsubst %,-D%,$(PUBLIC_DEFINES_TEMP))

export LIB_DIR = $(OUTPUT_DIR)/libs
export WHOLE_LINK_LIBS =
export NORMAL_LINK_LIBS =
export WHOLE_EXTERN_LINK_LIBS =
export NORMAL_EXTERN_LINK_LIBS =

-include ./toolchains.make

ELF_FILE=$(OUTPUT_DIR)/$(BIN_NAME).elf
MAP_FILE=$(OUTPUT_DIR)/$(BIN_NAME).map
all: pre_build $(ELF_FILE) post_build

COMPONENT_LIST_LIB = $(patsubst %,lib%.a,$(COMPONENT_LIST))

pre_build:
	@rm -fr ${OUTPUT_DIR}
	@mkdir -p $(OUTPUT_DIR)/libs
	@mkdir -p $(HSO_TMP)
ifeq ("$(KERNEL)", "liteos")
	@mkdir -p $(dir $(LITEOS_PLATFORM_MENUCONFIG_H))
	@$(LOS_ROOT)/Huawei_LiteOS/build/make/make_menuconfig_headfile.sh $(LOS_ROOT)/Huawei_LiteOS/tools/build/config/$(CHIP).config $(LITEOS_PLATFORM_MENUCONFIG_H)
endif
	@python3 $(HSO_MK_XML_PY) mkdir ${SDK_ROOT}/ ${CHIP} ${CORE}
	@$(CC) -P -xc -E -o $(OUTPUT_DIR)/linker.lds $(PUBLIC_INCLUDES) $(PUBLIC_CCFLAGS) $(PUBLIC_DEFINES) $(LINK_SCRIPT)
	@echo built $(OUTPUT_DIR)/linker.lds
	@echo pre_build

post_build:$(ELF_FILE)
ifneq ($(build_level), release)
	$(OBJDUMP) -x -S -l $(ELF_FILE) > $(OUTPUT_DIR)/$(BIN_NAME).lst
	$(OBJDUMP) -d -m $(ARCH_FAMILY) $(ELF_FILE) > $(OUTPUT_DIR)/$(BIN_NAME).asm
	$(NM) -S -n --format=sysv  $(ELF_FILE) > $(OUTPUT_DIR)/$(BIN_NAME).nm
	$(OBJDUMP) -Wi $(ELF_FILE) > $(OUTPUT_DIR)/$(BIN_NAME).info
endif
	$(OBJCOPY) --gap-fill 0xFF -O binary -R .logstr -R .ARM -R .ARM  $(ELF_FILE) $(OUTPUT_DIR)/$(BIN_NAME).bin
ifeq ("$(BIN_NAME).bin", "ssb.bin")
	@python3 $(BUILD_UTILS_PY) add_len_and_sha256_info_to_ssb $(OUTPUT_DIR)/$(BIN_NAME).bin $(CHIP)
else
ifneq ($(nhso), true)
	@echo "Merge HSO_XML & Create HSO_DB"
	@python3 ${HSO_XML_PRE_PROCESS_PY} ${SDK_ROOT}/ ${CHIP} ${CORE}
	@python3 ${HSO_XML_MERGE_PY} ${SDK_ROOT}/ ${CHIP} ${CORE}
	@python3 ${HSO_XML_MERGE_PY} ${SDK_ROOT}/ ${CHIP} "dsp_core"
	@python3 ${HSO_XML_MERGE_PY} ${SDK_ROOT}/ ${CHIP} "bt_core"
	@python3 ${HSO_XML_DB_CREATE_PY} ${SDK_ROOT}/ ${CHIP}
endif
endif
	@rm $(HSO_TMP) -fr
	@echo post_build

STD_LIB_DIR = REPLACE_STD_LIB_DIR
STD_LIBS = REPLACE_STD_LIBS

-include $(patsubst %,./components/%.make,$(COMPONENT_LIST))
$(ELF_FILE):pre_build $(COMPONENT_LIST_LIB)
	@echo buiilding $(ELF_FILE)
	@$(CC) $(LINKFLAGS) -Wl,-T$(OUTPUT_DIR)/linker.lds $(patsubst %,-L%,$(LIB_DIR)) -Wl,--whole-archive $(patsubst %,-l%,$(WHOLE_LINK_LIBS)) $(WHOLE_EXTERN_LINK_LIBS) -Wl,--no-whole-archive $(NORMAL_EXTERN_LINK_LIBS) $(patsubst %,-l%,$(NORMAL_LINK_LIBS)) $(patsubst %,-L%,$(STD_LIB_DIR)) -Wl,--start-group $(patsubst %,-l%,$(STD_LIBS)) -Wl,--end-group -Wl,-Map=$(MAP_FILE) -o $(ELF_FILE)