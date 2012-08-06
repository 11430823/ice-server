/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		路由消息
*********************************************************************/

#pragma once

#include <lib_mysql/Cfunc_route_base.h>

#include "user.h"
#include "tool.h"

class Cfunc_route : public Cfunc_route_base
{
public:
	Cfunc_route(mysql_interface* db) : Cfunc_route_base(db), user(db), tool(db){
		#undef  BIND_PROTO_CMD
		#define BIND_PROTO_CMD(cmd_id, fun_name)\
			this->cmd_map.insert_cmd_fun(cmd_id, &Cfunc_route::fun_name);
		#include "func_cmd.h"
		#undef  BIND_PROTO_CMD
	}

private:
	Cuser user;
	Ctool tool;

#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmd_id, fun_name)\
	int fun_name(DEAL_FUN_ARG);
#include "func_cmd.h"
#undef  BIND_PROTO_CMD

};