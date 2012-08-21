/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		mysql接口
*********************************************************************/

#pragma once

#include <lib_include.h>

#include <mysql/mysql.h>

#include <lib_util.h>

class mysql_interface 
{
	PRIVATE_R(std::string, host);
	PRIVATE_R(uint16_t, port);
	PRIVATE_R(std::string, user);
	PRIVATE_R(std::string, pass);
	PRIVATE_RW(bool, is_log_sql);//是否可以将sql打印到日志中
	PRIVATE_RW(uint32_t, id);//todo ?
public:
	MYSQL handle;
	mysql_interface (const std::string& h, const std::string& user, const std::string& pass, uint16_t port = 3306,
		bool log_sql = false, const char * a_unix_socket = NULL);
	virtual ~mysql_interface();
	void show_error_log(const char* sql);
	int select_db(char* db_name);
	int exec_query_sql (const char* sql, MYSQL_RES** result);
	int exec_update_sql(const char* sql, int* affected_rows);
private:
	char unix_socket[255];
	int connect_server();
	int execsql(const char* sql);
	const int get_errno(){return mysql_errno(&handle);}
	const char*  get_error(){return mysql_error(&handle);}
};