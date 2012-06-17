pid=`ps -ef | grep "dbser.exe" | grep -v "grep" | awk '{print $2}'`
kill -TERM $pid
echo $pid
#kill -9 $pid
