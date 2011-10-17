EXE=libice.so
CC=g++
FLAG=-Wall -fPIC -shared -O0 -o
INCLUDES=-I/usr/include
LOCAL_SRCS=$(wildcard ./*.c)
includes=$(wildcard ./*.h)

$(EXE):
	$(CC) $(LOCAL_SRCS) $(INCLUDES) $(FLAG) $@
	cp -f $(includes) ../include/
	cp -f $@ ../lib/
	rm $@
clean:
	rm $(EXE) *.o -f
