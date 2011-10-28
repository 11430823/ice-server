/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	消息
	brief:		
*********************************************************************/

#pragma once

#include <stdint.h>

enum E_CMD{
	e_min_cmd = 0,
	e_login_cmd = 1,//登陆
	e_max_cmd,
};

//处理函数参数
#define CMD_HANDLE_CLIENT_RAG

typedef int (*FN_HANDLE_CLIENT)(CMD_HANDLE_CLIENT_RAG);

class Cmd
{
public:
	bool is_able(uint16_t cmd){
		return (e_min_cmd < cmd && cmd < e_max_cmd);
	}
public:
	Cmd(){}
	int init_handle();
	static int handle_login(CMD_HANDLE_CLIENT_RAG);
	//virtual ~Cmd(){}
	FN_HANDLE_CLIENT handle_client_fns[e_max_cmd];
protected:
private:
	Cmd(const Cmd &cr);
	Cmd & operator=( const Cmd &cr);
};

