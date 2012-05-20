#!/bin/sh

user="root"
password="root"
host="localhost"
tmp_file="table.sql"
dbname_prefix="ICE_"

todo_sql(){
cat << EOF > $tmp_file
	CREATE DATABASE IF NOT EXISTS $1
EOF
}

db_index=0
db_end_index=100
while [ $db_index -lt $db_end_index ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbname_prefix$dbx
	todo_sql $dbname_prefix$dbx
	cat $tmp_file | mysql -u $user --password="$password" -h $host
	db_index=`expr $db_index + 1`
done
