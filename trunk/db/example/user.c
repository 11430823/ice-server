
#include "user.h"

const uint32_t USER_NICK_LEN = 16;

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

int Cuser::get_all( uint32_t userid, send_data_cli_t& out )
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