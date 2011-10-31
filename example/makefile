EXE=liblogic.so
CC=g++

#编译选项
#FLAG=-Wall -DDEBUG -S
FLAG=-Wall -fPIC -MMD -shared -DDEBUG -g -O0 -o

#包含的头文件
INCLUDES=-I/usr/include/ -I../include/ -I../libinclude/
GLIB_INCLUDES=-I/usr/local/include/glib-2.0/ -I/usr/local/lib/glib-2.0/include/


LIBS=-L/usr/lib/ -L../lib/

#LINK=-ldl -lglib-2.0
LINK=-lglib-2.0 -lice

#编译的代码文件
LOCAL_SRCS =$(wildcard ./*.c)
OTHER_SRCS =

$(EXE):
	rm -f $(EXE)
	$(CC) -ldl -lrt $(LOCAL_SRCS) $(OTHER_SRCS) $(INCLUDES) $(GLIB_INCLUDES) $(LINK) $(LIBS) $(FLAG) $@ 
	cp $(EXE) ../bin/
	rm -f $(EXE)
clean:
	rm -f $(EXE)

