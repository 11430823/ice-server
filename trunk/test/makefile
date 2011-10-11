EXE=ice-server.exe
CC=g++

#编译选项
#FLAG=-Wall -DDEBUG -g -o
FLAG=-w -DDEBUG -g -o0 -o

#包含的头文件
INCLUDES=-I/usr/include -I../include/
GLIB_INCLUDES=


LIBS=-L/usr/lib/

LINK=-ldl

#编译的代码文件
LOCAL_SRCS =$(wildcard ./*.c)
OTHER_SRCS =

$(EXE):
	$(CC) $(LOCAL_SRCS) $(OTHER_SRCS) $(INCLUDES) $(GLIB_INCLUDES) $(LINK) $(LIBS) $(FLAG) $@
clean:
	rm -f $(EXE)

