/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		表
*********************************************************************/

#pragma once

#include <lib_include.h>

#include "db_macro.h"
#include "proto_header.h"
#include "mysql_iface.h"

#define  GEN_SQLSTR(sqlstr,...)  ::snprintf(sqlstr, sizeof(sqlstr), __VA_ARGS__)
const uint32_t SQL_STR_MAX_LEN = 16384;//sql str 最大长度
const uint32_t DB_NAME_MAX_LEN = 50;
const uint32_t TABLE_NAME_MAX_LEN = 50;
const uint32_t DB_TABLE_NAME_MAX_LEN = DB_NAME_MAX_LEN + TABLE_NAME_MAX_LEN;

class Ctable
{
	PROTECTED_RW(mysql_interface*, db);
	PROTECTED_RW(uint32_t, id);//用于保存主键值，缓存中使用,一般在get_table_name 中同步设置
	PROTECTED_RW(uint32_t, ret);
protected:
	char sqlstr[SQL_STR_MAX_LEN];
	char db_name[DB_NAME_MAX_LEN];
	char table_name[TABLE_NAME_MAX_LEN];
	//: db.table 
	char db_table_name[DB_TABLE_NAME_MAX_LEN];
	char dbser_return_buf[PROTO_MAX_SIZE];
public:
	virtual char*  get_table_name();
	/**
	 * @brief	查询一条记录是否存在
	 */
	int record_is_existed(char* sql_str,  bool& p_existed);
	/**
	 * @brief	更新数据(一行)
	 */
	int exec_update_sql(char* sql_str, int nofind_err);
	/**
	 * @brief	插入数据(一行)
	 */
	int exec_insert_sql(char* sql_str, int existed_err);
	/**
	 * @brief	删除数据(一行)
	 */
	int exec_delete_sql(char* sql_str, int nofind_err);

	/**
	 * @brief	影响行数为多行的接口(insert ,update ,delete,都可以使用)
	 */
	int exec_update_sqls(char* sql_str, int nofind_err);

	Ctable(mysql_interface* db,const char* dbname,const char* tablename); 
	Ctable(mysql_interface * db); 
	virtual ~Ctable(void){}
#if 0
	int select_data( char *&_p_result,	uint32_t &_pkg_len, MYSQL_RES* &res , uint32_t * p_count)
	{

		this->db->set_id(this->id);

		if (DB_SUCC != ( this->db->exec_query_sql(sqlstr,&res))){
			return DB_ERR;
		}

		*p_count=mysql_num_rows(res);
		return SUCC;
	}
	int exec_insert_sql_get_auto_increment_id(char * sql_str, int existed_err,
		uint32_t *p_increment_id )
	{
		int dbret;
		int acount; 

		this->db->set_id(this->id);
		if ((dbret=this->db->exec_update_sql(sql_str,&acount ))==DB_SUCC)
		{
			*p_increment_id=mysql_insert_id(&(this->db->handle));
			return DB_SUCC;
		}else {
			if (dbret==ER_DUP_ENTRY)
				return  existed_err;
			else return DB_ERR;
		}

	}
#endif
};

