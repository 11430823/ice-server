EXE=libice.so
CC=g++
FLAG=-Wall -fPIC -shared -O0 -o
INCLUDES=-I/usr/include/ -I../libinclude/ -I/usr/include/libxml2/
GLIB_INCLUDES=-I/usr/local/include/glib-2.0/ -I/usr/local/lib/glib-2.0/include/
LINK=-lxml2
LOCAL_SRCS=$(wildcard ./*.c)
includes=$(wildcard ./*.h)

$(EXE):
	$(CC) $(LOCAL_SRCS) $(INCLUDES) $(GLIB_INCLUDES) $(LINK) $(FLAG) $@
	cp -f $@ ../lib/
	rm $@
clean:
	rm $(EXE) *.o -f
