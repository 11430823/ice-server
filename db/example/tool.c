#include "tool.h"

int Ctool::__insert( uint32_t userid, uint32_t id, uint32_t num )
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values (%u, %u, %u)",
		this->get_table_name(userid), userid, id, num);
	return this->exec_insert_sql(this->sqlstr, KEY_EXISTED_ERR);
}

int Ctool::__get_num( uint32_t userid,uint32_t id, uint32_t& num )
{
	num = 0;
	GEN_SQLSTR(this->sqlstr,"select num from %s where userid=%u and id=%u",
		this->get_table_name(userid), userid, id);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
	INT_CPY_NEXT_FIELD(num);
	STD_QUERY_ONE_END();
}

int Ctool::__add( uint32_t userid, uint32_t id, uint32_t num )
{
	GEN_SQLSTR(this->sqlstr, "update %s set number+=%u where userid=%u and id=%u" ,
		this->get_table_name(userid), num, userid, id);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Ctool::__del( uint32_t userid, uint32_t id, uint32_t num )
{
	GEN_SQLSTR(this->sqlstr, "delete from %s where userid=%u and id=%u",
		this->get_table_name(userid), userid, id);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Ctool::__reduce( uint32_t userid, uint32_t id, uint32_t num )
{
	GEN_SQLSTR(this->sqlstr, "update %s set number-=%u where userid=%u and id=%u",
		this->get_table_name(userid), num, userid, id);
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

