EXE=libdbser.so
CC=g++

MYSQL_FLAGS=-lmysqlclient -I/usr/include/mysql/ -L/usr/lib/mysql/

#编译选项
ICE_LIB_PATH=../../lib/
ICE_LIBS_FLAG=-Wl,-rpath,$(ICE_LIB_PATH)
FLAG=-Wall -fPIC -MMD -shared -DDEBUG -DENABLE_TRACE_LOG -g -O0 -o

#包含的头文件
INCLUDES=-I/usr/include/ -I../include/ -I../../lib/include/ -I../../fire/include/
GLIB_INCLUDES=-I/usr/include/glib-2.0/ -I/usr/lib/glib-2.0/include/ 

LIBS=-L/usr/lib/ -L$(ICE_LIB_PATH)

LINK=-lglib-2.0 -lice $(ICE_LIBS_FLAG) -licemysql $(ICE_LIBS_FLAG)

#编译的代码文件
LOCAL_SRCS =$(wildcard ./*.c)
OTHER_SRCS =$(wildcard ../src/*.c)

$(EXE):
	rm -f $(EXE)
	$(CC) -ldl -lrt $(LOCAL_SRCS) $(OTHER_SRCS) $(INCLUDES) $(GLIB_INCLUDES) $(LINK) $(MYSQL_FLAGS) $(LIBS) $(FLAG) $@ 
	cp $(EXE) ../bin/
	rm -f $(EXE)
clean:
	rm -f $(EXE)
