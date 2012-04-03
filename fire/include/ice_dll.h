/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	接口
	brief:		ok
*********************************************************************/

#pragma once

#include <sys/types.h>
#include <netinet/in.h>

#pragma pack(1)

struct fdsession_t {
	int			fd;
	in_port_t	remote_port;
	uint32_t	remote_ip;
	uint8_t		type;
	uint8_t		flag;
	time_t		last_tm;
};

#pragma pack()

class ice_dll_t
{
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
	void* m_handle;
};

extern ice_dll_t g_dll;