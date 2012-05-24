
#pragma once 

#include <lib_mysql/CtableRoute.h>

extern const uint32_t USER_NICK_LEN;

#pragma pack(1)

struct user_add_in_t 
{
	uint32_t id;
	char nick[USER_NICK_LEN];
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