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

enum E_DB_CMD{
	E_DB_CMD_BEGIN = 0,
	//user
	E_DB_CMD_USER_ADD = 0xFFFFF001,
	//tool
	E_DB_CMD_TOOL_ADD = 0xFFFFF001,
	E_DB_CMD_TOOL_REDUCE = 0xFFFFF001,
	E_DB_CMD_TOOL_GET_ALL = 0xFFFFF001,
	E_DB_CMD_END,	
};

class Cfunc_route : public Cfunc_route_base
{
public:
	Cfunc_route(mysql_interface* db) : Cfunc_route_base(db), tool(db){
		this->cmd_map.insert_cmd_fun(E_DB_CMD_TOOL_ADD, &Cfunc_route::tool_add);
  		this->cmd_map.insert_cmd_fun(E_DB_CMD_TOOL_REDUCE, &Cfunc_route::tool_reduce);
  		this->cmd_map.insert_cmd_fun(E_DB_CMD_TOOL_GET_ALL, &Cfunc_route::tool_get_all);
	}

	//user
	Cuser user;

	//tool
	Ctool tool;
	int tool_add(DEAL_FUN_ARG);
	int tool_reduce(DEAL_FUN_ARG);
	int tool_get_all(DEAL_FUN_ARG);
};

int Cfunc_route::tool_add(DEAL_FUN_ARG)
{
	tool_add_in_t info;
	in>>info.id>>info.num;
	this->ret = this->tool.add(head.id, info.id, info.num);
	
	return this->ret;
}

int Cfunc_route::tool_reduce(DEAL_FUN_ARG)
{
	tool_reduce_in_t info;
	in>>info.id>>info.num;
	this->ret = this->tool.reduce(head.id, info.id, info.num);

	return this->ret;
}

int Cfunc_route::tool_get_all(DEAL_FUN_ARG)
{
	this->tool.get_all(head.id, this->send_data);
	this->send_data.set_head(head);
	*sendbuf = this->send_data.data();
	sndlen =  this->send_data.len();
	return 0;
}