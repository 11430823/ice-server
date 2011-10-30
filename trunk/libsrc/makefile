EXE=libice.so
CC=g++
FLAG=-Wall -fPIC -shared -O0 -o
INCLUDES=-I/usr/include -I../libinclude
GLIB_INCLUDES=-I/usr/local/include/glib-2.0/ -I/usr/local/lib/glib-2.0/include/
LOCAL_SRCS=$(wildcard ./*.c)
includes=$(wildcard ./*.h)

$(EXE):
	$(CC) $(LOCAL_SRCS) $(INCLUDES) $(GLIB_INCLUDES) $(FLAG) $@
	cp -f $@ ../lib/
	rm $@
	sudo cp ../libinclude/* /usr/include/ice_lib/
	sudo cp ../lib/$@ /usr/lib/
clean:
	rm $(EXE) *.o -f
