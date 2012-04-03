/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	接口
	brief:		ok
*********************************************************************/

#pragma once

// #include <sys/types.h>
// #include <netinet/in.h>

#include <lib_util.h>
#include <lib_tcp_server_epoll.h>
#if 0 //todo 增加一个处理PIPE管道消息的回调函数
int handle_pipe_event(int fd, epoll_event& r_evs);
#endif
class ice_dll_t
{
	PRIVATE_READONLY_DEFAULT(void*, handle);
public:
	ice_dll_t();
	virtual ~ice_dll_t();
	ice::on_functions_tcp_server_epoll functions;
	//************************************
	// Brief:	注册插件
	// Returns:	int 0:success -1:error
	//************************************
	int  register_plugin();
protected:
private:
};

extern ice_dll_t g_dll;