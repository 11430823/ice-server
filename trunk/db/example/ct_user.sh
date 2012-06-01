#!/bin/bash

user="root"
password="root"
host="localhost"
tmp_file="table.sql"
table_name_prefix="t_user_"
db_name_prefix="ICE_"

todo_sql() {
#	drop table if exists $table_name_prefix$1;
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS $table_name_prefix$1 (
	userid int(10) unsigned  not null ,
	nick char(16) not null,
	primary key(userid)
	) ENGINE=innodb CHARSET=UTF8;
EOF
}

db_index=0
db_end_index=100
table_end_index=100
echo $db_index $db_end_index

while [ $db_index -lt $db_end_index ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	table_index=0
	while [ $table_index -lt $table_end_index ] ; do
		tbx=`printf "%02d" $table_index`
		todo_sql $tbx
		cat $tmp_file | mysql -u $user --password="$password" -h $host "$db_name_prefix$dbx" 
		table_index=`expr $table_index + 1`
	done
	db_index=`expr $db_index + 1`
done

