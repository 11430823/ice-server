/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		
*********************************************************************/
#pragma  once

#include <lib_util.h>
#include <lib_tcp_server_epoll.h>

class net_server_t
{
	PRIVATE_READONLY_DEFAULT(ice::lib_tcp_server_epoll_t*, server_epoll);
public:
	struct bind_config_elem_t* bc_elem;
public:
	net_server_t(void){
		this->bc_elem = NULL;
		this->server_epoll = NULL;
	}
	virtual ~net_server_t(){

	}
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
