#ifndef MYSQL_IFACE_H
#define MYSQL_IFACE_H

#include <stdlib.h>
#include <inttypes.h>
#include <string>

#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include <mysql/errmsg.h>

#include "db_error_base.h" 
#include "benchapi.h" 

class mysql_interface {
private:
	char	host[16];
	char	user[16];
	char	pass[16];
	char 	unix_socket[255];
	unsigned int port;
	//是否可以将sql打印到debug 日志中
	int  is_log_debug_sql;
	//
	bool is_test_env;
	std::string select_db_name_fix;
	std::string select_db_name;

	//是否访问预设的db
	bool  is_select_db_succ;

	//是否只能运行sql语句
	int  is_only_exec_select_sql;

	int execsql(const char* cmd);
	int connect_server ();
	 
public:
	MYSQL	handle; 
	uint32_t id;
	mysql_interface ( const char *h, const char *u, const char *p, unsigned int port=3306,
		  const char * a_unix_socket=NULL );	
	~mysql_interface ();
	inline bool get_is_select_db_succ(){
		return this->is_select_db_succ;
	}
	
	inline void set_is_test_env(bool value){
		this->is_test_env=value;
	}
	


	
	const int get_errno() { return mysql_errno(&handle); }
	const char*  get_error() { return mysql_error(&handle); }
	void  show_error_log(const char *cmd); 
	void  set_is_log_debug_sql(int value ); 
	void  set_is_only_exec_select_sql(int value ){
		this->is_only_exec_select_sql=value;
	};

	int  select_db (char *db_name ); 

	//设置这个命令要查询的DB名字前缀,如USER_00->USER_
	inline void set_select_db_str_fix (const char *db_name_fix ){
		this->select_db_name_fix=db_name_fix;
		this->select_db_name="";
		this->is_select_db_succ=true;
	}

	int exec_query_sql (const char *cmd, MYSQL_RES **result);
	int exec_update_sql(const char *cmd, int* affected_rows );

};
#endif
