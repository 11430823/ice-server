/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		网络服务接口
*********************************************************************/
#pragma  once

#include <lib_util.h>
#include <lib_tcp_server_epoll.h>

class net_server_t
{
	PRIVATE_R_DEFAULT(ice::lib_tcp_server_epoll_t*, server_epoll);
public:
	net_server_t(void);
	virtual ~net_server_t(){}
	//************************************
	// Brief:     
	// Returns:   int 0:success,-1:error
	//************************************
	int create(uint32_t max_fd_num);
	int destroy();
protected:
private:
	net_server_t(const net_server_t &cr);
	net_server_t & operator=( const net_server_t &cr);
};

extern net_server_t g_net_server;
