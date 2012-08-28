#选项 -rdynamic 用来通知链接器将所有符号添加到动态符号表中r(目的是能够通过使用 dlopen 来实现向后跟踪)
EXE=el-server.exe
CC=g++

#编译选项
USER_NAME=$(shell whoami)
EL_LIB_PATH=/home/$(USER_NAME)/endless/
EL_LIBS_FLAG=-Wl,-rpath,$(ICE_LIB_PATH)
FLAG=-Wall -DDEBUG -DENABLE_TRACE_LOG -g -O0 -o
LOCAL_INCLUDE=../include/

#包含的头文件
INCLUDES=-I/usr/include/ -I$(EL_LIB_PATH)lib_include/ -I$(LOCAL_INCLUDE)
GLIB_INCLUDES=-I/usr/include/glib-2.0/ -I/usr/lib/glib-2.0/include/

LIBS=-L/usr/lib/ -L$(EL_LIB_PATH)

LINK=-ldl -rdynamic -lglib-2.0 -lel $(EL_LIBS_FLAG)

#编译的代码文件
LOCAL_SRCS =$(wildcard ./*.c)
OTHER_SRCS =

$(EXE):
	$(CC) $(LOCAL_SRCS) $(OTHER_SRCS) $(INCLUDES) $(GLIB_INCLUDES) $(LIBS) $(LINK) $(FLAG) $@
#	rm ./bin/$(EXE)
	cp $(EXE) $(EL_LIB_PATH)
	cp -rf $(LOCAL_INCLUDE) $(EL_LIB_PATH)service_include
	rm -f $(EXE)
clean:
	rm -f $(EXE)

