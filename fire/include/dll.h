/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	接口
	brief:		ok
*********************************************************************/

#pragma once

#include <lib_include.h>

#include <lib_util.h>

#include "net_tcp.h"

class dll_t
{
	PRIVATE_R(void*, handle);
public:
	dll_t();
	virtual ~dll_t();
	on_functions_tcp_server_epoll functions;
	/**
	 * @brief	注册插件
	 * @return	int
	 */
	int  register_plugin();
	/**
	 * @brief	处理PIPE管道消息的回调函数
	 * @param	int fd
	 * @return	int
	 */
	static int on_pipe_event(int fd, epoll_event& r_evs);

protected:
private:
};

extern dll_t g_dll;

