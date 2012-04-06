#include <lib_util.h>
#include <lib_log.h>

#include "net_tcp.h"

net_server_t g_net_server;

int net_server_t::create(uint32_t max_fd_num)
{
	this->server_epoll = new ice::lib_tcp_server_epoll_t(max_fd_num);
	if (NULL == this->server_epoll){
		ALERT_LOG("new tcp_server_epoll err [maxevents:%u]", max_fd_num);
		return -1;
	}
	int ret = 0;
	if (0 != (ret = this->server_epoll->create())){
		ALERT_LOG("new tcp_server_epoll create err [ret:%d]", ret);
		return -1;
	}

	return 0;
}

int net_server_t::destroy()
{
	safe_delete(this->server_epoll);
	return 0;
}

net_server_t::net_server_t( void )
{
	this->server_epoll = NULL;
}



