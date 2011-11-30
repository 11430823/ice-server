pid=`ps -ef | grep "ice-server.exe" | awk '{print $2}'`
kill -TERM $pid
kill -9 $pid
