/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		����ͻ��˷�������Ϣ
*********************************************************************/

#pragma once

#define CLI_HANDLE_FUN_ARG

class cli_handle_t;
//������ú�����ָ������
typedef int (cli_handle_t::*CLI_FUNC)(CLI_HANDLE_FUN_ARG);

class cli_handle_t
{
public:
	cli_handle_t(){
		this->cmd_map.insert(
		//��ʼ��CMD�ʹ�����
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
	//��CMD�ʹ�����
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