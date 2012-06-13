#include "tool.h"

//×Ö¶ÎÃû³Æ
const char* TOOL_USERID = "userid";
const char* TOOL_ID = "id";
const char* TOOL_NUM = "num";


int Ctool::__insert( uint32_t userid, uint32_t id, uint32_t num )
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values (%u, %u, %u)",
		this->get_table_name(userid), userid, id, num);
	return this->exec_insert_sql(this->sqlstr, KEY_EXISTED_ERR);
}

int Ctool::__get_num( uint32_t userid, uint32_t id, uint32_t& num )
{
	num = 0;
	GEN_SQLSTR(this->sqlstr,"select %s from %s where %s=%u and %s=%u",
		TOOL_NUM,
		this->get_table_name(userid),
		TOOL_USERID, userid, 
		TOOL_ID, id);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
	INT_CPY_NEXT_FIELD(num);
	STD_QUERY_ONE_END();
}

int Ctool::__add( uint32_t userid, uint32_t id, uint32_t num )
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s+=%u where %s=%u and %s=%u" ,
		this->get_table_name(userid), 
		TOOL_NUM, num,
		TOOL_USERID, userid,
		TOOL_ID, id);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Ctool::__del( uint32_t userid, uint32_t id, uint32_t num )
{
	GEN_SQLSTR(this->sqlstr, "delete from %s where %s=%u and %s=%u",
		this->get_table_name(userid),
		TOOL_USERID, userid,
		TOOL_ID, id);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Ctool::__reduce( uint32_t userid, uint32_t id, uint32_t num )
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s-=%u where %s=%u and %s=%u",
		this->get_table_name(userid), 
		TOOL_NUM, num,
		TOOL_USERID, userid,
		TOOL_ID, id);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Ctool::add( uint32_t userid, uint32_t id, uint32_t num )
{
	if (0 == num){
		return 0;
	}

	uint32_t has_num = 0;
	__get_num(userid, id, has_num);
	if (has_num > 0){//has
		return __add(userid, id, num);
	} else {//no has
		return __insert(userid, id, num);
	}
	return 0;
}

int Ctool::reduce( uint32_t userid, uint32_t id, uint32_t num )
{
	if (0 == num){
		return 0;
	}
	uint32_t has_num = 0;
	__get_num(userid, id, has_num);

	if (num == has_num){
		return __del(userid, id, num);
	} else if (num < has_num){
		return __reduce(userid, id, num);
	} else {
		//todo ²»×ã,·µ»Ø´íÎóÂë
	}
	return 0;
}

int Ctool::get_all( uint32_t userid, ice::lib_send_data_cli_t& out )
{
	GEN_SQLSTR(this->sqlstr,"select %s,%s from %s where %s=%u",
		TOOL_ID, TOOL_NUM,
		this->get_table_name(userid),
		TOOL_USERID, userid);

	uint32_t cnt = 0;
	out<<cnt;
	uint32_t pos = out.len();

	STD_QUERY_WHILE_BEGIN( this->sqlstr, cnt);
		GET_NEXT_FIELD_INT(out, uint32_t);
		GET_NEXT_FIELD_INT(out, uint32_t);
	STD_QUERY_WHILE_END();

	out.set_count(cnt, pos);
	return this->ret;
}