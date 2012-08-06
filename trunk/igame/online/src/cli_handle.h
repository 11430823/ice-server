/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		处理客户端发来的消息
*********************************************************************/

#pragma once

#define CLI_HANDLE_FUN_ARG

class cli_handle_t;
//定义调用函数的指针类型
typedef int (cli_handle_t::*CLI_FUNC)(CLI_HANDLE_FUN_ARG);

class cli_handle_t
{
public:
	cli_handle_t(){
		this->cmd_map.insert(
		//初始化CMD和处理函数
		#undef  BIND_PROTO_CMD
		#define BIND_PROTO_CMD(cmd_id, fun_name)\
			this->cmd_map.insert(cmd_id, &cli_handle_t::fun_name);
			
			#include "../../protocol/online_cmd.h"
		#undef  BIND_PROTO_CMD
	}
	virtual ~cli_handle_t(){}
	int handle(const void* pkg, int pkglen);
protected:
	
private:
	//绑定CMD和处理函数
	#undef  BIND_PROTO_CMD
	#define BIND_PROTO_CMD(cmd_id, fun_name)\
		int fun_name(DEAL_FUN_ARG);
		
		#include "./protocol/online_cmd.h"
	#undef  BIND_PROTO_CMD
	ice::lib_cmd_map_t<CLI_FUNC> cmd_map;
	cli_handle_t(const cli_handle_t& cr);
	cli_handle_t& operator=(const cli_handle_t& cr);
};

extern cli_handle_t g_cli_handle;