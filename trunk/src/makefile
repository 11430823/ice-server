EXE=ice-server.exe
CC=g++

#编译选项
FLAG=-Wall -DDEBUG -g -O0 -o

#包含的头文件
INCLUDES=-I/usr/include -I../include/ -I../libsrc/
GLIB_INCLUDES=-I/usr/local/include/glib-2.0/ -I/usr/local/lib/glib-2.0/include/


LIBS=-L/usr/lib/


LINK=-ldl -lglib-2.0 -lice

#编译的代码文件
LOCAL_SRCS =$(wildcard ./*.c)
OTHER_SRCS =

$(EXE):
	$(CC) $(LOCAL_SRCS) $(OTHER_SRCS) $(INCLUDES) $(GLIB_INCLUDES) $(LIBS) $(LINK) $(FLAG) $@
#	rm ../bin/$(EXE)
	cp $(EXE) ../bin/
	rm -f $(EXE)
clean:
	rm -f $(EXE)

