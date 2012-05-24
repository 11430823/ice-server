/*
 * =====================================================================================
 *
 *       Filename:  Ctable.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include <mysql/mysqld_error.h>

#include <lib_log.h>

#include "Ctable.h"

Ctable::Ctable(mysql_interface* db, const char* dbname, const char* tablename ) 
{ 
	this->db = db;
	::strcpy(this->table_name, tablename);
	::strcpy(this->db_name, dbname);
	this->ret = 0;
}

Ctable::Ctable(mysql_interface* db)
{
	this->db = db;
	this->ret = 0;
}

char* Ctable::get_table_name()
{
	this->db->select_db(this->db_name);

	::sprintf(this->db_table_name, "%s.%s", this->db_name, this->table_name);
	return this->db_table_name;
}

int Ctable::exec_delete_sql(char* sql_str, int nofind_err)
{
	return this->exec_update_sql(sql_str, nofind_err);
}

int Ctable::exec_update_sql(char* sql_str, int nofind_err )
{
	int acount; 
	this->db->set_id(this->id);
	if (0 == this->db->exec_update_sql(sql_str, &acount)){
		if (1 == acount){
			return 0;	
		}else{
			return nofind_err; 
		}
	}else {
		return DB_ERR;
	} 
}	

int  Ctable::record_is_existed(char* sql_str, bool& p_existed)
{
	p_existed = false;
	STD_QUERY_ONE_BEGIN(sql_str, 0);
		p_existed=true;
	STD_QUERY_ONE_END();
}

int Ctable::exec_insert_sql(char* sql_str, int existed_err )
{
	this->db->set_id(this->id);
 	int dbret;
	int acount; 
	if (0 == (dbret = this->db->exec_update_sql(sql_str, &acount))){
		return 0;
	}else {
		if (ER_DUP_ENTRY == dbret){
			return  existed_err;
		} else {
			return DB_ERR;
		}
	}
}

int Ctable::exec_update_sqls(char * sql_str, int nofind_err )
{
	int acount; 
	this->db->set_id(this->id);
	if (0 == this->db->exec_update_sql(sql_str, &acount)){
		return 0;
	}
	return DB_ERR;
}