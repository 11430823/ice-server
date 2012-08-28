#-lpthread 是多线程库支持

EXE=libice.so
#请在用户目录下创建ice文件夹,用来存放so
USER_NAME=$(shell whoami)
TAG_DIR=/home/$(USER_NAME)/ice/
CC=g++
FLAG=-Wall -DENABLE_TRACE_LOG -fPIC -shared -O0 -g -o
LOCAL_INCLUDE=../include/
INCLUDES=-I/usr/include/ -I$(LOCAL_INCLUDE) -I./ -I/usr/include/libxml2/
GLIB_INCLUDES=-I/usr/include/glib-2.0/ -I/usr/lib/glib-2.0/include/ 
LINK=-lxml2 -lglib-2.0
LOCAL_SRCS=$(wildcard ./*.c)
LOCAL_SRCS_NET=$(wildcard ./lib_net/*.c)
LOCAL_SRCS_WEBSOCKET=$(wildcard ./lib_websocket/*.c)

$(EXE):
	$(CC) $(LOCAL_SRCS) $(LOCAL_SRCS_NET) $(LOCAL_SRCS_WEBSOCKET) $(INCLUDES) $(GLIB_INCLUDES) $(LINK) $(FLAG) $@
	cp -f $@ $(TAG_DIR)
	cp -rf $(LOCAL_INCLUDE) $(TAG_DIR)
	rm $@
install:
# 将头文件放入指定的目录中
# 将SO放入指定的目录中
clean:
	rm $(EXE) *.o -f
