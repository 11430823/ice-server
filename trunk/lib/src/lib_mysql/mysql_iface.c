#include <mysql/mysqld_error.h>
#include <mysql/errmsg.h>

#include <lib_log.h>
#include <lib_err_code.h>

#include "mysql_iface.h"

mysql_interface::mysql_interface (const std::string& h, const std::string& user,
								  const std::string& pass,
								  uint16_t port, bool log_sql, const char * a_unix_socket)
{
	this->id = 0;
	this->host = h;
	this->user = user;
	this->pass = pass;
	this->port = port;
	this->is_log_sql = log_sql;
	memset(this->unix_socket, 0, sizeof(this->unix_socket));

	mysql_init(&handle);

	if (a_unix_socket!=NULL) {
		SAFE_STRNCPY(this->unix_socket , a_unix_socket);
	}

	DEBUG_LOG("======DB conn msg [host:%s][user:%s][pass:%s][port:%u][is_log_sql:%u][unix_socket:%s]======",
		this->host.c_str(), this->user.c_str(), this->pass.c_str(), 
		this->port, this->is_log_sql, this->unix_socket);
	connect_server();
}

void  mysql_interface::show_error_log(const char* sql) 
{
	ERROR_LOG("sqlerr[errno:%d, error:%s, sql:%s]", this->get_errno(), this->get_error(), sql);
}

mysql_interface::~mysql_interface ()
{
	mysql_close(&handle);
}

int mysql_interface::connect_server ()
{
	char* us = NULL;
	if ('\0' != this->unix_socket[0]){
		us = this->unix_socket;
	}
	//CLIENT_FOUND_ROWS	//Return the number of found (matched) rows, not the number of changed rows.
	unsigned long flag = CLIENT_FOUND_ROWS;
	if (!mysql_real_connect(&this->handle, this->host.c_str(), 
		this->user.c_str(), this->pass.c_str(), NULL, this->port, us, flag)){
		ERROR_LOG("db connect is err [%d, %s]\n", this->get_errno(), this->get_error());
		return DB_ERR;
	}
	DEBUG_LOG("db connect is ok [%d]\n", this->get_errno());

	//关闭自动提交.提高性能.但是一定要手动提交.
	mysql_autocommit(&this->handle, 0);

	return 0;
}

int mysql_interface::exec_query_sql (const char* sql, MYSQL_RES** result)
{
	if (0 == this->execsql(sql)){
		if(NULL == (*result = mysql_store_result(&this->handle))){
			this->show_error_log(sql);
		} else {
			return 0;
		}
	}
	return DB_ERR;
}

int mysql_interface::exec_update_sql(const char* sql,int* affected_rows)
{
	if (0 == this->execsql(sql)){
		*affected_rows = mysql_affected_rows(&(this->handle));
		return 0;
	}
	return this->get_errno();
}

int mysql_interface::execsql(const char* sql)
{
	if (this->is_log_sql){
		KDEBUG_LOG(this->id ,"sql:[%s]", sql);
	}

	int ret = mysql_query(&handle, sql);
	if (0 == ret){//SUCC
		return ret;
	} else if (CR_SERVER_GONE_ERROR == this->get_errno()){//server go away, must reconnect
		if (0 == this->connect_server()){
			ret = mysql_query(&handle, sql);
			if (0 != ret){
				ret = this->get_errno(); 
			}
		}else{
			ret = this->get_errno(); 
		}
	}else{
		this->show_error_log(sql);
		ret = this->get_errno();
	}	
	return ret;
}

int mysql_interface::select_db(char* db_name)
{
	int ret = mysql_select_db(&this->handle, db_name);
	if (0 != ret) {
		this->show_error_log(db_name);
	}
	return this->get_errno();
}