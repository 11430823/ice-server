
#pragma once 

#include <lib_mysql/CtableRoute.h>

const uint32_t USER_NICK_LEN = 16;

#pragma pack(1)

struct user_add_in_t{
	char nick[USER_NICK_LEN];
};

struct user_get_all_in_t{
};

struct user_get_all_out_t{
	uint32_t userid;
	uint32_t nick_len;
		char nick[];
};

#pragma pack()

class Cuser : public CtableRoute
{
public:
	Cuser(mysql_interface * db) : CtableRoute(db, "ICE", "t_user"){}

	int add(uint32_t userid, char* nick);
	int get_all(uint32_t userid, send_data_cli_t& out);

private:
};