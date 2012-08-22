/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		����ͻ��˷�������Ϣ
*********************************************************************/

#pragma once
#include <lib_proto/lib_cmd_map.h>
#include <lib_proto/lib_msg.h>

#define  DEAL_FUN_ARG ice::lib_msg_t* c_in

class handle_cli;
//������ú�����ָ������
typedef int (handle_cli::*P_DEALFUN_T)(DEAL_FUN_ARG);

class handle_cli
{
public:
	handle_cli();
	virtual ~handle_cli(){}
	int handle(const void* pkg, int pkglen);
	ice::lib_cmd_map_t<P_DEALFUN_T> cmd_map;
protected:
	
private:
	//��CMD�ʹ�����
	#undef  BIND_PROTO_CMD
	#define BIND_PROTO_CMD(cmd_id, fun_name, p_in)\
		int fun_name(DEAL_FUN_ARG);
	#include "../../protocol/cpp/protocol_online_cmd.h"
	#undef  BIND_PROTO_CMD
	

	handle_cli(const handle_cli& cr);
	handle_cli& operator=(const handle_cli& cr);
};

extern handle_cli g_handle_cli;