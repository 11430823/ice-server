#include "user.h"

//×Ö¶ÎÃû³Æ
const char* USER_USERID = "userid";
const char* USER_NICK = "nick";

int Cuser::add( uint32_t userid, char* nick )
{
	char nick_mysql[mysql_str_len(strlen(nick))];
	set_mysql_string(nick_mysql, nick, strlen(nick));
	GEN_SQLSTR(this->sqlstr,"insert into %s(%s,%s) values(%u,'%s')",
		this->get_table_name(userid),
		USER_USERID, USER_NICK,
		userid, nick_mysql);
	return this->exec_insert_sql(this->sqlstr, KEY_EXISTED_ERR);
}

int Cuser::get_all( uint32_t userid, ice::lib_send_data_cli_t& out )
{
	GEN_SQLSTR(this->sqlstr,"select %s,%s from %s where %s=%u",
		USER_USERID, USER_NICK,
		this->get_table_name(userid),
		USER_USERID, userid);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, KEY_INEXISTENCE_ERR);
		GET_NEXT_FIELD_INT(out, uint32_t);
		GET_NEXT_FIELD_BIN(out, USER_NICK_LEN);
	STD_QUERY_ONE_END();

	return this->ret;
}