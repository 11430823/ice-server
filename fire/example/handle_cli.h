/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		处理客户端发来的消息
*********************************************************************/

#pragma once

#define  DEAL_FUN_ARG

class handle_cli;
//定义调用函数的指针类型
typedef int (handle_cli::*P_DEALFUN_T)(DEAL_FUN_ARG);

class handle_cli
{
public:
	handle_cli(){
		//初始化CMD和处理函数
		#undef  BIND_PROTO_CMD
		#define BIND_PROTO_CMD(cmdid, fun_name)\
			this->cmd_map.insert_cmd_fun(cmdid, &handle_cli::fun_name);
//todo		#include "./protocol/protocol_online_cmd.h"
		#undef  BIND_PROTO_CMD
	}
	virtual ~handle_cli(){}
	int handle(const void* pkg, int pkglen);
protected:
	
private:
	//绑定CMD和处理函数
	#undef  BIND_PROTO_CMD
	#define BIND_PROTO_CMD(cmdid, fun_name)\
		int fun_name(DEAL_FUN_ARG);
//todo	#include "./protocol/protocol_online_cmd.h"
	#undef  BIND_PROTO_CMD

	handle_cli(const handle_cli& cr);
	handle_cli& operator=(const handle_cli& cr);
};

extern handle_cli g_handle_cli;