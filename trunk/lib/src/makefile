#-lpthread 是 多线程库支持
#MYSQL_FLAGS=-lmysqlclient -I/usr/include/mysql/ -L/usr/lib/mysql/

EXE=libice.so
CC=g++
FLAG=-Wall -DENABLE_TRACE_LOG -fPIC -shared -O0 -g -o
INCLUDES=-I/usr/include/ -I../include/ -I./ -I/usr/include/libxml2/
GLIB_INCLUDES=-I/usr/include/glib-2.0/ -I/usr/lib/glib-2.0/include/ 
LINK=-lxml2
LOCAL_SRCS=$(wildcard ./*.c)
#LOCAL_INCLUDES=$(wildcard ../include/*.h)
LOCAL_SRCS_NET=$(wildcard ./lib_net/*.c)

$(EXE):
	$(CC) $(LOCAL_SRCS) $(LOCAL_SRCS_NET) $(INCLUDES) $(GLIB_INCLUDES) $(LINK) $(FLAG) $@
	cp -f $@ ../
	rm $@
install:
# 将头文件放入指定的目录中
# 将SO放入指定的目录中
clean:
	rm $(EXE) *.o -f
