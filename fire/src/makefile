#选项 -rdynamic 用来通知链接器将所有符号添加到动态符号表中r(目的是能够通过使用 dlopen 来实现向后跟踪)
EXE=fire-server.exe
CC=g++

#编译选项
ICE_LIB_PATH=../../lib/
ICE_LIBS_FLAG=-Wl,-rpath,$(ICE_LIB_PATH)
FLAG=-Wall -DDEBUG -DENABLE_TRACE_LOG -g -O0 -o

#包含的头文件
INCLUDES=-I/usr/include/ -I../../lib/include/ -I../include/
GLIB_INCLUDES=-I/usr/include/glib-2.0/ -I/usr/lib/glib-2.0/include/

LIBS=-L/usr/lib/ -L$(ICE_LIB_PATH)

LINK=-ldl -rdynamic -lglib-2.0 -lice $(ICE_LIBS_FLAG)

#编译的代码文件
LOCAL_SRCS =$(wildcard ./*.c)
OTHER_SRCS =

$(EXE):
	$(CC) $(LOCAL_SRCS) $(OTHER_SRCS) $(INCLUDES) $(GLIB_INCLUDES) $(LIBS) $(LINK) $(FLAG) $@
#	rm ./bin/$(EXE)
	cp $(EXE) ../bin/
	rm -f $(EXE)
clean:
	rm -f $(EXE)

