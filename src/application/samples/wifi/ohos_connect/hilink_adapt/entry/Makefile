#当前路径
PWD := .
BASE_PATH := $(PWD)/.

CC :=/usr/bin/gcc
AR :=/usr/bin/ar

#CC := /opt/toolchains/yocto-32bit-toolchain/sysroots/x86_64-oesdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc
#AR := /opt/toolchains/yocto-32bit-toolchain/sysroots/x86_64-oesdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-ar

#头文件路径
INCLUDE_PATH := -I$(BASE_PATH)/include

#适配源文件
SOURCE_FILES := $(wildcard $(BASE_PATH)/*.c)

#库文件路径
LIB_FILE_PATH := $(BASE_PATH)/../lib

#库文件
LIB_FILES := -lhilinkbtsdk -lpthread

#编译出的.o文件
OBJECTS_PATH := objs
OBJECTS := $(patsubst %.c, %.o, $(SOURCE_FILES))

#编译选项
COMPILE_FLAGS := -g -O $(INCLUDE_PATH)
LINK_FLAGS := -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack --data-sections -L$(LIB_FILE_PATH) $(LIB_FILES)

#编译开始
.PHONY: all
all: clean demo

#清理
clean:
	rm -f $(OBJECTS)

#进程
demo: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LINK_FLAGS)

#编译.o
$(OBJECTS):%.o : %.c
	$(CC) $(COMPILE_FLAGS) -c $< -o $@
