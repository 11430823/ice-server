
#pragma once

#include <stdlib.h>
#include <inttypes.h>
#include <string>

#include <mysql/mysql.h>

#include <lib_util.h>

class mysql_interface 
{
	PRIVATE_R_DEFAULT(std::string, host);
	PRIVATE_R_DEFAULT(std::string, user);
	PRIVATE_R_DEFAULT(std::string, pass);
	PRIVATE_R_DEFAULT(uint16_t, port);
	PRIVATE_RW_DEFAULT(bool, is_log_sql);//是否可以将sql打印到日志中
	PRIVATE_RW_DEFAULT(uint32_t, id);

private:
	char unix_socket[255];
	int connect_server();
	int execsql(const char* sql);
	const int get_errno(){return mysql_errno(&handle);}
	const char*  get_error(){return mysql_error(&handle);}
public:
	MYSQL handle;
	mysql_interface (std::string h, std::string user, std::string pass, uint16_t port = 3306,
		const char * a_unix_socket = NULL);
	~mysql_interface();
	void show_error_log(const char* sql);
	int select_db(char* db_name); 
	int exec_query_sql (const char* sql, MYSQL_RES** result);
	int exec_update_sql(const char* sql, int* affected_rows);
};