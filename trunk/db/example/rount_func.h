/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		路由消息
*********************************************************************/

#pragma once

#include <lib_mysql/Cfunc_route_base.h>

#include "tool.h"

class Croute_func : public Cfunc_route_base
{
public:
	Croute_func(mysql_interface* db) : Cfunc_route_base(db), tool(db){}

	Ctool tool;
	int tool_add(DEAL_FUN_ARG);
	int tool_reduce(DEAL_FUN_ARG);
	int tool_get_all(DEAL_FUN_ARG);
};

int Croute_func::tool_add( cli_proto_head_t& rhead, recv_data_cli_t& rin, char** sendbuf, int* sndlen )
{
	tool_add_in_t info;
	rin>>info.id>>info.num;
	this->ret = this->tool.add(rhead.id, info.id, info.num);
	//todo 返回    STD_RETURN(ret);
	return 0;
}

int Croute_func::tool_reduce( cli_proto_head_t& rhead, recv_data_cli_t& rin, char** sendbuf, int* sndlen )
{
	tool_reduce_in_t info;
	rin>>info.id>>info.num;
	this->ret = this->tool.reduce(rhead.id, info.id, info.num);
	//todo 返回  STD_RETURN(ret);
	return 0;
}

int Croute_func::tool_get_all( cli_proto_head_t& rhead, recv_data_cli_t& rin, char** sendbuf, int* sndlen )
{
	//todo 获取数据
	*sendbuf = this->send_data.data();
	*sndlen =  this->send_data.len();
	return 0;
}