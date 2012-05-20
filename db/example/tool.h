
#pragma once

#include <lib_mysql/CtableRoute.h>

#pragma pack(1)
struct tool_add_in_t 
{
	uint32_t id;
	uint32_t num;
};

typedef tool_add_in_t tool_reduce_in_t;

struct tool_get_all_in_t 
{
};

struct tool_get_all_out_t 
{
	uint32_t id;
	uint32_t num;
};
#pragma pack()

class Ctool : public CtableRoute
{
public:
	Ctool(mysql_interface * db) : CtableRoute(db, "ICE", "t_tool"){}
	int add(uint32_t userid, uint32_t id, uint32_t num);
	int reduce(uint32_t userid, uint32_t id, uint32_t num);
private:
	int __get_num(uint32_t userid,uint32_t id, uint32_t& num);
	int __insert(uint32_t userid, uint32_t id, uint32_t num);
	int __add(uint32_t userid, uint32_t id, uint32_t num);
	int __reduce(uint32_t userid, uint32_t id, uint32_t num);
	int __del(uint32_t userid, uint32_t id, uint32_t num);
};