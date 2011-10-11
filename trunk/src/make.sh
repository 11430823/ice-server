cd ../example/
make
cd -

make

pkill -9 ice-server.exe
cd ../bin/
./ice-server.exe
cd -
