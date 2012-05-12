#sudo apt-get install libmysqlclient-dev
#-lpthread 是多线程库支持

MYSQL_FLAGS=
#-lmysqlclient -I/usr/include/mysql/ -L/usr/lib/mysql/

EXE=libicemysql.so
CC=g++

#编译选项
ICE_LIB_PATH=../../
ICE_LIBS_FLAG=-Wl,-rpath,$(ICE_LIB_PATH)

FLAG=-Wall -DDEBUG -DENABLE_TRACE_LOG -fPIC -shared -O0 -g -o

INCLUDES=-I/usr/include/ -I../../include/lib_mysql/ -I../../include/
GLIB_INCLUDES=-I/usr/include/glib-2.0/ -I/usr/lib/glib-2.0/include/ 

LOCAL_SRCS=$(wildcard ./*.c)
LIBS=-L/usr/lib/ -L$(ICE_LIB_PATH)
LINK=-lglib-2.0 -lice $(ICE_LIBS_FLAG)

$(EXE):
	$(CC) $(LOCAL_SRCS) $(INCLUDES) $(GLIB_INCLUDES) $(LIBS) $(LINK) $(MYSQL_FLAGS) $(FLAG) $@
	cp -f $@ ../../
	rm $@
install:
# 将头文件放入指定的目录中
# 将SO放入指定的目录中
clean:
	rm $(EXE) *.o -f
