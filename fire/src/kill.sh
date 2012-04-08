pid=`ps -ef | grep "fire-server.exe" | grep -v "grep" | awk '{print $2}'`
kill -TERM $pid
#kill -9 $pid
